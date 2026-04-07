#include <windows.h>
#include <string>

#include "Logger.h"

HANDLE Logger::console;


WORD fg_red = 0x0004;
WORD fg_l_red = 0x000C;


WORD fg_green = 0x0002;
WORD fg_l_green = 0x000A;


WORD fg_cyan = 0x000B;
WORD fg_blue = 0x0009;
WORD fg_l_blue = 0x0003;
WORD fg_d_blue = 0x0001;

WORD fg_purple = 0x0005;
WORD fg_l_purple = 0x000D;


WORD fg_yellow = 0x0006;
WORD fg_l_yellow = 0x000E;


WORD fg_white = 0x000F;
WORD fg_l_gray = 0x0007;
WORD fg_gray = 0x0008;

namespace
{
    void write_raw(const char* text)
    {
        if (!Logger::console || !text)
            return;
        DWORD written = 0;
        WriteConsoleA(Logger::console, text, static_cast<DWORD>(lstrlenA(text)), &written, nullptr);
    }

    void write_raw(const std::string& text)
    {
        if (!Logger::console || text.empty())
            return;
        DWORD written = 0;
        WriteConsoleA(Logger::console, text.c_str(), static_cast<DWORD>(text.size()), &written, nullptr);
    }
}

void Logger::log_client_name()
{
    SetConsoleTextAttribute(console, fg_l_purple);
}

void debug(const std::string& msg)
{
    (void)msg;
}

void info(const std::string& msg)
{
    SetConsoleTextAttribute(Logger::console, fg_l_green);
    write_raw(" [INFO] ");
    SetConsoleTextAttribute(Logger::console, fg_white);
    write_raw(msg);
    write_raw("\n");
}

void warn(const std::string& msg)
{
    (void)msg;
}

void err(const std::string& msg)
{
    (void)msg;
}

void fatal(const std::string& msg)
{
    (void)msg;
}

void Logger::log_debug(const std::string& msg) { debug(msg); }
void Logger::log_info(const std::string& msg) { info(msg); }
void Logger::log_warn(const std::string& msg) { warn(msg); }
void Logger::log_err(const std::string& msg) { err(msg); }
void Logger::log_fatal(const std::string& msg) { fatal(msg); }
