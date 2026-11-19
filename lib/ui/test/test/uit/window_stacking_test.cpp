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

#include "../ut/gui_test_support.hpp"

#include <ui/test/scenario.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <chrono>
#include <iostream>
#include <string>

namespace sight::ui::test::detail
{

/// A window that takes the whole screen, on top of everything.
static void cover_the_screen(QWidget& _window)
{
    _window.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    _window.setGeometry(QApplication::primaryScreen()->geometry());
    _window.show();
}

//------------------------------------------------------------------------------

namespace
{

/// Another application with a window over everything, until it is ended.
class foreign_window
{
public:

    foreign_window()
    {
        // This test again, in the process below: the environment tells it to play the other application.
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        environment.insert("SIGHT_UI_TEST_FOREIGN_WINDOW", "1");
        environment.remove("SIGHT_TEST_PARENT");
        m_process.setProcessEnvironment(environment);
        m_process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        m_process.start(
            QString::fromStdString(boost::dll::program_location().string()),
            {"--test-case=fails_while_another_application_covers_the_target"
            });
    }

    foreign_window(const foreign_window&)            = delete;
    foreign_window(foreign_window&&)                 = delete;
    foreign_window& operator=(const foreign_window&) = delete;
    foreign_window& operator=(foreign_window&&)      = delete;

    ~foreign_window()
    {
        end();
    }

    /// Waits for its window to be there. Only the application knows: it says so on its output.
    [[nodiscard]] bool wait_until_shown()
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::minutes(2);
        while(std::chrono::steady_clock::now() < deadline && m_process.state() != QProcess::NotRunning)
        {
            m_process.waitForReadyRead(100);
            m_output += m_process.readAllStandardOutput();
            if(m_output.contains("shown"))
            {
                return true;
            }
        }

        return false;
    }

    //------------------------------------------------------------------------------

    void end()
    {
        if(m_process.state() != QProcess::NotRunning)
        {
            m_process.kill();
            m_process.waitForFinished(30000);
        }
    }

private:

    QProcess m_process;
    QByteArray m_output;
};

} // namespace

//------------------------------------------------------------------------------

/// Plays the other application: its window, until the test ends this process.
static void play_the_foreign_application(QApplication& _application)
{
    QWidget window;
    cover_the_screen(window);
    QTimer poll;
    QObject::connect(
        &poll,
        &QTimer::timeout,
        [&window]
        {
            if(window.windowHandle() != nullptr && window.windowHandle()->isExposed())
            {
                std::cout << "shown" << std::endl;
            }
        });
    poll.start(std::chrono::milliseconds(50));
    QApplication::exec();
    static_cast<void>(_application);
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::test::window_stacking")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_while_a_frameless_window_covers_the_target")
    {
        QPushButton target("target");
        target.setObjectName("target");
        target.resize(300, 200);
        target.show();
        int clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&clicks]{++clicks;});

        QWidget cover;
        cover.setObjectName("cover");
        cover_the_screen(cover);

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout){ui.click("target", {}, _timeout);},
                    [](const std::string& _message){return _message.find("'cover") != std::string::npos;});
                CHECK(failure.find("'cover") != std::string::npos);
                CHECK(clicks == 0);

                // Once it is gone, the same click goes through.
                QMetaObject::invokeMethod(&cover, &QWidget::hide, Qt::QueuedConnection);
                ui.click("target");
            });
        CHECK(clicks == 1);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_while_another_application_covers_the_target")
    {
        if(qEnvironmentVariableIsSet("SIGHT_UI_TEST_FOREIGN_WINDOW"))
        {
            play_the_foreign_application(application);
            return;
        }

        QPushButton target("target");
        target.setObjectName("target");
        target.resize(300, 200);
        target.show();
        int clicks = 0;
        QObject::connect(&target, &QPushButton::clicked, [&clicks]{++clicks;});

        foreign_window foreign;
        REQUIRE(foreign.wait_until_shown());

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout){ui.click("target", {}, _timeout);},
                    [](const std::string& _message){return _message.find("another application") != std::string::npos;});
                CHECK(failure.find("another application") != std::string::npos);
                CHECK(clicks == 0);
            });
    }
}

} // namespace sight::ui::test::detail
