/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>
#include <QIcon>
#include <QLayout>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include "fwGuiQt/QtMainFrame.hpp"
#include "fwGuiQt/layoutManager/FrameLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IFrameLayoutManager,
        ::fwGui::FrameLayoutManager,
        ::fwGui::layoutManager::IFrameLayoutManager::RegistryKeyType,
        ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

FrameLayoutManager::FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

FrameLayoutManager::~FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

void FrameLayoutManager::createFrame()
{
    SLM_TRACE_FUNC();
    FrameInfo frameInfo = this->getFrameInfo();

    ::fwGuiQt::QtMainFrame *mainframe = new ::fwGuiQt::QtMainFrame();

    ::fwGuiQt::QtMainFrame::CloseCallback fct = ::boost::bind( &::fwGui::FrameLayoutManager::onCloseFrame, this);
    mainframe->setCloseCallback(fct);

    m_qtWindow = mainframe;
    m_qtWindow->setWindowTitle(QString::fromStdString(frameInfo.m_name));
    m_qtWindow->setMinimumSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second);

    if(!frameInfo.m_iconPath.empty())
    {
        QIcon icon(QString::fromStdString(frameInfo.m_iconPath.string()));

        OSLM_ASSERT("Sorry, unable to create an icon instance from " << frameInfo.m_iconPath.native_file_string(), !icon.isNull());

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

    m_qtWindow->move( frameInfo.m_position.first, frameInfo.m_position.second );
    m_qtWindow->resize( frameInfo.m_size.first, frameInfo.m_size.second );
    this->setState(frameInfo.m_state);

    m_qtWindow->show();

    m_qtWindow->setCentralWidget(new QWidget(m_qtWindow));

    QObject::connect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    ::fwGuiQt::container::QtContainer::NewSptr container;
    container->setQtContainer(m_qtWindow->centralWidget());
    m_container = container;

    ::fwGuiQt::container::QtContainer::NewSptr frameContainer;
    frameContainer->setQtContainer(m_qtWindow);
    m_frame = frameContainer;
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
{
    this->getRefFrameInfo().m_state = this->getState();
    this->getRefFrameInfo().m_size.first = m_qtWindow->size().width();
    this->getRefFrameInfo().m_size.second = m_qtWindow->size().height();
    this->getRefFrameInfo().m_position.first = m_qtWindow->pos().x();
    this->getRefFrameInfo().m_position.second = m_qtWindow->pos().y();
    this->writeConfig();

    QObject::disconnect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    if (m_qtWindow->layout())
    {
        m_qtWindow->layout()->deleteLater();
        m_qtWindow->setLayout(0);
    }
    m_container->destroyContainer();
    m_frame->clean();
    m_qtWindow->setParent(0);
    m_qtWindow->deleteLater();
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

} // namespace fwGui



