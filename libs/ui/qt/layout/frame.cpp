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

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QShortcut>
#include <QStyle>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::frame, sight::ui::layout::frame_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void frame::create_frame()
{
    frame_info frame_info = this->get_frame_info();

    const std::string frame_title = frame_info.m_version.empty() ? frame_info.m_name : frame_info.m_name + " "
                                    + frame_info.m_version;

    auto* mainframe = new ui::qt::main_frame();
    m_qt_window = mainframe;
    m_qt_window->setObjectName(QString::fromStdString(frame_info.m_name));
    if(!frame_info.m_qss_class.empty())
    {
        m_qt_window->setProperty("class", QString::fromStdString(frame_info.m_qss_class));
    }

    ui::qt::main_frame::CloseCallback fct = [this](auto&& ...){on_close_frame();};
    mainframe->set_close_callback(fct);

    // cspell: ignore QWIDGETSIZE
    m_qt_window->setWindowTitle(QString::fromStdString(frame_title));
    m_qt_window->setMinimumSize(std::max(frame_info.m_min_size.first, 0), std::max(frame_info.m_min_size.second, 0));
    m_qt_window->setMaximumSize(
        frame_info.m_max_size.first == -1 ? QWIDGETSIZE_MAX : frame_info.m_max_size.first,
        frame_info.m_max_size.second == -1 ? QWIDGETSIZE_MAX : frame_info.m_max_size.second
    );
    m_qt_window->adjustSize();

    if(!frame_info.m_icon_path.empty())
    {
        QIcon icon(QString::fromStdString(frame_info.m_icon_path.string()));
        SIGHT_ASSERT("Unable to create an icon instance from " << frame_info.m_icon_path.string(), !icon.isNull());
        m_qt_window->setWindowIcon(icon);
    }

    if(!qApp->activeWindow())
    {
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
    else if(frame_info.m_style == ui::layout::frame_manager::fullscreen && frame_info.m_state == frame_state::unknown)
    {
        frame_info.m_state = frame_state::full_screen;
    }
    else if(frame_info.m_style == ui::layout::frame_manager::frameless)
    {
        m_qt_window->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }

    const auto& [width, height] =
        [&]
        {
            if(frame_info.m_size.first > 0 && frame_info.m_size.second > 0)
            {
                return frame_info.m_size;
            }

            return std::make_pair(m_qt_window->width(), m_qt_window->height());
        }();

    QPoint pos(frame_info.m_position.first, frame_info.m_position.second);

    // Get the screen at the position
    auto* screen_at_pos = QGuiApplication::screenAt(pos);

    // We need to display the window on a specific screen
    if(frame_info.m_screen >= 0)
    {
        QRect frame_rect(0, 0, width, height);

        // Get the position in the wanted screen - nothing to do if the screen is already the screen at pos
        if(const auto& screens = QGuiApplication::screens();
           frame_info.m_screen >= screens.size() && screen_at_pos == nullptr)
        {
            // Wanted screen doesn't exist and no screen at pos, use default screen
            frame_rect.moveCenter(QGuiApplication::primaryScreen()->availableGeometry().center());
            pos = frame_rect.topLeft();
        }
        else if(screen_at_pos == nullptr || screens.indexOf(screen_at_pos, 0) != frame_info.m_screen)
        {
            // Wanted screen is not the same as screen at, use it
            frame_rect.moveCenter(screens.at(frame_info.m_screen)->availableGeometry().center());
            pos = frame_rect.topLeft();
        }
    }
    else if(screen_at_pos == nullptr)
    {
        // No screen information at all, use default screen
        QRect frame_rect(0, 0, width, height);
        frame_rect.moveCenter(QGuiApplication::primaryScreen()->availableGeometry().center());
        pos = frame_rect.topLeft();
    }

    m_qt_window->setGeometry(pos.x(), pos.y(), width, height);

    this->set_state(frame_info.m_state);

    auto* qwidget = new QWidget(m_qt_window);
    m_qt_window->setCentralWidget(qwidget);

    QObject::connect(m_qt_window, &QMainWindow::destroyed, this, &frame::on_close_frame);

    m_shortcut = new QShortcut(
        QKeySequence::FullScreen,
        m_qt_window,
        [&]
        {
            if(m_qt_window->isFullScreen())
            {
                m_qt_window->showNormal();
            }
            else
            {
                m_qt_window->showFullScreen();
            }
        });

    ui::qt::container::widget::sptr container = ui::qt::container::widget::make();
    container->set_qt_container(qwidget);
    m_container = container;

    ui::qt::container::widget::sptr frame_container = ui::qt::container::widget::make();
    frame_container->set_qt_container(m_qt_window);
    m_frame = frame_container;
    m_frame->set_visible(frame_info.m_visibility);
}

//-----------------------------------------------------------------------------

void frame::destroy_frame()
{
    this->get_frame_info().m_state           = this->get_state();
    this->get_frame_info().m_size.first      = m_qt_window->size().width();
    this->get_frame_info().m_size.second     = m_qt_window->size().height();
    this->get_frame_info().m_position.first  = m_qt_window->geometry().x();
    this->get_frame_info().m_position.second = m_qt_window->geometry().y();
    this->write_config();

    QObject::connect(m_qt_window, &QMainWindow::destroyed, this, &frame::on_close_frame);

    m_container->destroy_container();

    // m_qtWindow is cleaned/destroyed by m_frame
    m_frame->destroy_container();
}

//-----------------------------------------------------------------------------

void frame::on_close_frame()
{
    this->m_close_callback();
}

//-----------------------------------------------------------------------------

void frame::set_state(frame_state _state)
{
    // Updates the window state.
    switch(_state)
    {
        case frame_state::iconized:
            m_qt_window->showMinimized();
            break;

        case frame_state::maximized:
            m_qt_window->showMaximized();
            break;

        case frame_state::full_screen:
            m_qt_window->showFullScreen();
            break;

        default:
            m_qt_window->showNormal();
    }
}

//-----------------------------------------------------------------------------

ui::layout::frame_manager::frame_state frame::get_state()
{
    if(m_qt_window->isMinimized())
    {
        return frame_state::iconized;
    }

    if(m_qt_window->isMaximized())
    {
        return frame_state::maximized;
    }

    if(m_qt_window->isFullScreen())
    {
        return frame_state::full_screen;
    }

    return frame_state::normal;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
