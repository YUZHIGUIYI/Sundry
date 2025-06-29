// #include <disable_copy.h>
// #include <cstdint>
// #include <unordered_map>
// #include <algorithm>
// #include <string>
// #include <filesystem>
// #include <iostream>
// #include <shlobj_core.h>
//
// struct Vec3
// {
//     float x, y, z;
//     Vec3() = default;
//     Vec3(float value = 0.0f) : x(value), y(value), z(value) {}
// };
//
// struct Object
// {
//     float scale;
//     Vec3 position, size;
//     Object(float value = 1.0f) : position(0.0f), size(value) {}
// };
//
// struct Scene : disable_copy
// {
//     uint32_t m_Id  = 0;
//
//     std::unordered_map<uint32_t, Object> elements;
//
//     Scene() = default;
//     ~Scene() = default;
//
//     bool insert(const Object& object)
//     {
//         auto [it, isInsert] = elements.try_emplace(m_Id++, object);
//         return isInsert;
//     }
// };
//
// int main()
// {
//     // Scene scene1;
//     // No allowed since base class 'disable_copy' has a deleted copy assignment operator and
//     // a deleted copy constructor
//     // Scene scene2 = scene1;
//     // Scene scene3 = std::move(scene1);
//     // Scene scene4(scene1);
//     // Scene scene5;
//     // scene5 = scene1;
//     // scene5 = std::move(scene1);
//     // scene1.insert(Object{ 1.0f });
//
//     std::string redirect_library_path{};
//     std::string system_library_path = "C:\\Windows\\System32\\d3d12.dll";
//     auto roaming_path = std::getenv("APPDATA");
//     if (roaming_path != nullptr)
//     {
//         std::string dll_user_target = std::string{ roaming_path } + "\\" + std::string{ "gfxshim" };
//         std::string dll_runtime_target = dll_user_target + "\\" + std::string{ "dx-runtimes" };
//         if (!std::filesystem::exists(dll_user_target))
//         {
//             std::filesystem::create_directory(dll_user_target);
//         }
//         if (!std::filesystem::exists(dll_runtime_target))
//         {
//             std::filesystem::create_directory(dll_runtime_target);
//         }
//         std::string rename_library_path = std::string{ system_library_path };
//         auto dot_pos = rename_library_path.find_last_of('.');
//         auto back_slash_pos = rename_library_path.find_last_of('\\');
//         if (dot_pos == std::string::npos || back_slash_pos == std::string::npos)
//         {
//             return false;
//         }
//         auto system_library_directory = rename_library_path.substr(0, back_slash_pos);
//         rename_library_path = dll_runtime_target + "\\"  + rename_library_path.substr(back_slash_pos + 1, dot_pos - back_slash_pos - 1) + "_ms.dll";
//         if (!std::filesystem::exists(rename_library_path))
//         {
//             if (!std::filesystem::copy_file(system_library_path, rename_library_path))
//             {
//                 return false;
//             }
//         }
//         auto redirect_core_library_path = dll_runtime_target + "\\D3D12Core.dll";
//         auto system_core_library_path = system_library_directory + "\\D3D12Core.dll";
//         if (!std::filesystem::exists(redirect_core_library_path))
//         {
//             if (!std::filesystem::copy_file(system_core_library_path, redirect_core_library_path))
//             {
//                 return false;
//             }
//         }
//         redirect_library_path = rename_library_path;
//     }
//
//     auto app_data = std::getenv("APPDATA");
//     std::cout << app_data << '\n';
// }

#include <windows.h>
#include <wincodec.h>         // WIC
#include <d2d1.h>             // Direct2D
#include <d2d1helper.h>       // 包含一些 D2D 辅助函数模版，比如 D2D1CreateFactory<T>()
#include <wrl.h>              // ComPtr (需要 VS2013 及以上)
#include <cstdio>             // printf、wprintf 等

// 链接库
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "WindowsCodecs.lib")

using Microsoft::WRL::ComPtr;

