/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SOpenIGTLinkListener.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/Object.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTools/Failed.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <string>

fwServicesRegisterMacro (::ioNetwork::INetworkListener, ::ioIGTL::SOpenIGTLinkListener, ::fwData::Object);

namespace ioIGTL
{

//-----------------------------------------------------------------------------

SOpenIGTLinkListener::SOpenIGTLinkListener() :
    m_hostname("127.0.0.1"),
    m_port(4242),
    m_timelineType(NONE),
    m_frameTLInitialized(false)
{
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
    std::string hostnameStr;
    std::string::size_type splitPosition;

    SLM_ASSERT("Configuration not found", m_configuration != NULL);
    if (m_configuration->findConfigurationElement ("server"))
    {
        serverInfo = m_configuration->findConfigurationElement ("server")->getValue();
        SLM_INFO ("OpenIGTLinkListener::configure server: " + serverInfo);
        splitPosition = serverInfo.find (':');
        SLM_ASSERT ("Server info not formatted correctly", splitPosition != std::string::npos);

        hostnameStr = serverInfo.substr (0, splitPosition);
        portStr     = serverInfo.substr (splitPosition + 1, serverInfo.size());

        m_hostnameKey = this->getPreferenceKey(hostnameStr);
        m_portKey     = this->getPreferenceKey(portStr);

        if(m_hostnameKey.empty())
        {
            m_hostname = hostnameStr;
        }
        if(m_portKey.empty())
        {
            m_port = ::boost::lexical_cast< std::uint16_t > (portStr);
        }
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
    catch (::fwCore::Exception& ex)
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

void SOpenIGTLinkListener::setHost(std::string const& hostname, boost::uint16_t const port) throw (::fwTools::Failed)
{
    m_hostname = hostname;
    m_port     = port;
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkListener::starting() throw (::fwTools::Failed)
{
    if(!m_hostnameKey.empty())
    {
        std::string hostname = ::fwPreferences::getPreference(m_hostnameKey);
        if(!hostname.empty())
        {
            m_hostname = hostname;
        }
    }
    if(!m_portKey.empty())
    {
        std::string port = ::fwPreferences::getPreference(m_portKey);
        if(!port.empty())
        {
            m_port = ::boost::lexical_cast< std::uint16_t > (port);
        }
    }

    ::fwData::Object::sptr obj  = this->getObject();
    ::arData::TimeLine::sptr tl = ::arData::TimeLine::dynamicCast(obj);

    if(tl)
    {
        if(obj->isA("::arData::MatrixTL"))
        {
            m_timelineType                 = SOpenIGTLinkListener::MATRIX;
            ::arData::MatrixTL::sptr matTL = this->getObject< ::arData::MatrixTL >();
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

    std::function<void() > task = std::bind (&SOpenIGTLinkListener::runClient, this);
    m_clientWorker = ::fwThread::Worker::New();

    m_clientWorker->post(task);
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
        ::arData::MatrixTL::sptr matTL = this->getObject< ::arData::MatrixTL >();

        SPTR(::arData::MatrixTL::BufferType) matrixBuf;
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
        ::arData::FrameTL::sptr frameTL = this->getObject< ::arData::FrameTL >();
        if(!m_frameTLInitialized)
        {
            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->getSize()[0],im->getSize()[1],im->getType(),im->getNumberOfComponents());
            m_frameTLInitialized = true;
        }


        SPTR(::arData::FrameTL::BufferType) buffer = frameTL->createBuffer(timestamp);

        ::boost::uint8_t* destBuffer = reinterpret_cast< ::boost::uint8_t* >( buffer->addElement(0) );

        ::boost::uint8_t* srcBuffer = static_cast< ::boost::uint8_t* >(helper.getBuffer());

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

std::string SOpenIGTLinkListener::getPreferenceKey(const std::string& key) const
{
    std::string keyResult;
    size_t first = key.find('%');
    size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = key.substr(1, key.size() - 2);
    }
    return keyResult;
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL


