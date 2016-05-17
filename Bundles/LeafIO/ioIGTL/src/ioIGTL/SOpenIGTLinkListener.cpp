/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SOpenIGTLinkListener.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwComEd/helper/Image.hpp>

#include <fwData/Object.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <extData/FrameTL.hpp>
#include <extData/MatrixTL.hpp>

#include <fwTools/Failed.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/macros.hpp>

#include <string>
#include <functional>

fwServicesRegisterMacro (::ioNetwork::INetworkListener, ::ioIGTL::SOpenIGTLinkListener, ::fwData::Object);


namespace ioIGTL
{
const ::fwCom::Slots::SlotKeyType SOpenIGTLinkListener::s_UPDATE_CONFIGURATION_SLOT = "updateConfiguration";

//-----------------------------------------------------------------------------

SOpenIGTLinkListener::SOpenIGTLinkListener() :
    ioNetwork::INetworkListener(),
    m_timelineType(NONE),
    m_frameTLInitialized(false)
{
    m_updateConfigurationSlot = ::fwCom::newSlot (&SOpenIGTLinkListener::updateConfiguration, this);
    ::fwCom::HasSlots::m_slots (SOpenIGTLinkListener::s_UPDATE_CONFIGURATION_SLOT, m_updateConfigurationSlot);

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SOpenIGTLinkListener::~SOpenIGTLinkListener()
{
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::configuring() throw (::fwTools::Failed)
{
    std::string serverInfo;
    std::string portStr;
    std::string::size_type splitPosition;

    SLM_ASSERT("Configuration not found", m_configuration != NULL);
    if (m_configuration->findConfigurationElement ("server"))
    {
        serverInfo = m_configuration->findConfigurationElement ("server")->getValue();
        SLM_INFO ("OpenIGTLinkListener::configure server: " + serverInfo);
        splitPosition = serverInfo.find (':');
        SLM_ASSERT ("Server info not formatted correctly", splitPosition != std::string::npos);
        m_hostname = serverInfo.substr (0, splitPosition);
        portStr    = serverInfo.substr (splitPosition + 1, serverInfo.size());
        m_port     = ::boost::lexical_cast< ::boost::uint16_t > (portStr);
    }
    else
    {
        throw ::fwTools::Failed ("Server element not found");
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

void SOpenIGTLinkListener::runClient() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Object::sptr obj = this->getObject();

    if(m_timelineType== SOpenIGTLinkListener::MATRIX)
    {
        obj = ::fwData::TransformationMatrix3D::New();
    }
    else if(m_timelineType== SOpenIGTLinkListener::FRAME)
    {
        obj = ::fwData::Image::New();
    }

    // 1. Connection
    try
    {
        m_client.connect (m_hostname, m_port);
    }
    catch (::fwCore::Exception &ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.showMessageDialog ("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            OSLM_ERROR(ex.what());
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
    catch (::fwCore::Exception &ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.showMessageDialog ("Error", ex.what());
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

void SOpenIGTLinkListener::setHost(std::string const &hostname, boost::uint16_t const port) throw (::fwTools::Failed)
{
    m_hostname = hostname;
    m_port     = port;
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::starting() throw (::fwTools::Failed)
{
    ::fwData::Object::sptr obj   = this->getObject();
    ::extData::TimeLine::sptr tl = ::extData::TimeLine::dynamicCast(obj);

    if(tl)
    {
        if(obj->isA("::extData::MatrixTL"))
        {
            m_timelineType                  = SOpenIGTLinkListener::MATRIX;
            ::extData::MatrixTL::sptr matTL = this->getObject< ::extData::MatrixTL >();
            matTL->setMaximumSize(10);
            matTL->initPoolSize(1);

        }
        else if(obj->isA("::extData::FrameTL"))
        {
            m_timelineType = SOpenIGTLinkListener::FRAME;
        }
        else
        {
            OSLM_WARN("This type of timeline is not managed !");
        }
    }

    std::function<void() > task = std::bind (&SOpenIGTLinkListener::runClient, this);
    m_clientWorker = ::fwThread::Worker::New();

    m_clientWorker->post(task);
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::updateConfiguration(std::string const hostname, ::boost::uint16_t port)
{
    m_hostname = hostname;
    m_port     = port;
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::stopping() throw (::fwTools::Failed)
{
    m_client.disconnect();
    m_clientWorker->stop();
    m_frameTLInitialized = false;
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::manageTimeline(::fwData::Object::sptr obj)
{

    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    //MatrixTL
    if(m_timelineType == SOpenIGTLinkListener::MATRIX)
    {
        ::extData::MatrixTL::sptr matTL = this->getObject< ::extData::MatrixTL >();

        SPTR(::extData::MatrixTL::BufferType) matrixBuf;
        matrixBuf = matTL->createBuffer(timestamp);
        ::fwData::TransformationMatrix3D::TMCoefArray values;
        ::fwData::TransformationMatrix3D::sptr t = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        values                                   = t->getCoefficients();
        float matrixValues[16];

        for(unsigned int i = 0; i< 16; ++i)
        {
            matrixValues[i] = static_cast< float >(values[i]);
        }

        matrixBuf->setElement(matrixValues, 0);
        matTL->pushObject(matrixBuf);
        ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = matTL->signal< ::extData::TimeLine::ObjectPushedSignalType >(
            ::extData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);
    }
    //FrameTL
    else if(m_timelineType == SOpenIGTLinkListener::FRAME)
    {

        ::fwData::Image::sptr im = ::fwData::Image::dynamicCast(obj);
        ::fwComEd::helper::Image helper(im);
        ::extData::FrameTL::sptr frameTL = this->getObject< ::extData::FrameTL >();
        if(!m_frameTLInitialized)
        {
            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->getSize()[0],im->getSize()[1],im->getType(),im->getNumberOfComponents());
            m_frameTLInitialized = true;
        }


        SPTR(::extData::FrameTL::BufferType) buffer = frameTL->createBuffer(timestamp);

        ::boost::uint8_t* destBuffer = reinterpret_cast< ::boost::uint8_t* >( buffer->addElement(0) );

        ::boost::uint8_t* srcBuffer = static_cast< ::boost::uint8_t* >(helper.getBuffer());

        std::copy(srcBuffer, srcBuffer+im->getSizeInBytes(), destBuffer);

        frameTL->pushObject(buffer);

        ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = frameTL->signal< ::extData::TimeLine::ObjectPushedSignalType >
                  (::extData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);

    }
    else
    {
        //TODO: otherTL
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL


