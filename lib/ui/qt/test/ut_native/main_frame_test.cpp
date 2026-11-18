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

#include <ui/qt/main_frame.hpp>
#include <ui/test/qt_application.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QMainWindow>
#include <QSurface>
#include <QWindow>

TEST_SUITE("sight::ui::qt::main_frame")
{
    // Its first show makes its window an OpenGL one. Before Qt 6.8, that recreates the native window: done while the
    // window was shown, it left a modal frame out of the modal windows of Qt 6.4, which then blocked nothing.
    TEST_CASE("stays_modal_once_shown")
    {
        // The real platform of the system, which a GUI test gets on a display of its own.
        const sight::ui::test::qt_application application {"ui_qt_native_ut", sight::ui::test::qt_platform::system};
        QMainWindow main;
        main.resize(400, 300);
        main.show();

        sight::ui::qt::main_frame frame;
        frame.resize(200, 100);
        frame.setWindowModality(Qt::ApplicationModal);
        frame.show();

        CHECK(QApplication::activeModalWidget() == &frame);
        REQUIRE(frame.windowHandle() != nullptr);
        CHECK(frame.windowHandle()->surfaceType() == QSurface::OpenGLSurface);
    }
}