// 封装一个函数，用于把绘制的图形保存到 BMP 文件
HRESULT SaveTriangleToBmp(const wchar_t* outputFilename, UINT width, UINT height)
{
    // 初始化 COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        wprintf(L"CoInitializeEx failed. hr=0x%X\n", hr);
        return hr;
    }

    // 1) 创建 WIC 工厂
    ComPtr<IWICImagingFactory> wicFactory;
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );
    if (FAILED(hr))
    {
        wprintf(L"CoCreateInstance for WIC factory failed. hr=0x%X\n", hr);
        CoUninitialize();
        return hr;
    }

    // 2) 创建一个基于内存的 WIC 位图，像素格式必须与 D2D 兼容
    ComPtr<IWICBitmap> wicBitmap;
    hr = wicFactory->CreateBitmap(
        width,
        height,
        GUID_WICPixelFormat32bppPBGRA,  // 与 D2D 兼容的常用格式
        WICBitmapCacheOnLoad,
        &wicBitmap
    );
    if (FAILED(hr))
    {
        wprintf(L"CreateBitmap failed. hr=0x%X\n", hr);
        CoUninitialize();
        return hr;
    }

    // 3) 创建 D2D 工厂
    ComPtr<ID2D1Factory> d2dFactory;
    {
        D2D1_FACTORY_OPTIONS factoryOptions = {};
        // 这里使用了 <d2d1helper.h> 中的模版函数 D2D1CreateFactory<T>
        hr = D2D1CreateFactory<ID2D1Factory>(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &d2dFactory
        );
        if (FAILED(hr))
        {
            wprintf(L"D2D1CreateFactory failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }
    }

    // 4) 从 wicBitmap 创建一个 D2D 渲染目标
    ComPtr<ID2D1RenderTarget> renderTarget;
    {
        // 指定 DXGI 格式和 Alpha 模式。其中 DXGI_FORMAT_B8G8R8A8_UNORM
        // 对应 32bpp BGRA；Alpha 模式 pre-multiplied
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f,  // DPI X
            96.0f   // DPI Y
        );

        hr = d2dFactory->CreateWicBitmapRenderTarget(
            wicBitmap.Get(),
            rtProps,
            &renderTarget
        );
        if (FAILED(hr))
        {
            wprintf(L"CreateWicBitmapRenderTarget failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }
    }

    // 5) 使用 D2D 绘制三角形
    renderTarget->BeginDraw();

    // 先填充白色背景
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        hr = renderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            &brush
        );
        if (SUCCEEDED(hr))
        {
            auto sz = renderTarget->GetSize();
            renderTarget->FillRectangle(D2D1::RectF(0, 0, sz.width, sz.height), brush.Get());
        }
    }

    // 再绘制蓝色三角形
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        hr = renderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Blue),
            &brush
        );
        if (SUCCEEDED(hr))
        {
            // 三个定点
            D2D1_POINT_2F p1 = D2D1::Point2F(width / 2.0f, 0.0f);
            D2D1_POINT_2F p2 = D2D1::Point2F(0.0f, (FLOAT)height);
            D2D1_POINT_2F p3 = D2D1::Point2F((FLOAT)width, (FLOAT)height);

            // 画三条边
            renderTarget->DrawLine(p1, p2, brush.Get(), 3.0f);
            renderTarget->DrawLine(p2, p3, brush.Get(), 3.0f);
            renderTarget->DrawLine(p3, p1, brush.Get(), 3.0f);

            // 如果想填充整个三角形，可用几何对象：
            // ComPtr<ID2D1PathGeometry> path;
            // if (SUCCEEDED(d2dFactory->CreatePathGeometry(&path)))
            // {
            //     ComPtr<ID2D1GeometrySink> sink;
            //     if (SUCCEEDED(path->Open(&sink)))
            //     {
            //         sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
            //         sink->AddLine(p2);
            //         sink->AddLine(p3);
            //         sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            //         sink->Close();
            //         // 填充
            //         renderTarget->FillGeometry(path.Get(), brush.Get());
            //         // 加上描边（可选）
            //         renderTarget->DrawGeometry(path.Get(), brush.Get(), 3.0f);
            //     }
            // }
        }
    }

    hr = renderTarget->EndDraw();
    if (FAILED(hr))
    {
        wprintf(L"EndDraw failed. hr=0x%X\n", hr);
        CoUninitialize();
        return hr;
    }

    // 6) 使用 WIC 把图像保存为 BMP
    {
        // 创建写文件用的 WIC 流
        ComPtr<IWICStream> wicStream;
        hr = wicFactory->CreateStream(&wicStream);
        if (FAILED(hr))
        {
            wprintf(L"CreateStream failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        // 用指定文件名初始化流
        hr = wicStream->InitializeFromFilename(outputFilename, GENERIC_WRITE);
        if (FAILED(hr))
        {
            wprintf(L"InitializeFromFilename failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        // 创建 BMP 编码器
        ComPtr<IWICBitmapEncoder> bmpEncoder;
        hr = wicFactory->CreateEncoder(
            GUID_ContainerFormatBmp,
            nullptr,
            &bmpEncoder
        );
        if (FAILED(hr))
        {
            wprintf(L"CreateEncoder(BMP) failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        hr = bmpEncoder->Initialize(wicStream.Get(), WICBitmapEncoderNoCache);
        if (FAILED(hr))
        {
            wprintf(L"bmpEncoder->Initialize failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        // 创建帧
        ComPtr<IWICBitmapFrameEncode> frameEncode;
        ComPtr<IPropertyBag2> frameProps;
        hr = bmpEncoder->CreateNewFrame(&frameEncode, &frameProps);
        if (FAILED(hr))
        {
            wprintf(L"CreateNewFrame failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        hr = frameEncode->Initialize(frameProps.Get());
        if (FAILED(hr))
        {
            wprintf(L"frameEncode->Initialize failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        // 设置帧大小、像素格式
        hr = frameEncode->SetSize(width, height);
        if (FAILED(hr))
        {
            wprintf(L"SetSize failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat32bppPBGRA;
        hr = frameEncode->SetPixelFormat(&pixelFormat);
        if (FAILED(hr))
        {
            wprintf(L"SetPixelFormat failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        // 把 wicBitmap 里的像素数据写到 BMP
        hr = frameEncode->WriteSource(wicBitmap.Get(), nullptr);
        if (FAILED(hr))
        {
            wprintf(L"WriteSource failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        hr = frameEncode->Commit();
        if (FAILED(hr))
        {
            wprintf(L"frameEncode->Commit failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }

        hr = bmpEncoder->Commit();
        if (FAILED(hr))
        {
            wprintf(L"bmpEncoder->Commit failed. hr=0x%X\n", hr);
            CoUninitialize();
            return hr;
        }
    }

    // 收尾
    CoUninitialize();
    return S_OK;
}

int wmain()
{
    // 测试：将 400×300 大小的三角形绘制并保存为 triangle_test.bmp
    HRESULT hr = SaveTriangleToBmp(L"triangle_test.bmp", 400, 300);
    if (FAILED(hr))
    {
        wprintf(L"Failed! hr=0x%X\n", hr);
        return -1;
    }
    wprintf(L"Success! Saved triangle to triangle_test.bmp\n");
    return 0;
}