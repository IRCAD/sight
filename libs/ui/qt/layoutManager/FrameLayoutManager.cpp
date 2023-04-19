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

#include "ui/qt/layoutManager/FrameLayoutManager.hpp"

#include "ui/qt/QtMainFrame.hpp"

#include <core/base.hpp>

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QStyle>

SIGHT_REGISTER_GUI(
    sight::ui::qt::FrameLayoutManager,
    sight::ui::base::layoutManager::IFrameLayoutManager::REGISTRY_KEY
);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

FrameLayoutManager::FrameLayoutManager(ui::base::GuiBaseObject::Key /*key*/)
{
}

//-----------------------------------------------------------------------------

FrameLayoutManager::~FrameLayoutManager()
= default;

//-----------------------------------------------------------------------------

void FrameLayoutManager::createFrame()
{
    FrameInfo frameInfo = this->getFrameInfo();

    auto* mainframe = new ui::qt::QtMainFrame();
    m_qtWindow = mainframe;
    m_qtWindow->setObjectName(QString::fromStdString(frameInfo.m_name));

    ui::qt::QtMainFrame::CloseCallback fct = [this](auto&& ...){onCloseFrame();};
    mainframe->setCloseCallback(fct);

    // cspell: ignore QWIDGETSIZE
    m_qtWindow->setWindowTitle(QString::fromStdString(frameInfo.m_name));
    m_qtWindow->setMinimumSize(std::max(frameInfo.m_minSize.first, 0), std::max(frameInfo.m_minSize.second, 0));
    m_qtWindow->setMaximumSize(
        frameInfo.m_maxSize.first == -1 ? QWIDGETSIZE_MAX : frameInfo.m_maxSize.first,
        frameInfo.m_maxSize.second == -1 ? QWIDGETSIZE_MAX : frameInfo.m_maxSize.second
    );

    if(!frameInfo.m_iconPath.empty())
    {
        QIcon icon(QString::fromStdString(frameInfo.m_iconPath.string()));
        SIGHT_ASSERT("Unable to create an icon instance from " << frameInfo.m_iconPath.string(), !icon.isNull());
        m_qtWindow->setWindowIcon(icon);
    }

    if(!qApp->activeWindow())
    {
        qApp->setActiveWindow(m_qtWindow);
    }

    if(frameInfo.m_style == ui::base::layoutManager::IFrameLayoutManager::STAY_ON_TOP)
    {
        m_qtWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    else if(frameInfo.m_style == ui::base::layoutManager::IFrameLayoutManager::MODAL)
    {
        m_qtWindow->setWindowModality(Qt::ApplicationModal);
    }

    int sizeX = (frameInfo.m_size.first > 0) ? frameInfo.m_size.first : m_qtWindow->size().width();
    int sizeY = (frameInfo.m_size.second > 0) ? frameInfo.m_size.second : m_qtWindow->size().height();

    int posX = frameInfo.m_position.first;
    int posY = frameInfo.m_position.second;
    QPoint pos(posX, posY);
    const QScreen* screen = QGuiApplication::screenAt(pos);
    if(screen == nullptr)
    {
        QRect frame_rect(0, 0, sizeX, sizeY);
        frame_rect.moveCenter(QGuiApplication::primaryScreen()->geometry().center());
        pos = frame_rect.topLeft();
    }

    m_qtWindow->setGeometry(pos.x(), pos.y(), sizeX, sizeY);

    this->setState(frameInfo.m_state);

    auto* qwidget = new QWidget(m_qtWindow);
    m_qtWindow->setCentralWidget(qwidget);

    QObject::connect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    ui::qt::container::QtContainer::sptr container = ui::qt::container::QtContainer::New();
    container->setQtContainer(qwidget);
    m_container = container;

    ui::qt::container::QtContainer::sptr frameContainer = ui::qt::container::QtContainer::New();
    frameContainer->setQtContainer(m_qtWindow);
    m_frame = frameContainer;
    m_frame->setVisible(frameInfo.m_visibility);
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
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

void FrameLayoutManager::onCloseFrame()
{
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::setState(FrameState state)
{
    // Updates the window state.
    switch(state)
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

ui::base::layoutManager::IFrameLayoutManager::FrameState FrameLayoutManager::getState()
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

} // namespace sight::ui::qt
