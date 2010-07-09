/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>
#include <QIcon>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

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


    m_qtFrame = new QMainWindow();
    m_qtFrame->setWindowTitle(QString::fromStdString(frameInfo.m_name));
    m_qtFrame->setMinimumSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second);

    if(!frameInfo.m_iconPath.empty())
    {
        QIcon icon(QString::fromStdString(frameInfo.m_name));

        OSLM_ASSERT("Sorry, unable to create an icon instance from " << frameInfo.m_iconPath.native_file_string(), !icon.isNull());

        m_qtFrame->setWindowIcon(icon);
    }
    qApp->setActiveWindow(m_qtFrame); // ?
    m_qtFrame->show();

    ::fwGuiQt::container::QtContainer::NewSptr frameContainer;
    frameContainer->setQtContainer(m_qtFrame);
    m_frame = frameContainer;
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
{
    m_qtFrame->hide();
    m_qtFrame->setParent(0);
    m_qtFrame->deleteLater();
}

//-----------------------------------------------------------------------------

//void FrameLayoutManager::onCloseFrame(wxCloseEvent& event)
//{
    //SLM_TRACE_FUNC();
    ////::fwServices::OSR::uninitializeRootObject();
//}

//-----------------------------------------------------------------------------

} // namespace fwGui



