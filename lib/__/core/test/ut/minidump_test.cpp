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

#ifdef WIN32

// cspell:ignore STARTUPINFOW MDMP

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

// dbghelp requires the Windows declarations even when its transitive includes provide them.
// NOLINTBEGIN(llvm-include-order, misc-include-cleaner)
#include <windows.h>
#include <dbghelp.h>
// NOLINTEND(llvm-include-order, misc-include-cleaner)
#pragma comment(lib, "dbghelp.lib")

#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

//------------------------------------------------------------------------------

/// Runs this test again in a process that crashes, with a dump directory or, for a null one, without.
namespace
{

/// Sets a variable of this process for a child, and restores it after: the next tests must not see it.
class environment_override final
{
public:

    environment_override(const wchar_t* _name, const wchar_t* _value) :
        m_name(_name)
    {
        std::array<wchar_t, 32768> previous {};
        const DWORD length = ::GetEnvironmentVariableW(_name, previous.data(), static_cast<DWORD>(previous.size()));
        if(length > 0 && length < previous.size())
        {
            m_previous = std::wstring(previous.data(), length);
        }

        // Removing a variable that is not set fails harmlessly.
        ::SetEnvironmentVariableW(_name, _value);
    }

    ~environment_override()
    {
        ::SetEnvironmentVariableW(m_name.c_str(), m_previous ? m_previous->c_str() : nullptr);
    }

    environment_override(const environment_override&)            = delete;
    environment_override(environment_override&&)                 = delete;
    environment_override& operator=(const environment_override&) = delete;
    environment_override& operator=(environment_override&&)      = delete;

private:

    std::wstring m_name;
    std::optional<std::wstring> m_previous;
};

} // namespace

//------------------------------------------------------------------------------

namespace
{

struct child_run
{
    DWORD pid {0};
    DWORD exit_code {0};
};

} // namespace

/// Runs this test again in a process that runs the given probe, with a dump directory or, for a null one, without.
static child_run run_probe_child(const wchar_t* const _directory, const wchar_t* const _probe)
{
    // Only this process and its children see these, until the child ends.
    const environment_override probe(L"SIGHT_MINIDUMP_PROBE", _probe);
    const environment_override directory(L"SIGHT_TEST_DUMP_DIR", _directory);

    std::array<wchar_t, 32768> executable {};
    const DWORD length = ::GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    REQUIRE(length > 0);
    REQUIRE(length < executable.size());

    std::wstring command = L"\"" + std::wstring(executable.data(), length)
                           + L"\" --test-suite=sight::core::test_minidump"
                             L" --test-case=reports_access_violations_in_unit_tests";
    STARTUPINFOW startup {};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process {};
    REQUIRE(
        ::CreateProcessW(
            executable.data(),
            command.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &startup,
            &process
        )
    );
    ::CloseHandle(process.hThread);

    // The timeout is a hang guard, not a timing assertion.
    const DWORD wait_result = ::WaitForSingleObject(process.hProcess, 60000);
    if(wait_result != WAIT_OBJECT_0)
    {
        ::TerminateProcess(process.hProcess, 1);
        ::WaitForSingleObject(process.hProcess, 5000);
    }

    child_run run {.pid      = process.dwProcessId};
    const BOOL got_exit_code = ::GetExitCodeProcess(process.hProcess, &run.exit_code);
    ::CloseHandle(process.hProcess);
    REQUIRE(wait_result == WAIT_OBJECT_0);
    REQUIRE(got_exit_code);
    return run;
}

//------------------------------------------------------------------------------

/// The fault address of each probe's access violation, which tells them apart in the dump.
static constexpr ULONG_PTR s_recovered_address {0x10};
static constexpr ULONG_PTR s_fatal_address {0x20};

//------------------------------------------------------------------------------

/// An access violation a handler recovers, as some libraries do. No object here: __try cannot unwind one.
static void recover_an_access_violation()
{
    __try
    {
        const std::array<ULONG_PTR, 2> arguments {0, s_recovered_address};
        ::RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 2, arguments.data());
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
}

//------------------------------------------------------------------------------

[[noreturn]] static void crash_for_the_probe()
{
    const std::array<ULONG_PTR, 2> arguments {0, s_fatal_address};
    ::RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 2, arguments.data());
    std::abort();
}

//------------------------------------------------------------------------------

/// The fault address of the access violation a dump records.
static ULONG_PTR dumped_fault_address(const std::filesystem::path& _dump)
{
    std::ifstream file(_dump, std::ios::binary);
    REQUIRE(file.is_open());
    std::vector<char> dump {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    // The exception stream, not just the existence of a possibly empty file.
    MINIDUMP_EXCEPTION_STREAM* exception = nullptr;
    ULONG size                           = 0;
    REQUIRE(
        ::MiniDumpReadDumpStream(dump.data(), ExceptionStream, nullptr, reinterpret_cast<void**>(&exception), &size)
    );
    REQUIRE(size >= sizeof(MINIDUMP_EXCEPTION_STREAM));
    REQUIRE(exception->ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION);
    REQUIRE(exception->ExceptionRecord.NumberParameters == 2);
    return static_cast<ULONG_PTR>(exception->ExceptionRecord.ExceptionInformation[1]);
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::core::test_minidump")
{
    TEST_CASE("reports_access_violations_in_unit_tests")
    {
        std::array<wchar_t, 32> probe {};
        if(::GetEnvironmentVariableW(L"SIGHT_MINIDUMP_PROBE", probe.data(), static_cast<DWORD>(probe.size())) != 0)
        {
            const std::wstring_view mode(probe.data());
            if(mode != L"crash")
            {
                recover_an_access_violation();
            }

            if(mode == L"recovered")
            {
                return;
            }

            crash_for_the_probe();
        }

        sight::core::os::temp_dir temporary;
        const auto directory = temporary.path() / L"\u00e9";
        REQUIRE(std::filesystem::create_directory(directory));
        const auto dump_path = [&directory](DWORD _pid)
                               {
                                   return directory / (L"core_ut_" + std::to_wstring(_pid) + L".dmp");
                               };

        // An unset directory leaves normal test processes without a minidump.
        const child_run without = run_probe_child(nullptr, L"crash");
        CHECK(without.exit_code != 0);
        CHECK_FALSE(std::filesystem::exists(dump_path(without.pid)));

        const child_run crashed = run_probe_child(directory.c_str(), L"crash");
        CHECK(crashed.exit_code != 0);
        CHECK(dumped_fault_address(dump_path(crashed.pid)) == s_fatal_address);

        // A recovered access violation never takes the place of the crash that follows.
        const child_run recovered_then_crashed = run_probe_child(directory.c_str(), L"recovered_then_crash");
        CHECK(recovered_then_crashed.exit_code != 0);
        CHECK(dumped_fault_address(dump_path(recovered_then_crashed.pid)) == s_fatal_address);

        // Nor leaves a dump behind a process that ends normally.
        const child_run recovered = run_probe_child(directory.c_str(), L"recovered");
        CHECK(recovered.exit_code == 0);
        CHECK_FALSE(std::filesystem::exists(dump_path(recovered.pid)));
    }
}

#else // ifdef WIN32

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

//------------------------------------------------------------------------------

/// Read at each call: an optimizer that saw a null pointer could drop the write, and the crash with it.
static volatile int* volatile s_null = nullptr;

//------------------------------------------------------------------------------

/// The frame the stack of the crash must show.
[[gnu::noinline]] static void crash_for_the_probe()
{
    *s_null = 1; // NOLINT(clang-analyzer-core.NullDereference): the crash under test
}

//------------------------------------------------------------------------------

namespace
{

struct crashed_child
{
    pid_t pid {0};
    int status {0};
    std::string output;
};

/// Where a crashing child looks for its debugger.
enum class path_place : std::uint8_t
{
    first,
    only
};

} // namespace

/// Runs a case of this test again in a process that crashes, with a dump directory or, for an empty one, without, and
/// with a folder first or alone in its PATH or, for an empty one, its PATH unchanged.
static crashed_child run_crashing_child(
    const std::filesystem::path& _directory,
    const std::filesystem::path& _output,
    std::string_view _test_case            = "reports_fatal_signals_in_unit_tests",
    const std::filesystem::path& _programs = {},
    path_place _place                      = path_place::first
)
{
    // Everything the child needs is built here: nothing runs between its fork and its exec.
    std::vector<std::string> environment {"SIGHT_MINIDUMP_PROBE=1"};
    if(!_directory.empty())
    {
        environment.push_back("SIGHT_TEST_DUMP_DIR=" + _directory.string());
    }

    for(char** variable = environ ; *variable != nullptr ; ++variable)
    {
        const std::string_view entry(*variable);
        if(entry.starts_with("SIGHT_TEST_DUMP_DIR="))
        {
            continue;
        }

        if(entry.starts_with("PATH=") && !_programs.empty())
        {
            environment.push_back(
                "PATH=" + _programs.string()
                + (_place == path_place::first ? ":" + std::string(entry.substr(5)) : std::string())
            );
            continue;
        }

        environment.emplace_back(entry);
    }

    std::vector<char*> envp;
    std::ranges::transform(environment, std::back_inserter(envp), [](std::string& _v){return _v.data();});
    envp.push_back(nullptr);

    std::array<std::string, 3> arguments {
        "/proc/self/exe",
        "--test-suite=sight::core::test_minidump",
        "--test-case=" + std::string(_test_case)
    };
    std::array<char*, 4> argv {arguments[0].data(), arguments[1].data(), arguments[2].data(), nullptr};

    posix_spawn_file_actions_t actions {};
    REQUIRE(::posix_spawn_file_actions_init(&actions) == 0);
    REQUIRE(
        ::posix_spawn_file_actions_addopen(
            &actions,
            STDOUT_FILENO,
            _output.c_str(),
            O_WRONLY | O_CREAT | O_TRUNC,
            0600
        ) == 0
    );
    REQUIRE(::posix_spawn_file_actions_adddup2(&actions, STDOUT_FILENO, STDERR_FILENO) == 0);
    crashed_child child;
    const int spawned = ::posix_spawn(&child.pid, argv[0], &actions, nullptr, argv.data(), envp.data());
    ::posix_spawn_file_actions_destroy(&actions);
    REQUIRE(spawned == 0);

    // The debugger of the crash takes a few seconds; a minute means the child hangs.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
    while(::waitpid(child.pid, &child.status, WNOHANG) == 0)
    {
        if(std::chrono::steady_clock::now() > deadline)
        {
            ::kill(child.pid, SIGKILL);
            ::waitpid(child.pid, &child.status, 0);
            FAIL("the crashing child did not end");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::ifstream file(_output);
    child.output.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return child;
}

//------------------------------------------------------------------------------

/// Writes an executable shell script that stands in for a debugger.
static void write_program(const std::filesystem::path& _folder, std::string_view _name, std::string_view _body)
{
    std::filesystem::create_directories(_folder);
    std::ofstream(_folder / _name) << "#!/bin/sh\n" << _body;
    std::filesystem::permissions(_folder / _name, std::filesystem::perms::owner_all);
}

//------------------------------------------------------------------------------

/// Whether the crash handler can give a stack here: a debugger it uses is in PATH, and Yama lets it attach.
static bool debugger_available()
{
    // Yama's 2 lets only an administrator attach, 3 nobody; without Yama, nothing more than the usual checks.
    std::ifstream scope_file("/proc/sys/kernel/yama/ptrace_scope");
    if(int scope = 0; scope_file >> scope && scope >= 2)
    {
        return false;
    }

    const char* const path = std::getenv("PATH");
    for(const auto entry : std::views::split(std::string_view(path != nullptr ? path : ""), ':'))
    {
        std::error_code error;
        for(const auto& file : std::filesystem::directory_iterator(
                std::filesystem::path(std::string_view(entry.begin(), entry.end())),
                error
        ))
        {
            const std::string name = file.path().filename().string();
#ifdef __clang__
            // gdb cannot read clang's debug information, so the handler does not use it.
            const bool usable = name == "lldb" || name.starts_with("lldb-");
#else
            const bool usable = name == "gdb" || name == "lldb" || name.starts_with("lldb-");
#endif
            if(usable && ::access(file.path().c_str(), X_OK) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::core::test_minidump")
{
    TEST_CASE("reports_fatal_signals_in_unit_tests")
    {
        if(std::getenv("SIGHT_MINIDUMP_PROBE") != nullptr)
        {
            crash_for_the_probe();
            FAIL("the crash probe unexpectedly returned");
        }

        sight::core::os::temp_dir temporary;
        const auto directory = temporary.path() / "dumps";
        REQUIRE(std::filesystem::create_directory(directory));

        // Without a dump directory: the stack of the crash only, and the process still ends on its signal.
        const crashed_child without = run_crashing_child({}, temporary.path() / "without.txt");
        CAPTURE(without.output);
        REQUIRE(WIFSIGNALED(without.status));
        CHECK(WTERMSIG(without.status) == SIGSEGV);
        if(!debugger_available())
        {
            // The process still ends on its signal, without a stack nor a dump.
            MESSAGE("no usable debugger here: only the end of the crashed process is checked");
            return;
        }

        CHECK(without.output.find("crash_for_the_probe") != std::string::npos);
        CHECK(std::filesystem::is_empty(directory));

        // With one: the dump of the process, a minidump from lldb or a core from gdb.
        const crashed_child with = run_crashing_child(directory, temporary.path() / "with.txt");
        CAPTURE(with.output);
        REQUIRE(WIFSIGNALED(with.status));
        CHECK(with.output.find("crash_for_the_probe") != std::string::npos);

        const auto stem = "core_ut_" + std::to_string(with.pid);
        const auto dump = std::filesystem::exists(directory / (stem + ".dmp")) ? directory / (stem + ".dmp")
                                                                               : directory / (stem + ".core");
        std::ifstream file(dump, std::ios::binary);
        REQUIRE(file.is_open());
        std::array<char, 4> magic {};
        REQUIRE(file.read(magic.data(), magic.size()));
        const std::string header(magic.data(), magic.size());
        CHECK((header == "MDMP" || header == "\x7f" "ELF"));
    }

    TEST_CASE("ends_a_crash_whose_debugger_never_ends")
    {
        if(std::getenv("SIGHT_MINIDUMP_PROBE") != nullptr)
        {
            crash_for_the_probe();
            FAIL("the crash probe unexpectedly returned");
        }

        // A debugger that stops the crashed process, as an attached one does, and never ends.
        sight::core::os::temp_dir temporary;
        const auto folder           = temporary.path() / "programs";
        const std::string_view hang = "while [ $# -gt 0 ]; do\n"
                                      "    if [ \"$1\" = -p ]; then kill -STOP \"$2\"; fi\n"
                                      "    shift\n"
                                      "done\n"
                                      "exec /bin/sleep 600\n";
        write_program(folder, "gdb", hang);
        write_program(folder, "lldb", hang);

        const crashed_child child = run_crashing_child(
            {},
            temporary.path() / "output.txt",
            "ends_a_crash_whose_debugger_never_ends",
            folder
        );
        CAPTURE(child.output);
        REQUIRE(WIFSIGNALED(child.status));
        CHECK(WTERMSIG(child.status) == SIGSEGV);
        CHECK(child.output.find("the debugger did not end within 30 s") != std::string::npos);
    }

    TEST_CASE("chooses_lldb_and_falls_back_to_gdb_with_gcc_only")
    {
        if(std::getenv("SIGHT_MINIDUMP_PROBE") != nullptr)
        {
            crash_for_the_probe();
            FAIL("the crash probe unexpectedly returned");
        }

        // Debuggers that only say who they are, alone in the PATH of the crashing child.
        sight::core::os::temp_dir temporary;
        const auto both     = temporary.path() / "both";
        const auto gdb_only = temporary.path() / "gdb_only";
        write_program(both, "lldb", "echo fake lldb ran\n");
        write_program(both, "gdb", "echo fake gdb ran\n");
        write_program(gdb_only, "gdb", "echo fake gdb ran\n");
        const std::string_view test_case = "chooses_lldb_and_falls_back_to_gdb_with_gcc_only";

        // lldb whenever it is there, whatever the compiler.
        const crashed_child with_both =
            run_crashing_child({}, temporary.path() / "both.txt", test_case, both, path_place::only);
        CAPTURE(with_both.output);
        REQUIRE(WIFSIGNALED(with_both.status));
        CHECK(with_both.output.find("fake lldb ran") != std::string::npos);
        CHECK(with_both.output.find("fake gdb ran") == std::string::npos);

        // gdb cannot read clang's debug information: it only stands in for lldb with gcc.
        const crashed_child with_gdb =
            run_crashing_child({}, temporary.path() / "gdb.txt", test_case, gdb_only, path_place::only);
        CAPTURE(with_gdb.output);
        REQUIRE(WIFSIGNALED(with_gdb.status));
#ifdef __clang__
        CHECK(with_gdb.output.find("fake gdb ran") == std::string::npos);
        CHECK(with_gdb.output.find("no lldb in PATH") != std::string::npos);
#else
        CHECK(with_gdb.output.find("fake gdb ran") != std::string::npos);
#endif
    }
}

#endif // ifdef WIN32
