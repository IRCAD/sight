/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "videoOrbbec/SScanBase.hpp"

#include <arData/CameraSeries.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwTools/Os.hpp>

#include <cmath>
#include <regex>
#include <string>

namespace videoOrbbec
{

//------------------------------------------------------------------------------

SScanBase::SScanBase() noexcept :
    m_oniDevice(nullptr)
{
}

//------------------------------------------------------------------------------

SScanBase::~SScanBase() noexcept
{
}

//------------------------------------------------------------------------------

void SScanBase::configuring()
{
}

//------------------------------------------------------------------------------

void SScanBase::stopping()
{
    // ensure that openNI device is reset.
    m_oniDevice = nullptr;
    this->stopCamera();
}

//------------------------------------------------------------------------------

void SScanBase::updating()
{
}

//------------------------------------------------------------------------------

void SScanBase::pauseCamera()
{
}

//------------------------------------------------------------------------------

void SScanBase::toggleLoopMode()
{
}

//------------------------------------------------------------------------------

void SScanBase::setPosition(int64_t position)
{
}

//------------------------------------------------------------------------------

void SScanBase::detectCameraOpenni()
{
    // Get Camera data
    auto cameraInput = this->getInput< ::fwData::Object >(s_CAMERA_INPUT);
    auto camera      = ::arData::Camera::dynamicConstCast(cameraInput);

    std::string camId;
    std::string desc;
    int index = -1;

    // Check if Camera or CameraSeries
    if(camera)
    {
        camId = camera->getCameraID();
        desc  = camera->getDescription();
    }
    else
    {
        auto cameraSeries = ::arData::CameraSeries::dynamicConstCast(cameraInput);
        if(cameraSeries)
        {
            // Assume same source on all cameras
            camId = cameraSeries->getCamera(0)->getCameraID();
            desc  = camera->getDescription();
        }
    }

    // Here we cannot use directly the index, because openNI will give us only Depth Cameras,
    // since index is assigned on all RGB Camera connected it may be wrong.
    // When multiple Astra are connected we append #Number at the end of description.
    // (see ::videoQt::editor::CameraDeviceDlg)

    // No '#' or '#1' leads to index '0', '#2' leads to index '1', ...
    const size_t pos = desc.rfind("#");
    index = pos != std::string::npos ? std::atoi(desc.substr(pos + 1).c_str()) -1 : 0;

    ::openni::Array< ::openni::DeviceInfo> devices;
    ::openni::OpenNI::enumerateDevices(&devices);

    OpenniDevicePtr device = std::unique_ptr< ::openni::Device >(new ::openni::Device());
    std::string astraDeviceUri;

    // We need to sort OpenNI devices according to their URI first.
    // Since OpenNI Array doesn't contain iterator we need to copy values to std container.

    std::vector< ::openni::DeviceInfo > deviceVec;
    deviceVec.resize( static_cast<size_t>(devices.getSize()));
    for(std::uint8_t i = 0; i < devices.getSize(); ++i)
    {
        deviceVec[i] = devices[i];
    }

// Sort according to nurmeric & alphabetical order.(assume that this will give us the same order as Qt).
// Except on Windows.
#ifndef WIN32
    std::sort(deviceVec.begin(), deviceVec.end(), []( ::openni::DeviceInfo _a, ::openni::DeviceInfo _b)
        {
            return std::string(_a.getUri()) < std::string(_b.getUri());
        });
#endif
    bool foundAstra = false;
    int astraNum    = 0;

    for(auto const& dev: deviceVec)
    {
        if(!(std::strcmp("Orbbec", dev.getVendor()) || std::strcmp("Astra", dev.getName())
             || dev.getUsbVendorId() != 11205))
        {

            astraDeviceUri = dev.getUri();

            if(index >= 0 && index == astraNum)
            {
                //found the good astra
                foundAstra = true;
                break;
            }

            ++astraNum;
        }
    }

    FW_RAISE_IF("Astra Pro OpenNI device not detected.", astraDeviceUri.size() == 0);
    FW_RAISE_IF("Cannot open OpenNI device: " << ::openni::OpenNI::getExtendedError(),
                device->open(astraDeviceUri.c_str()) != ::openni::STATUS_OK);

    m_oniDevice.swap(device);
    m_oniDevice->setImageRegistrationMode(
        ::openni::ImageRegistrationMode::IMAGE_REGISTRATION_OFF);
}

//------------------------------------------------------------------------------

void SScanBase::errorPopup(std::string const& msg)
{
    ::fwGui::dialog::MessageDialog::showMessageDialog(
        "videoOrbbec error",
        msg,
        ::fwGui::dialog::IMessageDialog::CRITICAL);
}

//------------------------------------------------------------------------------

} // namespace videoOrbbec
