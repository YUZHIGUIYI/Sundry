//
// Created by ZZK on 2023/9/1.
//

#pragma once

#include <iostream>
#include <source_location>
#include <format>
#include <type_traits>
#include <fstream>
#include <chrono>

#if defined(_WIN32)
#include <windows.h>
#undef max
#undef min
#undef near
#undef far
#endif

// A single-header-only logging library for C++20, support MSVC
// Clone from archibate, see https://github.com/archibate/minilog
// 内部再来一个detail命名空间，隐藏内部细节，因为用户往往不关心
// 仅公布API
namespace minilog
{
    // 可大量使用模板和宏的配合
    // 一种便于长期维护的枚举类型和枚举相关函数做法 - x macro
    // C++规定，“一个或两个下划线 + 名称” 为保留字
#define MINILOG_FOREACH_LOG_LEVEL(f) \
        f(trace) \
        f(debug) \
        f(info) \
        f(critical) \
        f(warn) \
        f(error) \
        f(fatal)

    enum class log_level : std::uint8_t
    {
#define _FUNCTION(name) name,
        MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    };

    // 隐藏的内部细节
    namespace detail
    {
#if defined(__linux__) || defined(__APPLE__)
        inline constexpr char k_level_ansi_colors[static_cast<std::uint8_t>(log_level::fatal) + 1][8] = {
            "\E[37m",
            "\E[35m",
            "\E[32m",
            "\E[34m",
            "\E[33m",
            "\E[31m",
            "\E[31;1m",
        };
        inline constexpr char k_reset_ansi_color[4] = "\E[m";
        #define _MINILOG_IF_HAS_ANSI_COLORS(x) x
#else
        inline constexpr short k_level_ansi_colors[static_cast<std::uint8_t>(log_level::fatal) + 1] = {
                0x0002 | 0x0004 | 0x0008,   // trace
                0x0001 | 0x0008,            // debug
                0x0002 | 0x0008,            // info
                0x0001 | 0x0002 | 0x0008,   // critical
                0x0002 | 0x0010,            // warn
                0x0001 | 0x0020 | 0x0080,   // error
                0x0004 | 0x0008 | 0x0080,   // fatal
        };
#endif

        inline std::string log_level_name(log_level level)
        {
            switch (level)
            {
#define _FUNCTION(name) case log_level::name: return #name;
                MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
            }
        }

        inline log_level log_level_from_name(std::string lev)
        {
#define _FUNCTION(name) if(lev == #name) return log_level::name;
            MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef __FUNCTION
            return log_level::info;
        }

        template <class T>
        struct with_source_location
        {
        private:
            T inner;
            std::source_location loc;

        public:
            // consteval只能在编译期调用
            // constexpr可以在编译期或运行期调用
            // 注意，C++默认参数的计算位置，不是在被调用函数上，而是在调用者处赋值计算的。
            template <class U> requires std::constructible_from<T, U>
            consteval  with_source_location(U &&inner, std::source_location loc = std::source_location::current())
                    : inner(std::forward<U>(inner)), loc(std::move(loc)) {}

            constexpr T const &format() const { return inner; }

            constexpr std::source_location const &location() const { return loc; }
        };

        // 对于static变量，建议加上inline，这样每个引用该头文件的cpp文件都共享（链接时）；但如果没有inline，则每个引用该头文件的cpp文件都会有一个，不共享
        // 程序开始执行时从环境变量中检测是否设置了该变量
        inline log_level g_max_level = [] () -> log_level
        {
            char* value = nullptr;
            size_t len = 0;
            auto err = _dupenv_s(&value, &len, "MINILOG_LEVEL");
            if (err)
            {
                return detail::log_level_from_name(value);
            }
            return log_level::info;
        } ();

        inline std::ofstream g_log_file = [] () -> std::ofstream
        {
            char* path = nullptr;
            size_t len = 0;
            auto err = _dupenv_s(&path, &len, "MINILOG_FILE");
            if (err)
            {
                return std::ofstream(path, std::ios::app);
            }
            return std::ofstream{};
        } ();

        inline void output_log(log_level level, std::string msg, std::source_location const &loc)
        {
            // 时间戳 - C++20
            std::chrono::zoned_time now{ std::chrono::current_zone(), std::chrono::system_clock::now() };
            // 单独一个 std::cout << 是线程安全的，所以建议组装成std::string一次性打印
            msg = std::format("{} {}:{} [{}] {}", now, loc.file_name(), loc.line(), detail::log_level_name(level), msg);
            if (g_log_file)
            {
                g_log_file << msg + '\n';
            }
            if (level >= g_max_level)
            {
#if defined(__linux__) || defined(__APPLE__)
                std::cout << k_level_ansi_colors[static_cast<size_t>(level)] +
                                msg + k_reset_ansi_color + '\n';
#else
                const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(handle, k_level_ansi_colors[static_cast<size_t>(level)]);
                std::cout << msg + '\n';
#endif
            }
        }
    }

    // 设置log level
    inline void set_log_level(log_level level)
    {
        detail::g_max_level = level;
    }

    // 设置log file路径
    inline void set_log_file(std::string path)
    {
        detail::g_log_file = std::ofstream(path, std::ios::app);
    }

    template <typename ... Args>
    void generic_log(log_level level, detail::with_source_location<std::format_string<Args...>> fmt, Args &&...args)
    {

        auto const &loc = fmt.location();
        // std::vformat中两个参数可以参考std::format的实现
        auto msg = std::vformat(fmt.format().get(), std::make_format_args(args...));
        detail::output_log(level, std::move(msg), loc);
    }

#define _FUNCTION(name) \
    template <typename ... Args> \
    void log_##name(detail::with_source_location<std::format_string<Args...>> fmt, Args &&... args) \
    { \
        return generic_log(log_level::name, std::move(fmt), std::forward<Args>(args)...); \
    }

    MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION

#define MINILOG_P(x) ::minilog::log_debug(#x "={}", x)
}
