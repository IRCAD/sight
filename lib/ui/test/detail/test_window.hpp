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

#include "gui_gateway.hpp"

#include <QPoint>
#include <QPointer>
#include <QScreen>
#include <QSize>
#include <QWidget>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

namespace sight::ui::test::detail
{

/// Environment setup, not an application action: every scenario starts with the same client area, or with the
/// smallest one its window allows, since a window cannot be smaller than its content.
inline void prepare_test_window(gui_gateway& _gateway, QPointer<QWidget> _window, gui_deadline _deadline)
{
    struct setup
    {
        bool ready {false};
        QSize size;
        QSize target;
    };

    setup last;
    while(std::chrono::steady_clock::now() < _deadline)
    {
        // By value: a call still running at the deadline outlives this frame.
        try
        {
            last = _gateway.call_until(
                _deadline,
                [_window]() -> setup
                {
                    if(_window.isNull())
                    {
                        throw std::runtime_error("the scenario main window was destroyed during setup");
                    }

                    const QSize target = QSize(1280, 800).expandedTo(_window->minimumSize());
                    const QSize size   = _window->size();

                    const QScreen* const screen = _window->screen();

                    // Wherever the system put it, its whole frame is then on the screen when it fits there.
                    const QPoint place = screen == nullptr ? _window->pos() : screen->availableGeometry().topLeft();
                    if(size == target && _window->pos() == place && !_window->isMaximized())
                    {
                        return {.ready = true, .size = size, .target = target};
                    }

                    // Asked again each time: a window manager may have put it back since.
                    _window->showNormal();
                    _window->resize(target);
                    _window->move(place);
                    return {.ready = false, .size = size, .target = target};
                });
        }
        catch(const std::runtime_error&)
        {
            // A last poll cut by the deadline: the failure is what the window did until then.
            if(!_gateway.is_loop_running() || std::chrono::steady_clock::now() < _deadline || last.target.isEmpty())
            {
                throw;
            }

            break;
        }

        if(last.ready)
        {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    throw std::runtime_error(
              "the scenario main window did not reach a client area of "
              + std::to_string(last.target.width()) + "x" + std::to_string(last.target.height())
              + ": it stays at " + std::to_string(last.size.width()) + "x" + std::to_string(last.size.height())
    );
}

} // namespace sight::ui::test::detail
