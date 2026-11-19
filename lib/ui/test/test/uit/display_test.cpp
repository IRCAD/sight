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

#include <doctest/doctest.h>

#ifdef __linux__
#include <dlfcn.h>
#endif

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <thread>

TEST_SUITE("sight::ui::test::display")
{
    // Checks that the display of a GUI test takes a client back at once: 200 times in a row, a client connects, asks
    // the server one question and leaves, and no connection may be refused or blocked.
    // Why: the launcher gives each GUI test an Xvfb of its own, and the test leaves it and comes back at once, as each
    // test case recreates its application. By default Xvfb resets when its last client leaves, and refuses or never
    // answers a client coming meanwhile: Qt then aborts or hangs, a few times per thousand under load.
    TEST_CASE("takes_a_client_back_at_once")
    {
#ifdef __linux__
        if(std::getenv("DISPLAY") == nullptr)
        {
            MESSAGE("no X display: nothing to test");
            return;
        }

        // The X client library of Qt, loaded here so that the test needs nothing more to build.
        void* const xcb = dlopen("libxcb.so.1", RTLD_NOW);
        REQUIRE(xcb != nullptr);

        struct cookie
        {
            unsigned int sequence;
        };

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): C functions of a library loaded at runtime.
        const auto connect    = reinterpret_cast<void* (*)(const char*, int*)>(dlsym(xcb, "xcb_connect"));
        const auto has_error  = reinterpret_cast<int (*)(void*)>(dlsym(xcb, "xcb_connection_has_error"));
        const auto disconnect = reinterpret_cast<void (*)(void*)>(dlsym(xcb, "xcb_disconnect"));
        const auto focus      = reinterpret_cast<cookie (*)(void*)>(dlsym(xcb, "xcb_get_input_focus"));
        using reply_t = void* (*)(void*, cookie, void**);
        const auto focus_reply = reinterpret_cast<reply_t>(dlsym(xcb, "xcb_get_input_focus_reply"));
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        REQUIRE(connect != nullptr);
        REQUIRE(has_error != nullptr);
        REQUIRE(disconnect != nullptr);
        REQUIRE(focus != nullptr);
        REQUIRE(focus_reply != nullptr);

        // Connects, makes a round trip and leaves, at once, as many times. A blocked connection never returns: the
        // loop runs aside, and the test fails when it is late.
        constexpr int count = 200;
        auto refused        = std::make_shared<std::atomic_int>(0);
        auto done           = std::make_shared<std::atomic_bool>(false);
        std::thread(
            [ = ]
        {
            for(int i = 0 ; i < count ; ++i)
            {
                void* const connection = connect(nullptr, nullptr);
                if(has_error(connection) != 0)
                {
                    ++*refused;
                }
                else
                {
                    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,hicpp-no-malloc): replies of xcb come from malloc.
                    std::free(focus_reply(connection, focus(connection), nullptr));
                }

                disconnect(connection);
            }

            *done = true;
        }).detach();

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
        while(!*done && std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE_MESSAGE(done->load(), "a connection to the display is still blocked after 30 s");
        CHECK(refused->load() == 0);
#else // ifdef __linux__
        MESSAGE("no X server on this system: nothing to test");
#endif // ifdef __linux__
    }
}
