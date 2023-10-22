/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "ui/qt/qt_main_frame.hpp"

#include <core/base.hpp>

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QStyle>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::frame, sight::ui::layout::frame_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

frame::~frame()
= default;

//-----------------------------------------------------------------------------

void frame::createFrame()
{
    FrameInfo frame_info = this->getFrameInfo();

    const std::string frame_title = frame_info.m_version.empty() ? frame_info.m_name : frame_info.m_name + " "
                                    + frame_info.m_version;

    auto* mainframe = new ui::qt::qt_main_frame();
    m_qtWindow = mainframe;
    m_qtWindow->setObjectName(QString::fromStdString(frame_info.m_name));
    if(!frame_info.m_qssClass.empty())
    {
        m_qtWindow->setProperty("class", QString::fromStdString(frame_info.m_qssClass));
    }

    ui::qt::qt_main_frame::CloseCallback fct = [this](auto&& ...){onCloseFrame();};
    mainframe->setCloseCallback(fct);

    // cspell: ignore QWIDGETSIZE
    m_qtWindow->setWindowTitle(QString::fromStdString(frame_title));
    m_qtWindow->setMinimumSize(std::max(frame_info.m_minSize.first, 0), std::max(frame_info.m_minSize.second, 0));
    m_qtWindow->setMaximumSize(
        frame_info.m_maxSize.first == -1 ? QWIDGETSIZE_MAX : frame_info.m_maxSize.first,
        frame_info.m_maxSize.second == -1 ? QWIDGETSIZE_MAX : frame_info.m_maxSize.second
    );

    if(!frame_info.m_iconPath.empty())
    {
        QIcon icon(QString::fromStdString(frame_info.m_iconPath.string()));
        SIGHT_ASSERT("Unable to create an icon instance from " << frame_info.m_iconPath.string(), !icon.isNull());
        m_qtWindow->setWindowIcon(icon);
    }

    if(!qApp->activeWindow())
    {
        qApp->setActiveWindow(m_qtWindow);
    }

    if(frame_info.m_style == ui::layout::frame_manager::STAY_ON_TOP)
    {
        m_qtWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    else if(frame_info.m_style == ui::layout::frame_manager::MODAL)
    {
        m_qtWindow->setWindowModality(Qt::ApplicationModal);
    }

    int size_x = (frame_info.m_size.first > 0) ? frame_info.m_size.first : m_qtWindow->size().width();
    int size_y = (frame_info.m_size.second > 0) ? frame_info.m_size.second : m_qtWindow->size().height();

    int pos_x = frame_info.m_position.first;
    int pos_y = frame_info.m_position.second;
    QPoint pos(pos_x, pos_y);
    const QScreen* screen = QGuiApplication::screenAt(pos);
    if(screen == nullptr)
    {
        QRect frame_rect(0, 0, size_x, size_y);
        frame_rect.moveCenter(QGuiApplication::primaryScreen()->geometry().center());
        pos = frame_rect.topLeft();
    }

    m_qtWindow->setGeometry(pos.x(), pos.y(), size_x, size_y);

    this->setState(frame_info.m_state);

    auto* qwidget = new QWidget(m_qtWindow);
    m_qtWindow->setCentralWidget(qwidget);

    QObject::connect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    ui::qt::container::widget::sptr container = ui::qt::container::widget::make();
    container->setQtContainer(qwidget);
    m_container = container;

    ui::qt::container::widget::sptr frame_container = ui::qt::container::widget::make();
    frame_container->setQtContainer(m_qtWindow);
    m_frame = frame_container;
    m_frame->setVisible(frame_info.m_visibility);
}

//-----------------------------------------------------------------------------

void frame::destroyFrame()
{
    this->getFrameInfo().m_state           = this->getState();
    this->getFrameInfo().m_size.first      = m_qtWindow->size().width();
    this->getFrameInfo().m_size.second     = m_qtWindow->size().height();
    this->getFrameInfo().m_position.first  = m_qtWindow->geometry().x();
    this->getFrameInfo().m_position.second = m_qtWindow->geometry().y();
    this->writeConfig();

    QObject::disconnect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    m_container->destroyContainer();

    // m_qtWindow is cleaned/destroyed by m_frame
    m_frame->destroyContainer();
}

//-----------------------------------------------------------------------------

void frame::onCloseFrame()
{
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

void frame::setState(FrameState _state)
{
    // Updates the window state.
    switch(_state)
    {
        case FrameState::ICONIZED:
            m_qtWindow->showMinimized();
            break;

        case FrameState::MAXIMIZED:
            m_qtWindow->showMaximized();
            break;

        case FrameState::FULL_SCREEN:
            m_qtWindow->showFullScreen();
            break;

        default:
            m_qtWindow->showNormal();
    }
}

//-----------------------------------------------------------------------------

ui::layout::frame_manager::FrameState frame::getState()
{
    FrameState state(FrameState::UNKNOWN);

    if(m_qtWindow->isMinimized())
    {
        state = FrameState::ICONIZED;
    }
    else if(m_qtWindow->isMaximized())
    {
        state = FrameState::MAXIMIZED;
    }
    else if(m_qtWindow->isFullScreen())
    {
        state = FrameState::FULL_SCREEN;
    }

    return state;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
