/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/STDataListener.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <extData/MatrixTL.hpp>

#include <fwTools/Failed.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/Base.hpp>

#include <string>
#include <functional>

fwServicesRegisterMacro (::ioNetwork::INetworkListener, ::ioIGTL::STDataListener, ::extData::MatrixTL);


namespace ioIGTL
{
const ::fwCom::Slots::SlotKeyType STDataListener::s_UPDATE_CONFIGURATION_SLOT = "updateConfiguration";

//-----------------------------------------------------------------------------

STDataListener::STDataListener() :
    ioNetwork::INetworkListener()
{
    m_updateConfigurationSlot = ::fwCom::newSlot (&STDataListener::updateConfiguration, this);
    ::fwCom::HasSlots::m_slots (STDataListener::s_UPDATE_CONFIGURATION_SLOT, m_updateConfigurationSlot);

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
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
        m_port     = ::boost::lexical_cast< std::uint16_t > (portStr);
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
    ::fwData::Object::sptr obj          = composite;
    try
    {
        m_client.connect (m_hostname, m_port);
        m_sigClientConnected->asyncEmit();
        while (m_client.isConnected())
        {
            if (m_client.receiveObject(obj))
            {
                this->manageTimeline(composite);
            }
        }
        m_sigClientDisconnected->asyncEmit();
    }
    catch (::fwCore::Exception &ex)
    {
        msgDialog.showMessageDialog ("Error", ex.what());
        this->slot(s_STOP_SLOT)->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void STDataListener::setHost(std::string const &hostname, std::uint16_t const port) throw (::fwTools::Failed)
{
    m_hostname = hostname;
    m_port     = port;
}

//-----------------------------------------------------------------------------

void STDataListener::starting() throw (::fwTools::Failed)
{
    ::extData::MatrixTL::sptr matTL = this->getObject< ::extData::MatrixTL >();
    matTL->setMaximumSize(10);
    matTL->initPoolSize(static_cast< unsigned int >(m_matrixNameIndex.size()));

    std::function<void() > task = std::bind (&STDataListener::runClient, this);
    m_clientWorker = ::fwThread::Worker::New();

    m_clientWorker->post(task);
}

//-----------------------------------------------------------------------------

void STDataListener::updateConfiguration(const std::string hostname, std::uint16_t port)
{
    m_hostname = hostname;
    m_port     = port;
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
    ::extData::MatrixTL::sptr matTL                = this->getObject< ::extData::MatrixTL >();
    SPTR(::extData::MatrixTL::BufferType) matrixBuf;
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

    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = matTL->signal< ::extData::TimeLine::ObjectPushedSignalType >(
        ::extData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL


