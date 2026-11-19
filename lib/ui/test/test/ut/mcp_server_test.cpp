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

// cspell:ignore STARTUPINFOW STARTF

#include "gui_test_support.hpp"

#include <ui/test/mcp_server.hpp>
#include <ui/test/scenario.hpp>

#include <doctest/doctest.h>

#include <nlohmann/json.hpp>

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#ifdef WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <poll.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using json = nlohmann::json;

namespace sight::ui::test::detail
{

namespace
{

/// A window of three named widgets, for the calls of the tool.
struct form
{
    form()
    {
        toggle.setObjectName("toggle");
        toggle.setCheckable(true);
        edit.setObjectName("edit");
        label.setObjectName("label");
        layout.addWidget(&toggle);
        layout.addWidget(&edit);
        layout.addWidget(&label);
        window.show();
    }

    QWidget window;
    QVBoxLayout layout {&window};
    QPushButton toggle {"toggle", &window};
    QLineEdit edit {&window};
    QLabel label {"ready", &window};
};

/// How the probe ended, and what it printed on its standard output.
struct child_run
{
    int exit_code {-1};
    std::string output;
};

} // namespace

//------------------------------------------------------------------------------

/// A request of the client, as one line.
static std::string request(int _id, std::string_view _method, const json& _params = json::object())
{
    return json {{"jsonrpc", "2.0"}, {"id", _id}, {"method", _method}, {"params", _params}}.dump();
}

//------------------------------------------------------------------------------

/// The response of the server to a request, parsed.
static json answer(mcp_server& _server, const std::string& _request)
{
    const auto response = _server.answer(_request);
    REQUIRE(response.has_value());
    return json::parse(*response);
}

//------------------------------------------------------------------------------

/// One call of the tool: whether it failed, and its text.
static std::pair<bool, std::string> call(mcp_server& _server, const json& _arguments)
{
    const json response = answer(_server, request(7, "tools/call", {{"name", "ui"}, {"arguments", _arguments}}));
    const json& result  = response.at("result");
    return {result.at("isError").get<bool>(), result.at("content").at(0).at("text").get<std::string>()};
}

//------------------------------------------------------------------------------

#ifdef WIN32
/// Runs the probe with the requests on its standard input, until it ends or the hang guard of the test ends it.
static child_run serve_in_a_child(const std::string& _requests)
{
    // Only the ends of the child are inherited, so that the pipes close with it.
    SECURITY_ATTRIBUTES inherited {.nLength = sizeof(SECURITY_ATTRIBUTES), .lpSecurityDescriptor = nullptr,
                                   .bInheritHandle = TRUE
    };
    HANDLE input_read   = nullptr;
    HANDLE input_write  = nullptr;
    HANDLE output_read  = nullptr;
    HANDLE output_write = nullptr;
    HANDLE error        = nullptr;
    REQUIRE(::CreatePipe(&input_read, &input_write, &inherited, 0));
    REQUIRE(::CreatePipe(&output_read, &output_write, &inherited, 0));
    REQUIRE(::SetHandleInformation(input_write, HANDLE_FLAG_INHERIT, 0));
    REQUIRE(::SetHandleInformation(output_read, HANDLE_FLAG_INHERIT, 0));
    REQUIRE(
        ::DuplicateHandle(
            ::GetCurrentProcess(),
            ::GetStdHandle(STD_ERROR_HANDLE),
            ::GetCurrentProcess(),
            &error,
            0,
            TRUE,
            DUPLICATE_SAME_ACCESS
        )
    );

    std::array<wchar_t, 32768> executable {};
    const DWORD length = ::GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    REQUIRE(length > 0);
    REQUIRE(length < executable.size());

    // Without the version line of doctest, the first thing it prints.
    std::wstring command = L"\"" + std::wstring(executable.data(), length)
                           + L"\" --test-suite=sight::ui::test::mcp_server"
                             L" --test-case=serves_its_standard_input_until_the_client_closes_it --no-intro=true";
    STARTUPINFOW startup {};
    startup.cb         = sizeof(startup);
    startup.dwFlags    = STARTF_USESTDHANDLES;
    startup.hStdInput  = input_read;
    startup.hStdOutput = output_write;
    startup.hStdError  = error;
    PROCESS_INFORMATION process {};

    // Unset here, or this process would be the probe: the next tests must not see it either.
    ::SetEnvironmentVariableW(L"SIGHT_MCP_PROBE", L"1");
    const BOOL created = ::CreateProcessW(
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
    ::SetEnvironmentVariableW(L"SIGHT_MCP_PROBE", nullptr);
    ::CloseHandle(input_read);
    ::CloseHandle(output_write);
    ::CloseHandle(error);
    REQUIRE(created);
    ::CloseHandle(process.hThread);

    std::thread writer(
        [&]
            {
        for(std::string_view rest = _requests ; !rest.empty() ; )
        {
            DWORD written = 0;
            if(::WriteFile(input_write, rest.data(), static_cast<DWORD>(rest.size()), &written, nullptr) == 0
               || written == 0)
            {
                break;
            }

            rest.remove_prefix(written);
        }

        ::CloseHandle(input_write);
            });

    child_run run;
    std::thread reader(
        [&]
            {
        std::array<char, 4096> buffer {};
        DWORD count = 0;
        while(::ReadFile(output_read, buffer.data(), static_cast<DWORD>(buffer.size()), &count, nullptr) != 0
              && count > 0)
        {
            run.output.append(buffer.data(), count);
        }
            });

    // A hang guard, not a timing assertion: an ended child breaks both pipes, which ends both threads.
    const DWORD waited = ::WaitForSingleObject(process.hProcess, 60000);
    if(waited != WAIT_OBJECT_0)
    {
        ::TerminateProcess(process.hProcess, 1);
        ::WaitForSingleObject(process.hProcess, INFINITE);
    }

    DWORD exit_code = 0;
    ::GetExitCodeProcess(process.hProcess, &exit_code);
    ::CloseHandle(process.hProcess);
    writer.join();
    reader.join();
    ::CloseHandle(output_read);
    REQUIRE_MESSAGE(waited == WAIT_OBJECT_0, "the server did not end when its client closed its input");
    run.exit_code = static_cast<int>(exit_code);
    return run;
}

//------------------------------------------------------------------------------

/// Runs the probe of a test case reading a console of its own, where nobody types, until it ends or the hang guard of
/// the test ends it. Returns its exit code.
static int run_on_a_console(const std::wstring& _test_case)
{
    std::array<wchar_t, 32768> executable {};
    const DWORD length = ::GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    REQUIRE(length > 0);
    REQUIRE(length < executable.size());

    std::wstring command = L"\"" + std::wstring(executable.data(), length)
                           + L"\" --test-suite=sight::ui::test::mcp_server --test-case=" + _test_case
                           + L" --no-intro=true";

    // A hidden console: the test never takes the foreground.
    STARTUPINFOW startup {};
    startup.cb          = sizeof(startup);
    startup.dwFlags     = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION process {};
    ::SetEnvironmentVariableW(L"SIGHT_MCP_PROBE", L"1");
    const BOOL created = ::CreateProcessW(
        executable.data(),
        command.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_NEW_CONSOLE,
        nullptr,
        nullptr,
        &startup,
        &process
    );
    ::SetEnvironmentVariableW(L"SIGHT_MCP_PROBE", nullptr);
    REQUIRE(created);
    ::CloseHandle(process.hThread);

    // A hang guard, not a timing assertion.
    const DWORD waited = ::WaitForSingleObject(process.hProcess, 60000);
    if(waited != WAIT_OBJECT_0)
    {
        ::TerminateProcess(process.hProcess, 1);
        ::WaitForSingleObject(process.hProcess, INFINITE);
    }

    DWORD exit_code = 0;
    ::GetExitCodeProcess(process.hProcess, &exit_code);
    ::CloseHandle(process.hProcess);
    REQUIRE_MESSAGE(waited == WAIT_OBJECT_0, "the server did not end with its application");
    return static_cast<int>(exit_code);
}
#else // ifdef WIN32
/// Runs the probe with the requests on its standard input, until it ends or the hang guard of the test ends it.
static child_run serve_in_a_child(const std::string& _requests)
{
    std::array<int, 2> input {};
    std::array<int, 2> output {};
    REQUIRE(::pipe2(input.data(), O_CLOEXEC) == 0);
    REQUIRE(::pipe2(output.data(), O_CLOEXEC) == 0);

    // Everything the child needs is built here: nothing runs between its fork and its exec.
    std::vector<std::string> environment {"SIGHT_MCP_PROBE=1"};
    for(char** variable = environ ; *variable != nullptr ; ++variable)
    {
        environment.emplace_back(*variable);
    }

    std::vector<char*> envp;
    std::ranges::transform(environment, std::back_inserter(envp), [](std::string& _v){return _v.data();});
    envp.push_back(nullptr);

    // Without the version line of doctest, the first thing it prints.
    std::array<std::string, 4> arguments {
        "/proc/self/exe",
        "--test-suite=sight::ui::test::mcp_server",
        "--test-case=serves_its_standard_input_until_the_client_closes_it",
        "--no-intro=true"
    };
    std::array<char*, 5> argv {
        arguments[0].data(), arguments[1].data(), arguments[2].data(), arguments[3].data(), nullptr
    };

    posix_spawn_file_actions_t actions {};
    REQUIRE(::posix_spawn_file_actions_init(&actions) == 0);
    REQUIRE(::posix_spawn_file_actions_adddup2(&actions, input[0], STDIN_FILENO) == 0);
    REQUIRE(::posix_spawn_file_actions_adddup2(&actions, output[1], STDOUT_FILENO) == 0);
    pid_t child       = 0;
    const int spawned = ::posix_spawn(&child, argv[0], &actions, nullptr, argv.data(), envp.data());
    ::posix_spawn_file_actions_destroy(&actions);
    ::close(input[0]);
    ::close(output[1]);
    REQUIRE(spawned == 0);

    std::thread writer(
        [&]
            {
        for(std::string_view rest = _requests ; !rest.empty() ; )
        {
            const auto written = ::write(input[1], rest.data(), rest.size());
            if(written <= 0)
            {
                break;
            }

            rest.remove_prefix(static_cast<std::size_t>(written));
        }

        ::close(input[1]);
            });

    // Read until the child closes its output, or the hang guard.
    child_run run;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
    std::array<char, 4096> buffer {};
    for(pollfd ready {.fd = output[0], .events = POLLIN, .revents = 0} ;
        std::chrono::steady_clock::now() < deadline ; )
    {
        if(::poll(&ready, 1, 100) <= 0)
        {
            continue;
        }

        const auto count = ::read(output[0], buffer.data(), buffer.size());
        if(count <= 0)
        {
            break;
        }

        run.output.append(buffer.data(), static_cast<std::size_t>(count));
    }

    int status   = 0;
    pid_t reaped = 0;
    while((reaped = ::waitpid(child, &status, WNOHANG)) == 0 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    const bool ended = reaped == child;
    if(!ended)
    {
        ::kill(child, SIGKILL);
        ::waitpid(child, &status, 0);
    }

    writer.join();
    ::close(output[0]);
    REQUIRE_MESSAGE(ended, "the server did not end when its client closed its input");
    run.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
    return run;
}

//------------------------------------------------------------------------------

/// Runs the probe of a test case reading a terminal of its own, where nobody types, until it ends or the hang guard of
/// the test ends it. Returns its exit code.
static int run_on_a_console(const std::string& _test_case)
{
    const int master = ::posix_openpt(O_RDWR | O_NOCTTY | O_CLOEXEC);
    REQUIRE(master >= 0);
    REQUIRE(::grantpt(master) == 0);
    REQUIRE(::unlockpt(master) == 0);
    std::array<char, 128> terminal {};
    REQUIRE(::ptsname_r(master, terminal.data(), terminal.size()) == 0);

    // Everything the child needs is built here: nothing runs between its fork and its exec.
    std::vector<std::string> environment {"SIGHT_MCP_PROBE=1"};
    for(char** variable = environ ; *variable != nullptr ; ++variable)
    {
        environment.emplace_back(*variable);
    }

    std::vector<char*> envp;
    std::ranges::transform(environment, std::back_inserter(envp), [](std::string& _v){return _v.data();});
    envp.push_back(nullptr);

    std::array<std::string, 4> arguments {
        "/proc/self/exe",
        "--test-suite=sight::ui::test::mcp_server",
        "--test-case=" + _test_case,
        "--no-intro=true"
    };
    std::array<char*, 5> argv {
        arguments[0].data(), arguments[1].data(), arguments[2].data(), arguments[3].data(), nullptr
    };

    posix_spawn_file_actions_t actions {};
    REQUIRE(::posix_spawn_file_actions_init(&actions) == 0);
    REQUIRE(::posix_spawn_file_actions_addopen(&actions, STDIN_FILENO, terminal.data(), O_RDWR | O_NOCTTY, 0) == 0);
    pid_t child       = 0;
    const int spawned = ::posix_spawn(&child, argv[0], &actions, nullptr, argv.data(), envp.data());
    ::posix_spawn_file_actions_destroy(&actions);
    REQUIRE(spawned == 0);

    // A hang guard, not a timing assertion.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
    int status          = 0;
    pid_t reaped        = 0;
    while((reaped = ::waitpid(child, &status, WNOHANG)) == 0 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    const bool ended = reaped == child;
    if(!ended)
    {
        ::kill(child, SIGKILL);
        ::waitpid(child, &status, 0);
    }

    ::close(master);
    REQUIRE_MESSAGE(ended, "the server did not end with its application");
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
}
#endif // ifdef WIN32

TEST_SUITE("sight::ui::test::mcp_server")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "speaks_the_protocol_of_a_client")
    {
        scenario ui(application);
        mcp_server server(ui);

        // The version of the client when it is known, else the latest of the server.
        CHECK(
            answer(server, request(1, "initialize", {{"protocolVersion", "2025-06-18"}}))["result"]["protocolVersion"]
            == "2025-06-18"
        );
        CHECK(
            answer(server, request(1, "initialize", {{"protocolVersion", "1999-01-01"}}))["result"]["protocolVersion"]
            == "2025-11-25"
        );

        const json tools = answer(server, request(2, "tools/list"))["result"]["tools"];
        REQUIRE(tools.size() == 1);
        CHECK(tools[0]["name"] == "ui");

        CHECK(answer(server, request(3, "ping"))["result"] == json::object());

        // A notification gets no response; a request that is not one gets its JSON-RPC error.
        CHECK_FALSE(server.answer(R"({"jsonrpc": "2.0", "method": "notifications/initialized"})").has_value());
        CHECK(answer(server, "{not json")["error"]["code"] == -32700);
        CHECK(answer(server, R"({"jsonrpc": "2.0", "id": 4, "method": 5})")["error"]["code"] == -32600);
        CHECK(answer(server, R"({"jsonrpc": "2.0", "id": 4, "method": "x", "params": 1})")["error"]["code"] == -32600);
        CHECK(answer(server, request(5, "resources/list"))["error"]["code"] == -32601);
        CHECK(answer(server, request(6, "tools/call", {{"name", "other"}}))["error"]["code"] == -32602);

        // Valid JSON of the wrong shape is an error of the request, never an exception out of the server.
        CHECK(answer(server, "[1, 2]")["error"]["code"] == -32600);
        CHECK(
            answer(server, R"({"jsonrpc": "2.0", "id": 9, "method": "initialize", "params": {"protocolVersion": 5}})")
            ["error"]["code"] == -32602
        );
        CHECK(
            answer(server, R"({"jsonrpc": "2.0", "id": 9, "method": "tools/call", "params": {"name": 3}})")
            ["error"]["code"] == -32602
        );

        // A client on Windows ends its lines with \r\n.
        const std::string windows_line = request(8, "ping") + "\r";
        CHECK(answer(server, windows_line)["id"] == 8);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acts_waits_and_returns_what_to_look_at_next")
    {
        form window;
        scenario ui(application);
        mcp_server server(ui);
        run_scenario(
            *this,
            [&]
            {
                // The tree lists what a user sees: wait for the window first.
                CHECK_FALSE(call(server, {{"locator", "toggle"}, {"expect", {{"visible", true}}}}).first);
                CHECK_FALSE(call(server, {{"locator", "edit"}, {"expect", {{"visible", true}}}}).first);

                // Without a locator, the tree of names.
                const auto [tree_failed, tree] = call(server, json::object());
                CHECK_FALSE(tree_failed);
                CHECK(tree.find("toggle") != std::string::npos);
                CHECK(tree.find("edit") != std::string::npos);

                // With one, its state.
                const auto [state_failed, state] = call(server, {{"locator", "toggle"}});
                CHECK_FALSE(state_failed);
                CHECK(json::parse(state)["checked"] == false);
                CHECK(json::parse(state)["class"] == "QPushButton");

                // An action, then the expected state, then the new state.
                const auto [click_failed, clicked] =
                    call(server, {{"locator", "toggle"}, {"do", "click"}, {"expect", {{"checked", true}}}});
                CHECK_FALSE(click_failed);
                CHECK(json::parse(clicked)["checked"] == true);

                const auto [type_failed, typed] =
                    call(server, {{"locator", "edit"}, {"do", "type"}, {"text", "abc"}, {"expect", {{"text", "abc"}}}});
                CHECK_FALSE(type_failed);
                CHECK(json::parse(typed)["text"] == "abc");

                const auto [set_failed, set] = call(server, {{"locator", "toggle"}, {"do", "set"}, {"value", false}});
                CHECK_FALSE(set_failed);
                CHECK(json::parse(set)["checked"] == false);

                // A checkable button has a value, as set() gives it.
                const json unchecked             = {{"locator", "toggle"}, {"expect", {{"value", false}}}};
                const auto [value_failed, value] = call(server, unchecked);
                CHECK_MESSAGE(!value_failed, value);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "acts_on_an_action_of_a_toolbar")
    {
        QMainWindow window;
        QToolBar toolbar(&window);
        window.addToolBar(&toolbar);
        QAction* const volume = toolbar.addAction("volume");
        volume->setObjectName("volume");
        volume->setCheckable(true);
        window.show();

        scenario ui(application);
        mcp_server server(ui);
        run_scenario(
            *this,
            [&]
            {
                // An action has no widget state: the call returns the tree, where the action shows.
                const auto [failed, text] = call(server, {{"locator", "volume"}, {"do", "click"}});
                CHECK_FALSE(failed);
                CHECK(text.find("volume") != std::string::npos);
                CHECK(volume->isChecked());

                const auto [read_failed, read] = call(server, {{"locator", "volume"}});
                CHECK_FALSE(read_failed);
                CHECK(read.find("volume") != std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_a_failed_step_as_an_error_of_the_tool")
    {
        form window;
        scenario ui(application);
        mcp_server server(ui);
        run_scenario(
            *this,
            [&]
            {
                const auto failure = [&server](const json& _arguments)
                                     {
                                         const auto [failed, text] = call(server, _arguments);
                                         CHECK(failed);
                                         return text;
                                     };

                CHECK(
                    failure({{"locator", "label"}, {"expect", {{"text", "done"}}}, {"timeout_ms", 200}}).find("ready")
                    != std::string::npos
                );
                CHECK(failure({{"locator", "toggle"}, {"do", "fly"}}).find("unknown verb") != std::string::npos);
                CHECK(failure({{"do", "click"}}).find("needs a locator") != std::string::npos);
                CHECK(failure({{"expect", {{"checked", true}}}}).find("needs a locator") != std::string::npos);
                CHECK(
                    failure({{"locator", "label"}, {"expect", {{"color", "red"}}}}).find("unknown state")
                    != std::string::npos
                );
                CHECK_FALSE(failure({{"locator", "edit"}, {"do", "type"}}).empty());

                // A parameter the call would ignore is refused: a double click is at the centre.
                CHECK(
                    failure({{"locator", "toggle"}, {"do", "double_click"}, {"at", {{"x", 0.1}, {"y", 0.1}}}})
                    .find("'at' is not used by 'double_click'") != std::string::npos
                );
                CHECK(
                    failure({{"locator", "label"}, {"text", "done"}}).find("'text' is not used without 'do'")
                    != std::string::npos
                );

                // The window stays usable after the failures.
                CHECK_FALSE(call(server, {{"locator", "toggle"}, {"do", "click"}}).first);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "serves_its_standard_input_until_the_client_closes_it")
    {
        if(std::getenv("SIGHT_MCP_PROBE") != nullptr)
        {
            // An application has a main window, which the server places as a GUI test does.
            QMainWindow window;
            QLabel label("probe");
            label.setObjectName("probe");
            window.setCentralWidget(&label);
            window.show();
            {
                const mcp_stdio server(application);

                // As in a GUI test, the dialogs are Qt's, which the dialog verbs drive.
                if(!QApplication::testAttribute(Qt::AA_DontUseNativeDialogs))
                {
                    std::_Exit(3);
                }

                // Not on the channel of the protocol.
                std::puts("noise");
                static_cast<void>(std::fflush(stdout));
                QApplication::exec();
            }

            return;
        }

        // Two requests around three too long, then the client leaves: the application ends. The limit is 1 MiB: one
        // byte more is refused even when its end comes in the same read, and a request of several reads once.
        constexpr std::size_t limit = std::size_t {1024} *1024;
        const auto [exit_code, received] = serve_in_a_child(
            request(1, "initialize", {{"protocolVersion", "2025-11-25"}}) + "\n"
            + std::string(std::size_t {1100} *1024, 'x') + "\n"
            + std::string(limit + 1, 'x') + "\n"
            + std::string(3 * limit, 'x') + "\n"
            + request(2, "tools/call", {{"name", "ui"}, {"arguments", json::object()}}) + "\n"
        );
        CAPTURE(received.substr(0, 2000));
        CHECK(exit_code == 0);

        // Only the responses, one per line: the initialization, one refusal per long line, then the tree.
        std::vector<json> responses;
        for(const auto line : std::views::split(std::string_view(received), '\n'))
        {
            if(!line.empty())
            {
                responses.push_back(json::parse(std::string_view(line.begin(), line.end())));
            }
        }

        REQUIRE(responses.size() == 5);
        CHECK(responses[0]["result"]["protocolVersion"] == "2025-11-25");
        CHECK(responses[1]["error"]["code"] == -32600);
        CHECK(responses[2]["error"]["code"] == -32600);
        CHECK(responses[3]["error"]["code"] == -32600);
        CHECK(responses[4]["id"] == 2);
        CHECK(responses[4]["result"]["content"][0]["text"].get<std::string>().find("probe") != std::string::npos);
        CHECK(received.find("noise") == std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "stops_reading_a_console_when_its_application_ends")
    {
        if(std::getenv("SIGHT_MCP_PROBE") != nullptr)
        {
            QMainWindow window;
            QLabel label("probe");
            window.setCentralWidget(&label);
            window.show();
            const mcp_stdio server(application);

            // Nobody types in the console: the application ends by itself, and the server must stop reading.
            QTimer::singleShot(std::chrono::seconds(2), &application, []{QCoreApplication::quit();});
            QApplication::exec();
            return;
        }

#ifdef WIN32
        CHECK(run_on_a_console(L"stops_reading_a_console_when_its_application_ends") == 0);
#else
        CHECK(run_on_a_console("stops_reading_a_console_when_its_application_ends") == 0);
#endif
    }
}

} // namespace sight::ui::test::detail
