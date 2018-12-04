/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ioIGTL/SClientListener.hpp"

#include "ioIGTL/helper/preferences.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/Object.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <functional>
#include <string>

fwServicesRegisterMacro(::ioNetwork::INetworkListener, ::ioIGTL::SClientListener);

const ::fwServices::IService::KeyType s_OBJECTS_GROUP = "objects";

namespace ioIGTL
{

//-----------------------------------------------------------------------------

SClientListener::SClientListener() :
    m_tlInitialized(false)
{
}

//-----------------------------------------------------------------------------

SClientListener::~SClientListener()
{
}

//-----------------------------------------------------------------------------

void SClientListener::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    const ConfigType configInOut = config.get_child("inout");

    SLM_ASSERT("configured group must be '" + s_OBJECTS_GROUP + "'",
               configInOut.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP);

    const auto keyCfg = configInOut.equal_range("key");
    for(auto itCfg = keyCfg.first; itCfg != keyCfg.second; ++itCfg)
    {
        const ::fwServices::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName                   = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(deviceName);
        m_client.addAuthorizedDevice(deviceName);
    }
    m_client.setFilteringByDeviceName(true);

    const std::string serverInfo = config.get("server", "");
    if(!serverInfo.empty())
    {
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw ::fwTools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientListener::runClient()
{
    // 1. Connection
    try
    {
        const std::uint16_t port   = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
        const std::string hostname = ::ioIGTL::helper::getPreferenceKey<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
        m_sigConnected->asyncEmit();
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
        return;
    }

    // 2. Receive messages
    try
    {
        while (m_client.isConnected())
        {
            std::string deviceName;
            ::fwData::Object::sptr receiveObject = m_client.receiveObject(deviceName);
            if (receiveObject)
            {
                const auto& iter = std::find(m_deviceNames.begin(), m_deviceNames.end(), deviceName);

                if(iter != m_deviceNames.end())
                {
                    const auto indexReceiveObject = std::distance(m_deviceNames.begin(), iter);
                    ::fwData::Object::sptr obj =
                        this->getInOut< ::fwData::Object >(s_OBJECTS_GROUP, indexReceiveObject);

                    const bool isATimeline = obj->isA("::arData::MatrixTL") || obj->isA("::arData::FrameTL");
                    if(isATimeline)
                    {
                        this->manageTimeline(receiveObject, indexReceiveObject);
                    }
                    else
                    {
                        obj->shallowCopy(receiveObject);

                        ::fwData::Object::ModifiedSignalType::sptr sig;
                        sig = obj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                        sig->asyncEmit();
                    }
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
            ::fwGui::dialog::MessageDialog::showMessageDialog("Error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            OSLM_ERROR(ex.what());
        }
    }
}

//-----------------------------------------------------------------------------

void SClientListener::starting()
{
    m_clientFuture = std::async(std::launch::async, std::bind(&SClientListener::runClient, this));
}

//-----------------------------------------------------------------------------

void SClientListener::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }
        m_clientFuture.wait();
        m_tlInitialized = false;
        m_sigDisconnected->asyncEmit();
    }
    catch (::fwCore::Exception& ex)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Connection error", ex.what());
        SLM_ERROR(ex.what());
    }
}

//-----------------------------------------------------------------------------

void SClientListener::manageTimeline(::fwData::Object::sptr obj, size_t index)
{
    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    ::arData::MatrixTL::sptr matTL                 = this->getInOut< ::arData::MatrixTL>(s_OBJECTS_GROUP, index);
    ::arData::FrameTL::sptr frameTL                = this->getInOut< ::arData::FrameTL>(s_OBJECTS_GROUP, index);

    //MatrixTL
    if(matTL)
    {
        if(!m_tlInitialized)
        {
            matTL->setMaximumSize(10);
            matTL->initPoolSize(1);
            m_tlInitialized = true;
        }

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
    else if(frameTL)
    {
        ::fwData::Image::sptr im = ::fwData::Image::dynamicCast(obj);
        ::fwDataTools::helper::Image helper(im);
        if(!m_tlInitialized)
        {
            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->getSize()[0], im->getSize()[1], im->getType(), im->getNumberOfComponents());
            m_tlInitialized = true;
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
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL
