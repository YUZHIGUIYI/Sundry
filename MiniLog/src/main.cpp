//
// Created by ZZK on 2023/9/1.
//

#include "minilog.h"

int main()
{
    minilog::set_log_level(minilog::log_level::trace);
    minilog::set_log_file("./mini.log");
#define _FUNCTION(name) minilog::log_##name(#name);
    MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
}