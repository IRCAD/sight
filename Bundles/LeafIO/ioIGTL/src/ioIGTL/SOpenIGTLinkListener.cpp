/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SOpenIGTLinkListener.hpp"

#include "ioIGTL/helper/preferences.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/Object.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <string>

fwServicesRegisterMacro(::ioNetwork::INetworkListener, ::ioIGTL::SOpenIGTLinkListener);

namespace ioIGTL
{

static const std::string s_TARGET_KEY = "target";

//-----------------------------------------------------------------------------

SOpenIGTLinkListener::SOpenIGTLinkListener() :
    m_timelineType(NONE),
    m_frameTLInitialized(false)
{
}

//-----------------------------------------------------------------------------

SOpenIGTLinkListener::~SOpenIGTLinkListener()
{
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::configuring()
{
    SLM_ASSERT("Configuration not found", m_configuration != NULL);
    if (m_configuration->findConfigurationElement("server"))
    {
        const std::string serverInfo = m_configuration->findConfigurationElement("server")->getValue();
        SLM_INFO("OpenIGTLinkListener::configure server: " + serverInfo);
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw ::fwTools::Failed("Server element not found");
    }

    std::vector < ::fwRuntime::ConfigurationElement::sptr > deviceNames = m_configuration->find("deviceName");
    if(!deviceNames.empty())
    {
        for(auto dn : deviceNames)
        {
            m_client.addAuthorizedDevice(dn->getValue());
        }
        m_client.setFilteringByDeviceName(true);
    }
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::runClient()
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object>(s_TARGET_KEY);

    if(m_timelineType == SOpenIGTLinkListener::MATRIX)
    {
        obj = ::fwData::TransformationMatrix3D::New();
    }
    else if(m_timelineType == SOpenIGTLinkListener::FRAME)
    {
        obj = ::fwData::Image::New();
    }

    // 1. Connection
    try
    {
        const std::uint16_t port   = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
        const std::string hostname = ::ioIGTL::helper::getPreferenceKey<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.showMessageDialog("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
        return;
    }

    m_sigClientConnected->asyncEmit();

    // 2. Receive messages
    try
    {
        while (m_client.isConnected())
        {
            if (m_client.receiveObject(obj))
            {
                if(m_timelineType != SOpenIGTLinkListener::NONE)
                {
                    this->manageTimeline(obj);
                }
                else
                {
                    this->notifyObjectUpdated();
                }
            }
        }
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.showMessageDialog("Error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            OSLM_ERROR(ex.what());
        }
    }
    m_sigClientDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::starting()
{
    ::fwData::Object::sptr obj  = this->getInOut< ::fwData::Object>(s_TARGET_KEY);
    ::arData::TimeLine::sptr tl = ::arData::TimeLine::dynamicCast(obj);

    if(tl)
    {
        if(obj->isA("::arData::MatrixTL"))
        {
            m_timelineType                 = SOpenIGTLinkListener::MATRIX;
            ::arData::MatrixTL::sptr matTL = this->getInOut< ::arData::MatrixTL>(s_TARGET_KEY);
            matTL->setMaximumSize(10);
            matTL->initPoolSize(1);

        }
        else if(obj->isA("::arData::FrameTL"))
        {
            m_timelineType = SOpenIGTLinkListener::FRAME;
        }
        else
        {
            SLM_WARN("This type of timeline is not managed !");
        }
    }

    m_clientFuture = std::async(std::launch::async, std::bind(&SOpenIGTLinkListener::runClient, this));
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::stopping()
{
    m_client.disconnect();
    m_clientFuture.wait();
    m_frameTLInitialized = false;
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::manageTimeline(::fwData::Object::sptr obj)
{
    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    //MatrixTL
    if(m_timelineType == SOpenIGTLinkListener::MATRIX)
    {
        ::arData::MatrixTL::sptr matTL = this->getInOut< ::arData::MatrixTL>(s_TARGET_KEY);

        SPTR(::arData::MatrixTL::BufferType) matrixBuf;
        matrixBuf = matTL->createBuffer(timestamp);
        ::fwData::TransformationMatrix3D::TMCoefArray values;
        ::fwData::TransformationMatrix3D::sptr t = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        values                                   = t->getCoefficients();
        float matrixValues[16];

        for(unsigned int i = 0; i < 16; ++i)
        {
            matrixValues[i] = static_cast< float >(values[i]);
        }

        matrixBuf->setElement(matrixValues, 0);
        matTL->pushObject(matrixBuf);
        ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = matTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);
    }
    //FrameTL
    else if(m_timelineType == SOpenIGTLinkListener::FRAME)
    {
        ::fwData::Image::sptr im = ::fwData::Image::dynamicCast(obj);
        ::fwDataTools::helper::Image helper(im);
        ::arData::FrameTL::sptr frameTL = this->getInOut< ::arData::FrameTL>(s_TARGET_KEY);
        if(!m_frameTLInitialized)
        {
            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->getSize()[0], im->getSize()[1], im->getType(), im->getNumberOfComponents());
            m_frameTLInitialized = true;
        }

        SPTR(::arData::FrameTL::BufferType) buffer = frameTL->createBuffer(timestamp);

        std::uint8_t* destBuffer = reinterpret_cast< std::uint8_t* >( buffer->addElement(0) );

        std::uint8_t* srcBuffer = static_cast< std::uint8_t* >(helper.getBuffer());

        std::copy(srcBuffer, srcBuffer+im->getSizeInBytes(), destBuffer);

        frameTL->pushObject(buffer);

        ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = frameTL->signal< ::arData::TimeLine::ObjectPushedSignalType >
                  (::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);
    }
    else
    {
        //TODO: otherTL
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL

