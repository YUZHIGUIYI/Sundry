//
// Created by ZZK on 2025/6/29.
//

#include <windows.h>
#include <gl/GL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>

// 原始OpenGL函数指针
typedef void* (WINAPI* PFNGLMAPBUFFER)(GLenum target, GLenum access);
typedef void (WINAPI* PFNGLDRAWARRAYS)(GLenum mode, GLint first, GLsizei count);
typedef void (WINAPI* PFNGLUNMAPBUFFER)(GLenum target);
typedef void (WINAPI* PFNGLGETBUFFERPARAMETERIV)(GLenum target, GLenum pname, GLint* params);
typedef void (WINAPI* PFNGLGETINTEGERV)(GLenum pname, GLint* data);

// 全局变量
PFNGLMAPBUFFER Real_glMapBuffer = nullptr;
PFNGLDRAWARRAYS Real_glDrawArrays = nullptr;
PFNGLUNMAPBUFFER Real_glUnmapBuffer = nullptr;
PFNGLGETBUFFERPARAMETERIV Real_glGetBufferParameteriv = nullptr;
PFNGLGETINTEGERV Real_glGetIntegerv = nullptr;

// 内存映射信息结构
struct MappedBuffer
{
    GLuint buffer_id;    // 缓冲区对象ID
    void* real_ptr;      // OpenGL原始缓冲区指针
    void* guard_ptr;     // 监视内存（带MEM_WRITE_WATCH）
    size_t size;         // 缓冲区大小
    bool has_writes;     // 是否有写入发生
    bool is_mapped;      // 是否已映射
};

std::unordered_map<GLuint, MappedBuffer> g_mappedBuffers; // 按缓冲区ID索引的映射缓冲区

// 生成唯一文件名
std::string GenerateDumpFilename()
{
    time_t now = time(nullptr);
    tm timeinfo{};
    localtime_s(&timeinfo, &now);

    char buffer[256] = {};
    strftime(buffer, sizeof(buffer), "buffer_dump_%Y%m%d_%H%M%S_", &timeinfo);

    static int counter = 0;
    return std::string(buffer) + std::to_string(counter++) + ".bin";
}

// 手动dump数据函数
void DumpBufferData(void* data, size_t size, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open())
    {
        file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
        std::cout << "Dumped " << size << " bytes to " << filename << '\n';
    } else
    {
        std::cerr << "Failed to dump buffer to " << filename << '\n';
    }
}

// 获取当前绑定的缓冲区ID
GLuint GetBoundBufferID(GLenum target)
{
    GLint binding_point = 0;

    switch (target)
    {
        case GL_ARRAY_BUFFER:
            Real_glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding_point);
            break;
        case GL_ELEMENT_ARRAY_BUFFER:
            Real_glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &binding_point);
            break;
        case GL_PIXEL_PACK_BUFFER:
            Real_glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &binding_point);
            break;
        case GL_PIXEL_UNPACK_BUFFER:
            Real_glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &binding_point);
            break;
        case GL_COPY_READ_BUFFER:
            Real_glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &binding_point);
            break;
        case GL_COPY_WRITE_BUFFER:
            Real_glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &binding_point);
            break;
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            Real_glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &binding_point);
            break;
        case GL_UNIFORM_BUFFER:
            Real_glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &binding_point);
            break;
        default:
            std::cerr << "Unsupported buffer target: " << target << '\n';
            break;
    }

    return static_cast<GLuint>(binding_point);
}

// 替换glMapBuffer
void* WINAPI Hooked_glMapBuffer(GLenum target, GLenum access)
{
    if (!Real_glMapBuffer)
    {
        Real_glMapBuffer = (PFNGLMAPBUFFER)wglGetProcAddress("glMapBuffer");
    }
    if (!Real_glGetBufferParameteriv)
    {
        Real_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIV)wglGetProcAddress("glGetBufferParameteriv");
    }
    if (!Real_glGetIntegerv)
    {
        Real_glGetIntegerv = (PFNGLGETINTEGERV)wglGetProcAddress("glGetIntegerv");
    }

    // 获取当前绑定的缓冲区ID
    GLuint buffer_id = GetBoundBufferID(target);
    if (buffer_id == 0)
    {
        std::cerr << "No buffer bound to target: " << target << '\n';
        return Real_glMapBuffer(target, access);
    }

    // 检查是否已映射
    auto it = g_mappedBuffers.find(buffer_id);
    if (it != g_mappedBuffers.end() && it->second.is_mapped)
    {
        std::cerr << "Buffer " << buffer_id << " is already mapped" << '\n';
        return Real_glMapBuffer(target, access);
    }

    void* real_ptr = Real_glMapBuffer(target, access);
    if (!real_ptr) return nullptr;

    // 查询缓冲区大小
    GLint size = 0;
    Real_glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    if (size <= 0)
    {
        std::cerr << "Invalid buffer size: " << size << '\n';
        Real_glUnmapBuffer(target);
        return nullptr;
    }

    // 分配监视内存（带写入监视标志）
    void* guard_ptr = VirtualAlloc(
        nullptr,
        size,
        MEM_COMMIT | MEM_WRITE_WATCH,
        PAGE_READWRITE
    );

    if (!guard_ptr)
    {
        std::cerr << "Failed to allocate guarded memory" << '\n';
        Real_glUnmapBuffer(target);
        return real_ptr;
    }

    // 初始化内存（复制当前GPU缓冲区内容）
    memcpy(guard_ptr, real_ptr, size);

    // 保存映射信息
    MappedBuffer buffer_info{
        buffer_id,
        real_ptr,
        guard_ptr,
        static_cast<size_t>(size),
        false, // 初始无写入
        true   // 标记为已映射
    };

    g_mappedBuffers[buffer_id] = buffer_info;

    // 返回监视指针给客户端
    return guard_ptr;
}

// 替换glUnmapBuffer
void WINAPI Hooked_glUnmapBuffer(GLenum target)
{
    if (!Real_glUnmapBuffer)
    {
        Real_glUnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFER>(wglGetProcAddress("glUnmapBuffer"));
    }
    if (!Real_glGetIntegerv)
    {
        Real_glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERV>(wglGetProcAddress("glGetIntegerv"));
    }

    // 获取当前绑定的缓冲区ID
    GLuint buffer_id = GetBoundBufferID(target);
    if (buffer_id == 0)
    {
        std::cerr << "No buffer bound to target: " << target << '\n';
        Real_glUnmapBuffer(target);
        return;
    }

    auto it = g_mappedBuffers.find(buffer_id);
    if (it == g_mappedBuffers.end() || !it->second.is_mapped)
    {
        std::cerr << "Buffer " << buffer_id << " not mapped or not found" << '\n';
        Real_glUnmapBuffer(target);
        return;
    }

    MappedBuffer& buffer = it->second;

    // 同步所有数据到GPU原始内存
    memcpy(buffer.real_ptr, buffer.guard_ptr, buffer.size);

    // 释放内存
    VirtualFree(buffer.guard_ptr, 0, MEM_RELEASE);

    // 从映射表中移除
    g_mappedBuffers.erase(it);

    // 调用原始解映射函数
    Real_glUnmapBuffer(target);
}

// 检测写入并同步到GPU
void ProcessWritesAndDump(MappedBuffer& buffer)
{
    // 获取系统信息
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    const DWORD page_size = sys_info.dwPageSize;

    // 准备获取写入地址
    ULONG_PTR page_count = 0;
    DWORD granularity = 0;

    // 第一次调用获取实际页数
    UINT result = GetWriteWatch(
        WRITE_WATCH_FLAG_RESET,
        buffer.guard_ptr,
        buffer.size,
        nullptr,  // 空指针获取所需页数
        &page_count,
        &granularity
    );

    if (result != ERROR_MORE_DATA || page_count == 0)
    {
        return; // 无写入或出错
    }

    // 分配页列表缓冲区
    std::vector<PVOID> written_pages(page_count);

    // 实际获取写入页
    result = GetWriteWatch(
        WRITE_WATCH_FLAG_RESET,
        buffer.guard_ptr,
        buffer.size,
        written_pages.data(),
        &page_count,
        &granularity
    );

    if (result != 0 || page_count == 0)
    {
        return; // 无写入或出错
    }

    // 标记有写入发生
    buffer.has_writes = true;

    // 处理每个写入区域
    for (ULONG_PTR i = 0; i < page_count; ++i)
    {
        PVOID page_base = written_pages[i];

        // 确保地址在缓冲区内
        if (page_base < buffer.guard_ptr ||
            static_cast<BYTE *>(page_base) >= static_cast<BYTE *>(buffer.guard_ptr) + buffer.size)
        {
            continue;
        }

        // 计算页内偏移和大小
        size_t page_offset = static_cast<BYTE *>(page_base) - static_cast<BYTE *>(buffer.guard_ptr);
        size_t copy_size = min(page_size, buffer.size - page_offset);

        // 同步到GPU原始内存
        memcpy(
            static_cast<BYTE *>(buffer.real_ptr) + page_offset,
            page_base,
            copy_size
        );
    }

    // 在绘制后dump整个缓冲区（包含最新写入）
    if (page_count > 0)
    {
        DumpBufferData(buffer.guard_ptr, buffer.size, GenerateDumpFilename());
    }
}

// 替换glDrawArrays
void WINAPI Hooked_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (!Real_glDrawArrays)
    {
        Real_glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYS>(wglGetProcAddress("glDrawArrays"));
    }

    // 处理所有映射缓冲区的写入
    for (auto& [buffer_id, buffer] : g_mappedBuffers)
    {
        if (buffer.is_mapped)
        {
            // 检测写入、同步到GPU并dump数据
            ProcessWritesAndDump(buffer);
        }
    }

    // 调用原始绘制函数
    Real_glDrawArrays(mode, first, count);
}

// 安装钩子
void InstallHooks()
{
    // 获取原始函数地址
    Real_glMapBuffer = reinterpret_cast<PFNGLMAPBUFFER>(wglGetProcAddress("glMapBuffer"));
    Real_glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYS>(wglGetProcAddress("glDrawArrays"));
    Real_glUnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFER>(wglGetProcAddress("glUnmapBuffer"));
    Real_glGetBufferParameteriv = reinterpret_cast<PFNGLGETBUFFERPARAMETERIV>(wglGetProcAddress("glGetBufferParameteriv"));
    Real_glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERV>(wglGetProcAddress("glGetIntegerv"));
}

// 卸载钩子
void UninstallHooks()
{
    // 清理所有映射缓冲区
    for (auto& [buffer_id, buffer] : g_mappedBuffers)
    {
        if (buffer.is_mapped)
        {
            // 同步数据到GPU
            memcpy(buffer.real_ptr, buffer.guard_ptr, buffer.size);

            // 释放内存
            VirtualFree(buffer.guard_ptr, 0, MEM_RELEASE);
        }
    }
    g_mappedBuffers.clear();
}

// 示例使用
int main()
{
    InstallHooks();

    // 模拟OpenGL应用程序
    GLuint vbo1, vbo2;
    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &vbo2);

    // 使用第一个缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);
    void* ptr1 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memset(ptr1, 0xAA, 512);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 使用第二个缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, 512 * 1024, nullptr, GL_DYNAMIC_DRAW);
    void* ptr2 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memset(ptr2, 0xBB, 256);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 解除映射
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // 清理
    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &vbo2);

    UninstallHooks();
    return 0;
}