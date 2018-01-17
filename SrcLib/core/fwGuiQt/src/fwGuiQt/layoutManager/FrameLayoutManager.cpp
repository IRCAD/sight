/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/layoutManager/FrameLayoutManager.hpp"

#include "fwGuiQt/QtMainFrame.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <boost/lambda/lambda.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QLayout>
#include <QMainWindow>

fwGuiRegisterMacro( ::fwGui::FrameLayoutManager, ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

FrameLayoutManager::FrameLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

FrameLayoutManager::~FrameLayoutManager()
{
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::createFrame()
{
    SLM_TRACE_FUNC();
    FrameInfo frameInfo = this->getFrameInfo();

    ::fwGuiQt::QtMainFrame* mainframe = new ::fwGuiQt::QtMainFrame();
    m_qtWindow                        = mainframe;

    ::fwGuiQt::QtMainFrame::CloseCallback fct = std::bind( &::fwGui::FrameLayoutManager::onCloseFrame, this);
    mainframe->setCloseCallback(fct);

    m_qtWindow->setWindowTitle(QString::fromStdString(frameInfo.m_name));
    m_qtWindow->setMinimumSize(std::max(frameInfo.m_minSize.first, 0), std::max(frameInfo.m_minSize.second, 0));

    if(!frameInfo.m_iconPath.empty())
    {
        QIcon icon(QString::fromStdString(frameInfo.m_iconPath.string()));
        OSLM_ASSERT("Unable to create an icon instance from " << frameInfo.m_iconPath.string(), !icon.isNull());
        m_qtWindow->setWindowIcon(icon);
    }
    if(!qApp->activeWindow())
    {
        qApp->setActiveWindow(m_qtWindow);
    }
    if (frameInfo.m_style == ::fwGui::layoutManager::IFrameLayoutManager::STAY_ON_TOP)
    {
        m_qtWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    else if(frameInfo.m_style == ::fwGui::layoutManager::IFrameLayoutManager::MODAL)
    {
        m_qtWindow->setWindowModality(Qt::ApplicationModal);
    }

    int sizeX = (frameInfo.m_size.first > 0) ? frameInfo.m_size.first : m_qtWindow->size().width();
    int sizeY = (frameInfo.m_size.second > 0) ? frameInfo.m_size.second : m_qtWindow->size().height();

    int posX = frameInfo.m_position.first;
    int posY = frameInfo.m_position.second;
    QPoint pos(posX, posY);
    if(!this->isOnScreen(pos))
    {
        QRect frect(0, 0, sizeX, sizeY);
        frect.moveCenter(QDesktopWidget().screenGeometry().center());
        pos = frect.topLeft();
    }
    m_qtWindow->setGeometry(pos.x(), pos.y(), sizeX, sizeY);

    this->setState(frameInfo.m_state);

    QWidget* qwidget = new QWidget(m_qtWindow);
    m_qtWindow->setCentralWidget(qwidget);

    QObject::connect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    ::fwGuiQt::container::QtContainer::sptr container = ::fwGuiQt::container::QtContainer::New();
    container->setQtContainer(qwidget);
    m_container = container;

    ::fwGuiQt::container::QtContainer::sptr frameContainer = ::fwGuiQt::container::QtContainer::New();
    frameContainer->setQtContainer(m_qtWindow);
    m_frame = frameContainer;
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
    SLM_TRACE_FUNC();
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::setState( FrameState state )
{
    // Updates the window state.
    switch( state )
    {
        case ICONIZED:
            m_qtWindow->showMinimized();
            break;

        case MAXIMIZED:
            m_qtWindow->showMaximized();
            break;

        case FULL_SCREEN:
            m_qtWindow->showFullScreen();
            break;
        default:
            m_qtWindow->showNormal();
    }
}

//-----------------------------------------------------------------------------

::fwGui::layoutManager::IFrameLayoutManager::FrameState FrameLayoutManager::getState()
{
    FrameState state( UNKNOWN );

    if( m_qtWindow->isMinimized() )
    {
        state = ICONIZED;
    }
    else if( m_qtWindow->isMaximized() )
    {
        state = MAXIMIZED;
    }
    else if( m_qtWindow->isFullScreen() )
    {
        state = FULL_SCREEN;
    }
    return state;
}

//-----------------------------------------------------------------------------

bool FrameLayoutManager::isOnScreen(const QPoint& pos)
{
    bool isVisible = false;
    for(int i = 0; i < QDesktopWidget().screenCount() && !isVisible; ++i)
    {
        isVisible = QDesktopWidget().screenGeometry(i).contains(pos, false);
    }
    return isVisible;
}

//-----------------------------------------------------------------------------

} // namespace fwGui
