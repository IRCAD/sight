/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SGrabberProxy.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace videoTools
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SGrabberProxy::s_RECONFIGURE_SLOT = "reconfigure";

fwServicesRegisterMacro( ::arServices::IGrabber, ::videoTools::SGrabberProxy, ::arData::FrameTL);

//-----------------------------------------------------------------------------

SGrabberProxy::SGrabberProxy() noexcept
{
    newSlot( s_RECONFIGURE_SLOT, &SGrabberProxy::reconfigure, this );
}

//-----------------------------------------------------------------------------

SGrabberProxy::~SGrabberProxy() noexcept
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::starting()
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::stopping()
{
    if(m_service != nullptr)
    {
        m_service->stopCamera();
        this->unregisterService(m_service);
        m_service.reset();

        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_KEY);
        frameTL->clearTimeline();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::configuring()
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::updating()
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::startCamera()
{
    if(m_service == nullptr)
    {
        if(m_grabberImpl.empty())
        {
            auto serviceFactory = ::fwServices::registry::ServiceFactory::getDefault();
            auto servicesImpl   = serviceFactory->getImplementationIdFromObjectAndType("::arData::FrameTL",
                                                                                       "::arServices::IGrabber");
            std::vector< std::string > availableExtensionsSelector;

            for(const auto& srv : servicesImpl)
            {
                if(srv != "::videoTools::SGrabberProxy")
                {
                    availableExtensionsSelector.push_back(srv);
                }
            }

            if(availableExtensionsSelector.size() > 1)
            {
                ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

                selector->setTitle("Choose a video grabber");

                selector->setSelections(availableExtensionsSelector);

                m_grabberImpl = selector->show();
            }
            else if( availableExtensionsSelector.size() == 1)
            {
                m_grabberImpl = availableExtensionsSelector.front();
            }
            else
            {
                const std::string msg = "No video grabber implementation found\n";
                ::fwGui::dialog::MessageDialog::showMessageDialog("Error", msg);
                return;
            }

            if(m_grabberImpl.empty())
            {
                const std::string msg = "No video grabber chosen, aborting...\n";
                ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", msg);
                return;
            }
        }

        m_service = this->registerService< ::arServices::IGrabber>(m_grabberImpl);

        auto camera = this->getInput< ::arData::Camera >(s_CAMERA_KEY);
        m_service->registerInput(camera, s_CAMERA_KEY);

        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_KEY);
        m_service->registerInOut(frameTL, s_FRAMETL_KEY);
        m_service->setWorker(m_associatedWorker);

        m_service->start();
    }

    m_service->startCamera();
}

//-----------------------------------------------------------------------------

void SGrabberProxy::pauseCamera()
{
    if(m_service != nullptr)
    {
        m_service->pauseCamera();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::stopCamera()
{
    if(m_service != nullptr)
    {
        m_service->stopCamera();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::toggleLoopMode()
{
    if(m_service != nullptr)
    {
        m_service->toggleLoopMode();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::setPosition(std::int64_t position)
{
    if(m_service != nullptr)
    {
        m_service->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::reconfigure()
{
    if(m_service != nullptr)
    {
        m_service->stopCamera();
        this->unregisterService(m_service);
        m_service.reset();

        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_KEY);
        frameTL->clearTimeline();
    }
    m_grabberImpl = "";
}

//-----------------------------------------------------------------------------

} // namespace videoTools

