/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/STDataListener.hpp"

#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTools/Failed.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <string>

fwServicesRegisterMacro (::ioNetwork::INetworkListener, ::ioIGTL::STDataListener, ::arData::MatrixTL);

namespace ioIGTL
{

//-----------------------------------------------------------------------------

STDataListener::STDataListener() :
    m_hostname("127.0.0.1"),
    m_port(4242)
{
}

//-----------------------------------------------------------------------------

STDataListener::~STDataListener()
{
}

//-----------------------------------------------------------------------------

void STDataListener::configuring() throw (::fwTools::Failed)
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

    ::fwRuntime::ConfigurationElement::sptr tdata = m_configuration->findConfigurationElement("TData");

    if(tdata)
    {
        std::vector < ::fwRuntime::ConfigurationElement::sptr > matrices = tdata->find("matrix");

        for(const auto& m : matrices)
        {
            const unsigned long index = ::boost::lexical_cast< unsigned long >(m->getAttributeValue("index"));
            m_matrixNameIndex[m->getAttributeValue("name")] = index;
        }
    }
    else
    {
        SLM_WARN("Element 'TData' is missing, the listener will do nothing !");
    }
}

//-----------------------------------------------------------------------------

void STDataListener::runClient() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;

    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

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
            if (m_client.receiveObject(composite))
            {
                this->manageTimeline(composite);
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
            SLM_ERROR(ex.what());
        }
    }
    m_sigClientDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void STDataListener::setHost(std::string const& hostname, std::uint16_t const port) throw (::fwTools::Failed)
{
    m_hostname = hostname;
    m_port     = port;
}

//-----------------------------------------------------------------------------

void STDataListener::starting() throw (::fwTools::Failed)
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

    ::arData::MatrixTL::sptr matTL = this->getObject< ::arData::MatrixTL >();
    matTL->setMaximumSize(10);
    matTL->initPoolSize(static_cast< unsigned int >(m_matrixNameIndex.size()));

    std::function<void() > task = std::bind (&STDataListener::runClient, this);
    m_clientWorker = ::fwThread::Worker::New();

    m_clientWorker->post(task);
}

//-----------------------------------------------------------------------------

void STDataListener::stopping() throw (::fwTools::Failed)
{
    m_client.disconnect();
    m_clientWorker->stop();
}

//-----------------------------------------------------------------------------

void STDataListener::manageTimeline(const ::fwData::Composite::sptr& obj)
{
    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    ::arData::MatrixTL::sptr matTL                 = this->getObject< ::arData::MatrixTL >();
    SPTR(::arData::MatrixTL::BufferType) matrixBuf;
    matrixBuf = matTL->createBuffer(timestamp);

    for(const ::fwData::Composite::ContainerType::value_type& elt : obj->getContainer())
    {
        ::fwData::TransformationMatrix3D::csptr transfoMatrix =
            ::fwData::TransformationMatrix3D::dynamicConstCast(elt.second);

        MatrixNameIndexType::const_iterator it = m_matrixNameIndex.find(elt.first);

        if(transfoMatrix && it != m_matrixNameIndex.end())
        {
            unsigned long index = it->second;

            ::fwData::TransformationMatrix3D::TMCoefArray values;
            values = transfoMatrix->getCoefficients();
            float matrixValues[16];
            bool isZero = true;
            for(unsigned int i = 0; i< 16; ++i)
            {
                matrixValues[i] = static_cast< float >(values[i]);
                //Test if matrix contains only '0' except last value (always '1)
                isZero &= i<15 ? (matrixValues[i] == 0.f) : true;
            }
            //don't push the matrix if it contains only '0'
            if(!isZero)
            {
                matrixBuf->setElement(matrixValues, static_cast< unsigned int>(index));
            }
        }
    }
    matTL->pushObject(matrixBuf);

    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = matTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
        ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

std::string STDataListener::getPreferenceKey(const std::string& key) const
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


