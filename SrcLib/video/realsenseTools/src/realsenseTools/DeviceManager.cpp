/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2015-, Open Perception, Inc.
 *  Copyright (c) 2016, Intel Corporation
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "realsenseTools/DeviceManager.hpp"

#include <fwCore/spyLog.hpp>

::boost::mutex realsenseTools::DeviceManager::m_mutex;

namespace realsenseTools
{

//-----------------------------------------------------------------------------

DeviceManager::DeviceManager ()
{
    populateDeviceList ();
}

//-----------------------------------------------------------------------------

DeviceManager::~DeviceManager ()
{
}

//-----------------------------------------------------------------------------

Device::Ptr
DeviceManager::captureDevice ()
{
    ::boost::mutex::scoped_lock lock (m_mutex);

    if (m_deviceList.size () == 0)
    {
        SLM_ERROR("no connected devices");
        return (Device::Ptr ());
    }
    else
    {
        for (size_t i = 0; i < m_deviceList.size (); ++i)
        {
            if (!m_deviceList[i].isCaptured)
            {
                return (capture (m_deviceList[i]));
            }
        }
        SLM_ERROR("all connected devices are captured by other grabbers");
        return (Device::Ptr ());
    }
}

//-----------------------------------------------------------------------------

Device::Ptr
DeviceManager::captureDevice (size_t index)
{
    ::boost::mutex::scoped_lock lock (m_mutex);
    if (index >= m_deviceList.size ())
    {
        OSLM_ERROR("device with index '" << index +1 << "' is not connected.");
        return (Device::Ptr ());
    }
    if (m_deviceList[index].isCaptured)
    {
        OSLM_ERROR("device with index '" << index +1 << "' is captured by another grabber.");
        return (Device::Ptr ());
    }
    return (capture (m_deviceList[index]));
}

//-----------------------------------------------------------------------------

Device::Ptr
DeviceManager::captureDevice (const std::string& sn)
{
    ::boost::mutex::scoped_lock lock (m_mutex);
    for (size_t i = 0; i < m_deviceList.size (); ++i)
    {
        if (m_deviceList[i].serial == sn)
        {
            if (m_deviceList[i].isCaptured)
            {
                SLM_ERROR("device with serial number is captured by another grabber" + sn);
                return (Device::Ptr ());
            }
            return (capture (m_deviceList[i]));
        }
    }
    SLM_ERROR("device with serial number is not connected" + sn);
    return (Device::Ptr ());
}

//-----------------------------------------------------------------------------

void
DeviceManager::populateDeviceList ()
{
    m_deviceList.clear ();
    for (int i = 0; i < m_context.get_device_count (); i++)
    {
        m_deviceList.push_back (DeviceInfo ());
        m_deviceList.back ().serial     = m_context.get_device (i)->get_serial ();
        m_deviceList.back ().index      = i;
        m_deviceList.back ().isCaptured = false;
    }
}

//-----------------------------------------------------------------------------

Device::Ptr
DeviceManager::capture (DeviceInfo& device_info)
{
    // This is called from public captureDevice() functions and should already be
    // under scoped lock
    Device::Ptr device (new Device (device_info.serial));
    device->m_device       = m_context.get_device (device_info.index);
    device_info.isCaptured = true;
    return device;
}

//-----------------------------------------------------------------------------

}//namespace realsense
