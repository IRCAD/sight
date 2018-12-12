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

#ifndef __REALSENSETOOLS_DEVICEMANAGER_HPP__
#define __REALSENSETOOLS_DEVICEMANAGER_HPP__

#include "realsenseTools/config.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

#include <librealsense/rs.hpp>

#include <pcl/pcl_exports.h>

#include <vector>

namespace realsenseTools
{

class RsGrabber;
class Device;

/**
 * @brief Class to handle realsense device.
 * Code is inspired from a PCL pull-request (ttps://github.com/PointCloudLibrary/pcl/pull/1633)
 */
class REALSENSETOOLS_CLASS_API DeviceManager : boost::noncopyable
{

public:

    typedef ::boost::shared_ptr<DeviceManager> Ptr;

    REALSENSETOOLS_API ~DeviceManager ();

    //------------------------------------------------------------------------------

    static Ptr& getInstance ()
    {
        static Ptr instance;
        if (!instance)
        {
            ::boost::mutex::scoped_lock lock(m_mutex);
            if (!instance)
            {
                instance.reset(new DeviceManager);
            }
        }
        return (instance);
    }

    //------------------------------------------------------------------------------

    inline size_t getNumDevices ()
    {
        return (m_deviceList.size());
    }

    REALSENSETOOLS_API ::boost::shared_ptr<Device> captureDevice ();

    REALSENSETOOLS_API ::boost::shared_ptr<Device> captureDevice (size_t index);

    REALSENSETOOLS_API ::boost::shared_ptr<Device> captureDevice (const std::string& sn);

private:

    struct DeviceInfo
    {
        int index;
        std::string serial;
        bool isCaptured;
    };

    /** If the device is already captured returns a pointer. */
    ::boost::shared_ptr<Device>
    capture (DeviceInfo& device_info);

    DeviceManager ();

    ::rs::context m_context;

    /** This function discovers devices that are capable of streaming
     * depth data. */
    void
    populateDeviceList ();

    std::vector<DeviceInfo> m_deviceList;

    static ::boost::mutex m_mutex;
};
/**
 * @brief The Device class
 */
class REALSENSETOOLS_CLASS_API Device : ::boost::noncopyable
{
public:

    typedef ::boost::shared_ptr<Device> Ptr;

    //------------------------------------------------------------------------------

    inline const std::string& getSerialNumber ()
    {
        return (m_deviceId);
    }

    //------------------------------------------------------------------------------

    inline rs::device* getDevice ()
    {
        return (m_device);
    }

    /** Reset the state of given device by releasing and capturing again. */
    static void reset (Device::Ptr& device)
    {
        std::string id = device->getSerialNumber();
        device.reset();
        device = DeviceManager::getInstance()->captureDevice(id);
    }

private:

    friend class DeviceManager;

    std::string m_deviceId;
    ::rs::device* m_device;

    Device (const std::string& id) :
        m_deviceId(id)
    {
    }

};
} // namespace realsenseTools

#endif /* __REALSENSETOOLS_DEVICEMANAGER_HPP__ */
