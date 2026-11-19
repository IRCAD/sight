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

#pragma once

#include <sight/ui/test/config.hpp>

#include "ui/test/scenario.hpp"

#include <QApplication>

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace sight::ui::test
{

/**
 * @brief Answers the messages of an MCP client with a scenario, so that an agent explores a running application the
 * way a GUI test drives it.
 *
 * It has one tool, `ui`, which takes the locators of a scenario and a subset of its verbs and states: `{}` returns the
 * tree of names of ui.dump(), `{"locator": "x"}` the state of a widget; `do` acts first and `expect` waits for a state,
 * then the call returns what to look at next.
 *
 * @code{.cpp}
 * sight::ui::test::scenario ui(application);
 * sight::ui::test::mcp_server server(ui);
 * const auto response = server.answer(R"({"jsonrpc": "2.0", "id": 1, "method": "tools/list"})");
 * @endcode
 *
 * Calls run one after the other on the thread of answer(), never the GUI thread. They act as a user does, and never
 * write data or call a slot.
 */
class SIGHT_UI_TEST_CLASS_API mcp_server final
{
public:

    /// @param _ui the scenario that drives the application.
    SIGHT_UI_TEST_API explicit mcp_server(scenario& _ui);

    /**
     * @brief Answers one JSON-RPC message of the client.
     *
     * @param _message one line of the client.
     * @return the response line, or nothing for a notification.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::optional<std::string> answer(std::string_view _message);

private:

    scenario& m_ui;
};

/**
 * @brief Serves the running application to an MCP client on the standard input and output of the process, until the
 * client closes its input, which ends the application.
 *
 * Standard output then belongs to the protocol: anything else the process prints there goes to standard error.
 *
 * @code{.cpp}
 * sight::ui::test::mcp_stdio::prepare();
 * profile->start();
 * const sight::ui::test::mcp_stdio server(*qobject_cast<QApplication*>(QCoreApplication::instance()));
 * profile->run();
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API mcp_stdio final
{
public:

    /// @param _application the running application, with its modules loaded: their XML names the locators.
    SIGHT_UI_TEST_API explicit mcp_stdio(QApplication& _application);

    /**
     * @brief Prepares the process as a GUI test does, before its profile starts: standard output belongs to the
     * protocol from now on, the preferences of the user are neither read nor written, and dialogs are Qt's.
     */
    SIGHT_UI_TEST_API static void prepare();

    /// Stops reading once the application stopped; a call in progress fails with the event loop.
    SIGHT_UI_TEST_API ~mcp_stdio();

    mcp_stdio(const mcp_stdio&)            = delete;
    mcp_stdio(mcp_stdio&&)                 = delete;
    mcp_stdio& operator=(const mcp_stdio&) = delete;
    mcp_stdio& operator=(mcp_stdio&&)      = delete;

private:

    struct implementation;
    std::unique_ptr<implementation> m_implementation;
};

} // namespace sight::ui::test
