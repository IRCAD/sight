/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

// cspell:ignore STARTF STARTUPINFOW

#if defined(WIN32) && defined(_DEBUG)

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <windows.h>
#include <crtdbg.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

//------------------------------------------------------------------------------

/// What a child running the given probe left behind.
struct probe_result
{
    bool ended {false};
    DWORD exit_code {0};
    std::string output;
};

//------------------------------------------------------------------------------

/// Runs this test again in a process that makes a CRT report, its error output in a file.
static probe_result run_probe(const wchar_t* const _probe)
{
    REQUIRE(::SetEnvironmentVariableW(L"SIGHT_CRT_PROBE", _probe));

    const sight::core::os::temp_dir temporary;
    const auto output_path = temporary.path() / "stderr.txt";

    SECURITY_ATTRIBUTES inherited {.nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE};
    HANDLE const output                     = ::CreateFileW(
        output_path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        &inherited,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    REQUIRE(output != INVALID_HANDLE_VALUE);

    std::array<wchar_t, 32768> executable {};
    const DWORD length = ::GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    REQUIRE(length > 0);
    REQUIRE(length < executable.size());

    std::wstring command = L"\"" + std::wstring(executable.data(), length)
                           + L"\" --test-suite=sight::core::crt_report --test-case=never_waits_for_a_click";
    STARTUPINFOW startup {};
    startup.cb         = sizeof(startup);
    startup.dwFlags    = STARTF_USESTDHANDLES;
    startup.hStdOutput = output;
    startup.hStdError  = output;
    PROCESS_INFORMATION process {};
    const BOOL created =
        ::CreateProcessW(
            executable.data(),
            command.data(),
            nullptr,
            nullptr,
            TRUE,
            0,
            nullptr,
            nullptr,
            &startup,
            &process
        );
    ::CloseHandle(output);
    ::SetEnvironmentVariableW(L"SIGHT_CRT_PROBE", nullptr);
    REQUIRE(created);
    ::CloseHandle(process.hThread);

    // A hang guard, not a timing assertion: a box would stay until somebody clicks.
    probe_result result;
    result.ended = ::WaitForSingleObject(process.hProcess, 120000) == WAIT_OBJECT_0;
    if(!result.ended)
    {
        ::TerminateProcess(process.hProcess, 1);
        ::WaitForSingleObject(process.hProcess, 5000);
    }

    ::GetExitCodeProcess(process.hProcess, &result.exit_code);
    ::CloseHandle(process.hProcess);

    std::ifstream file(output_path, std::ios::binary);
    result.output.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return result;
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::core::crt_report")
{
    TEST_CASE("never_waits_for_a_click")
    {
        // The child: the same calls as a debug assertion of the CRT and as a qFatal of Qt.
        std::array<wchar_t, 16> probe {};
        if(::GetEnvironmentVariableW(L"SIGHT_CRT_PROBE", probe.data(), static_cast<DWORD>(probe.size())) != 0)
        {
            if(std::wstring(probe.data()) == L"warning")
            {
                _CrtDbgReportW(_CRT_WARN, nullptr, 0, nullptr, L"%ls", L"probe warning");
                return;
            }

            _CrtDbgReportW(
                std::wstring(probe.data()) == L"assert" ? _CRT_ASSERT : _CRT_ERROR,
                nullptr,
                0,
                nullptr,
                L"%ls",
                L"probe report"
            );
            FAIL("the test went on after the report");
        }

        SUBCASE("an assertion fails the test with its message")
        {
            const auto result = run_probe(L"assert");
            REQUIRE(result.ended);
            CHECK(result.exit_code != 0);
            CHECK(result.output.find("probe report") != std::string::npos);
            CHECK(result.output.find("the test went on") == std::string::npos);
        }

        SUBCASE("a fatal error fails the test with its message")
        {
            const auto result = run_probe(L"error");
            REQUIRE(result.ended);
            CHECK(result.exit_code != 0);
            CHECK(result.output.find("probe report") != std::string::npos);
            CHECK(result.output.find("the test went on") == std::string::npos);
        }

        SUBCASE("a warning is reported and the test goes on")
        {
            const auto result = run_probe(L"warning");
            REQUIRE(result.ended);
            CHECK(result.exit_code == 0);
            CHECK(result.output.find("probe warning") != std::string::npos);
        }
    }
}

#endif // if defined(WIN32) && defined(_DEBUG)
