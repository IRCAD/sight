/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/layout/frame.hpp"
#include "ui/qt/main_frame.hpp"

#include <core/base.hpp>
#include <core/enum_flag_operators.hxx>

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QShortcut>
#include <QStyle>
#include <QWindow>
#include <QTimer>

#ifdef _WIN32
#include <windows.h>
#endif

SIGHT_REGISTER_GUI(sight::ui::qt::layout::frame, sight::ui::layout::frame_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void frame::create_frame()
{
    frame_info frame_info = this->get_frame_info();

    const std::string frame_title =
        [&frame_info]
        {
            std::string title;

            if(frame_info.m_name && !frame_info.m_name->empty())
            {
                title = *frame_info.m_name;
            }

            if(frame_info.m_version && !frame_info.m_version->empty())
            {
                if(!title.empty())
                {
                    title += " ";
                }

                title += *frame_info.m_version;
            }

            return title;
        }();

    auto* mainframe = new ui::qt::main_frame();
    m_qt_window = mainframe;

    if(frame_info.m_name && !frame_info.m_name->empty())
    {
        m_qt_window->setObjectName(QString::fromStdString(*frame_info.m_name));
    }

    if(frame_info.m_qss_class && !frame_info.m_qss_class->empty())
    {
        m_qt_window->setProperty("class", QString::fromStdString(*frame_info.m_qss_class));
    }

    ui::qt::main_frame::CloseCallback fct = [this](auto&& ...){on_close_frame();};
    mainframe->set_close_callback(fct);

    // cspell: ignore QWIDGETSIZE
    m_qt_window->setWindowTitle(QString::fromStdString(frame_title));

    // Set a minimum size if needed
    const auto minimum = m_qt_window->minimumSizeHint();

    if(frame_info.m_min_width && *frame_info.m_min_width > minimum.width())
    {
        m_qt_window->setMinimumWidth(*frame_info.m_min_width);
    }

    if(frame_info.m_min_height && *frame_info.m_min_height > minimum.height())
    {
        m_qt_window->setMinimumHeight(*frame_info.m_min_height);
    }

    // Set a maximum size if needed
    if(frame_info.m_max_width && *frame_info.m_max_width > 0)
    {
        m_qt_window->setMaximumWidth(*frame_info.m_max_width);
    }

    if(frame_info.m_max_height && *frame_info.m_max_height > 0)
    {
        m_qt_window->setMaximumHeight(*frame_info.m_max_height);
    }

    m_qt_window->adjustSize();

    if(frame_info.m_icon_path && !frame_info.m_icon_path->empty())
    {
        QIcon icon(QString::fromStdString(frame_info.m_icon_path->string()));
        SIGHT_ASSERT("Unable to create an icon instance from " << frame_info.m_icon_path->string(), !icon.isNull());
        m_qt_window->setWindowIcon(icon);
    }

    if(!qApp->activeWindow())
    {
        // This is needed to avoid crash in many tests when we call qApp->activeWindow()
        // qApp->setActiveWindow() is deprecated and QWindow::activateWindow() should be used, but unfortunately
        // qApp->activeWindow() will return nullptr the time to process the activation, and will break the tests.
        qApp->setActiveWindow(m_qt_window);
    }

    if(frame_info.m_style == ui::layout::frame_manager::stay_on_top)
    {
        m_qt_window->setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    else if(frame_info.m_style == ui::layout::frame_manager::modal)
    {
        m_qt_window->setWindowModality(Qt::ApplicationModal);
    }
    else if(frame_info.m_style == ui::layout::frame_manager::fullscreen && !frame_info.m_state)
    {
        frame_info.m_state = frame_state::full_screen;
    }
    else if(frame_info.m_style == ui::layout::frame_manager::frameless)
    {
        m_qt_window->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }

    // Get the size hint of the window
    const auto frame_size = m_qt_window->size().isValid() ? m_qt_window->size() : m_qt_window->sizeHint();

    // Use stored position and size (if available from preference)
    QRect frame_rect(
        frame_info.m_x.value_or(0),
        frame_info.m_y.value_or(0),
        frame_info.m_width
        && *frame_info.m_width > m_qt_window->minimumWidth()
        && *frame_info.m_width < m_qt_window->maximumWidth()
        ? *frame_info.m_width : frame_size.width(),
        frame_info.m_height
        && *frame_info.m_height > m_qt_window->minimumHeight()
        && *frame_info.m_height < m_qt_window->maximumHeight()
        ? *frame_info.m_height : frame_size.height()
    );

    // Get the screen at the center of the window (if available from preference)
    auto* const preferences_screen = frame_info.m_width && frame_info.m_height
                                     ? QGuiApplication::screenAt(frame_rect.center()) : nullptr;

    // Screen position read from preferences.
    const bool preferences_screen_valid = (preferences_screen != nullptr);

    // A screen index as been set, test if it exists.
    const auto& screens                = QGuiApplication::screens();
    const bool configured_screen_valid = (frame_info.m_configured_screen
                                          && *frame_info.m_configured_screen >= 0
                                          && *frame_info.m_configured_screen < screens.size());

    if(preferences_screen_valid)
    {
        // If the screen from preferences is valid, we still need to check if the configuration have changed
        if(configured_screen_valid && *frame_info.m_configured_screen != *frame_info.m_saved_screen)
        {
            frame_rect.moveCenter(screens.at(*frame_info.m_configured_screen)->availableGeometry().center());
        }
    }
    else if(configured_screen_valid)
    {
        // If there is no screen from preferences, we can use the configured screen
        frame_rect.moveCenter(screens.at(*frame_info.m_configured_screen)->availableGeometry().center());
    }
    else
    {
        // Last resort, use the primary screen
        frame_rect.moveCenter(QGuiApplication::primaryScreen()->availableGeometry().center());
    }

    // Set the geometry will also place the window on the correct screen
    m_qt_window->setGeometry(frame_rect);

#ifndef _WIN32
    /// @note: On Linux, calling showXXX() is mandatory. On Windows setWindowState() is enough.
    if(frame_info.m_state)
    {
        if((*frame_info.m_state & frame_state::full_screen) == frame_state::full_screen)
        {
            m_qt_window->showFullScreen();
        }
        else if((*frame_info.m_state & frame_state::maximized) == frame_state::maximized)
        {
            m_qt_window->showMaximized();
        }
        else if((*frame_info.m_state & frame_state::iconized) == frame_state::iconized)
        {
            m_qt_window->showMinimized();
        }
        else
        {
            m_qt_window->showNormal();
        }
    }
    else
    {
        // Default behavior
        m_qt_window->showNormal();
    }
#endif

    // Restore the window state
    if(frame_info.m_state)
    {
        if((*frame_info.m_state & frame_state::iconized) == frame_state::iconized
           && (m_qt_window->windowState() & Qt::WindowMinimized) != Qt::WindowMinimized)
        {
            m_qt_window->setWindowState(m_qt_window->windowState() | Qt::WindowMinimized);
        }

        if((*frame_info.m_state & frame_state::maximized) == frame_state::maximized
           && (m_qt_window->windowState() & Qt::WindowMaximized) != Qt::WindowMaximized)
        {
            m_qt_window->setWindowState(m_qt_window->windowState() | Qt::WindowMaximized);
        }

        if((*frame_info.m_state & frame_state::full_screen) == frame_state::full_screen
           && (m_qt_window->windowState() & Qt::WindowFullScreen) != Qt::WindowFullScreen)
        {
            this->set_full_screen(true);
        }
    }

#ifndef _WIN32
    /// @note: Workaround to force the first window state on linux, when using multiple windows.
    ///        Without this, the window state is ignored and the window appear in "normal" state.
    if(frame_info.m_state)
    {
        const auto set_window_state =
            [qt_window = QPointer(m_qt_window)](Qt::WindowState _state)
            {
                QTimer::singleShot(
                    0,
                    [qt_window, _state]
                {
                    if(!qt_window.isNull())
                    {
                        qt_window->setWindowState(qt_window->windowState() | _state);
                    }
                });
            };

        if((*frame_info.m_state & frame_state::iconized) == frame_state::iconized)
        {
            set_window_state(Qt::WindowMinimized);
        }

        if((*frame_info.m_state & frame_state::maximized) == frame_state::maximized)
        {
            set_window_state(Qt::WindowMaximized);
        }

        if((*frame_info.m_state & frame_state::full_screen) == frame_state::full_screen)
        {
            set_window_state(Qt::WindowFullScreen);
        }
    }
#endif

    auto* central_widget = new QWidget(m_qt_window);
    m_qt_window->setCentralWidget(central_widget);

    QObject::connect(m_qt_window, &QMainWindow::destroyed, this, &frame::on_close_frame);

    m_shortcut = new QShortcut(
        QKeySequence::FullScreen,
        m_qt_window,
        [&]
        {
            // Toggle the full screen state, but do not change the other states (active, minimized, etc.)
            this->set_full_screen((m_qt_window->windowState() & Qt::WindowFullScreen) != Qt::WindowFullScreen);
        });

    ui::qt::container::widget::sptr container = ui::qt::container::widget::make();
    container->set_qt_container(central_widget);
    m_container = container;

    ui::qt::container::widget::sptr frame_container = ui::qt::container::widget::make();
    frame_container->set_qt_container(m_qt_window);
    m_frame = frame_container;
    m_frame->set_visible(frame_info.m_visibility.value_or(true));

    if(frame_info.m_visibility)
    {
        m_qt_window->activateWindow();
        m_qt_window->raise();
    }
}

//-----------------------------------------------------------------------------

void frame::destroy_frame()
{
    auto& frame_info  = this->get_frame_info();
    frame_state state = frame_state::normal;

    // Save the frame position and size
    // Get the Qt states
    const auto window_states = m_qt_window->windowState();

    if((window_states& Qt::WindowFullScreen) == Qt::WindowFullScreen)
    {
        state |= frame_state::full_screen;
    }

    if((window_states& Qt::WindowMaximized) == Qt::WindowMaximized)
    {
        state |= frame_state::maximized;
    }

    if((window_states& Qt::WindowMinimized) == Qt::WindowMinimized)
    {
        state |= frame_state::iconized;
    }

    frame_info.m_state = state;

    // Get the Qt geometry
    auto window_geometry = m_qt_window->geometry();

    // Do not save the raw size or the raw position if the window is maximized or iconized
    // Instead, we save the size and position of the window using sizehint / center, so when the user restore
    // it, it will have a less than full screen size and will be able to see the other windows and the title bar.
    // Ideally, we should save the size and position of the window in normal state, but this should be enough for now.
    if(frame_info.m_state != frame_state::normal)
    {
        m_qt_window->adjustSize();
        const auto size = m_qt_window->size();
        window_geometry.setSize(size.isValid() ? size : m_qt_window->sizeHint());
        window_geometry.moveCenter(m_qt_window->screen()->availableGeometry().center());
    }

    frame_info.m_x      = window_geometry.x();
    frame_info.m_y      = window_geometry.y();
    frame_info.m_width  = window_geometry.width();
    frame_info.m_height = window_geometry.height();

    this->write_config();

    m_container->destroy_container();

    // m_qt_window is cleaned/destroyed by m_frame
    m_frame->destroy_container();
}

//-----------------------------------------------------------------------------

void frame::on_close_frame()
{
    this->m_close_callback();
}

//------------------------------------------------------------------------------

void frame::set_full_screen(bool _full_screen)
{
    if(_full_screen && (m_qt_window->windowState() & Qt::WindowFullScreen) != Qt::WindowFullScreen)
    {
        m_qt_window->setWindowState(m_qt_window->windowState() | Qt::WindowFullScreen);

#ifdef _WIN32
        // This is a workaround for OpenGLWidget and fullscreen mode on Windows
        // Without the "fake" border, all popup, dialogs and menu are hidden behind the main window
        // Normally, this "hack" is done inside Qt private code, but it seems to be broken.
        // See https://bugreports.qt.io/browse/QTBUG-104076
        const auto hwnd  = reinterpret_cast<HWND>(m_qt_window->winId());
        const auto style = GetWindowLongPtr(hwnd, GWL_STYLE) | WS_BORDER;
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
#endif
    }
    else if(!_full_screen && (m_qt_window->windowState() & Qt::WindowFullScreen) == Qt::WindowFullScreen)
    {
#ifdef _WIN32
        // Revert back the "hack" done in set_full_screen
        const auto hwnd  = reinterpret_cast<HWND>(m_qt_window->winId());
        const auto style = GetWindowLongPtr(hwnd, GWL_STYLE) ^ WS_BORDER;
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
#endif
        m_qt_window->setWindowState(m_qt_window->windowState() ^ Qt::WindowFullScreen);
    }
}

} // namespace sight::ui::qt::layout
