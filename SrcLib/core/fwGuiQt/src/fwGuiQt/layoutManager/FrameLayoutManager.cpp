/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>
#include <QIcon>

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

    m_qtWindow->show();

    m_qtWindow->setCentralWidget(new QWidget(m_qtWindow));

    QObject::connect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));

    ::fwGuiQt::container::QtContainer::NewSptr frameContainer;
    //frameContainer->setQtContainer(m_qtWindow);
    frameContainer->setQtContainer(m_qtWindow->centralWidget());
    m_frame = frameContainer;
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
{
    QObject::disconnect(m_qtWindow, SIGNAL(destroyed(QObject*)), this, SLOT(onCloseFrame()));
    m_qtWindow->setParent(0);
    m_qtWindow->deleteLater();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::onCloseFrame()
{
    SLM_TRACE_FUNC();
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



