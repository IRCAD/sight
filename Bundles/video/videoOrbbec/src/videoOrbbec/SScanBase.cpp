/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOrbbec/SScanBase.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwVideoQt/helper/formats.hpp>
#include <fwVideoQt/Registry.hpp>

#include <QCamera>
#include <QCameraInfo>

#include <cmath>

namespace videoOrbbec
{

const std::string SScanBase::s_QT_CAMERA_STRING = "Astra Pro HD Camera";

//------------------------------------------------------------------------------

SScanBase::SScanBase() noexcept :
    m_oniDevice(nullptr),
    m_720p(false)
{
}

//------------------------------------------------------------------------------

SScanBase::~SScanBase() noexcept
{
}

//------------------------------------------------------------------------------

void SScanBase::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr videoModeConfig =
        m_configuration->findConfigurationElement("videoMode");
    if(videoModeConfig)
    {
        if(videoModeConfig->getValue() == "720p")
        {
            m_720p = true;
        }
        else if(videoModeConfig->getValue() == "VGA")
        {
            m_720p = false;
        }
        else
        {
            SLM_FATAL("Unknown value for the 'videoMode' attribute: " + videoModeConfig->getValue()
                      + ". Valid values are '720p' and 'VGA'.");
        }
    }
}

//------------------------------------------------------------------------------

void SScanBase::stopping()
{
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
    ::openni::Array< ::openni::DeviceInfo> devices;
    ::openni::OpenNI::enumerateDevices(&devices);
    OpenniDevicePtr device = std::unique_ptr< ::openni::Device >(new ::openni::Device());
    std::string astraDeviceUri;

    for(int i = 0; i != devices.getSize(); ++i)
    {
        auto const& dev = devices[i];
        if(!(std::strcmp("Orbbec", dev.getVendor()) || std::strcmp("Astra", dev.getName())
             || dev.getUsbVendorId() != 11205))
        {
            astraDeviceUri = dev.getUri();
            break;
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

::arData::Camera::sptr SScanBase::detectCameraQt()
{
    // Workaround https://bugreports.qt.io/browse/QTBUG-59320
    QCamera* cam = new QCamera;
    delete cam;

    const auto qtCameras = QCameraInfo::availableCameras();
    bool foundAstra      = false;
    auto cameraData      = ::arData::Camera::New();
    for(auto const& it: qtCameras)
    {
        OSLM_DEBUG("Setting video mode to " << (m_720p ? "720p" : "VGA"));
        // Some OS's (hem OS X) append semi-random numbers to the camera name depending on which USB port
        // you plug it in. So, we extract a substring of desired length to get rid of that
        auto camName = it.description().toStdString();
        if(camName.length() > s_QT_CAMERA_STRING.length())
        {
            camName = camName.substr(0, s_QT_CAMERA_STRING.length());
        }
        else if(camName.length() < s_QT_CAMERA_STRING.length())
        {
            continue;
        }
        if(camName == s_QT_CAMERA_STRING)
        {
            // Found a match, pick a video mode and fill the arData::Camera
            foundAstra = true;
#ifdef WIN32
            QCamera qtCamera(it);
            qtCamera.load();
            const auto settingsList = qtCamera.supportedViewfinderSettings();
            const auto settings     =
                std::find_if(settingsList.begin(), settingsList.end(),
                             [&] (auto& s)
                {
                    auto res = s.resolution();
                    if(m_720p)
                    {
                        return res.width() == 1280
                        && res.height() == 720
                        && std::lround(s.minimumFrameRate()) == 10;
                    }
                    else
                    {
                        return res.width() == 640
                        && res.height() == 480
                        && std::lround(s.minimumFrameRate()) == 30;
                    }
                });
            FW_RAISE_IF("No valid videomodes detected", settings == settingsList.end());
            const auto resolution = settings->resolution();
            const auto formatIter = ::fwVideoQt::helper::pixelFormatTranslator.left.find(settings->pixelFormat());
            FW_RAISE_IF("No compatible pixel format detected",
                        formatIter == ::fwVideoQt::helper::pixelFormatTranslator.left.end());

            // startCamera needs to know the frame size, so we set it here
            cameraData->setWidth(resolution.width());
            cameraData->setHeight(resolution.height());
            cameraData->setMaximumFrameRate(settings->maximumFrameRate());
#else
            // This is a temporary workaround against Qt multimedia not being able to load
            // the camera on linux until we can find a proper solution...
            cameraData->setWidth(m_720p ? 1280 : 640);
            cameraData->setHeight(m_720p ? 720 : 480);
            cameraData->setMaximumFrameRate(m_720p ? 10 : 30);
#endif
            cameraData->setCameraSource(::arData::Camera::DEVICE);
            cameraData->setDescription(s_QT_CAMERA_STRING);
            cameraData->setCameraID(it.deviceName().toStdString());
            break;
        }
    }
    return cameraData;
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

void SScanBase::qtMessageHandler(QtMsgType type, QMessageLogContext const& ctx, QString const& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
        case QtDebugMsg:
            SLM_DEBUG(localMsg.constData());
            break;
        case QtInfoMsg:
            SLM_INFO(localMsg.constData());
            break;
        case QtWarningMsg:
            SLM_WARN(localMsg.constData());
            break;
        case QtCriticalMsg:
            SLM_ERROR(localMsg.constData());
            break;
        case QtFatalMsg:
            SLM_FATAL(localMsg.constData());
            break;
    }
}

} // namespace videoOrbbec
