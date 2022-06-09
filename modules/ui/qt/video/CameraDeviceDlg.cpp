/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "CameraDeviceDlg.hpp"

#include "modules/ui/qt/video/formats.hpp"

#include <data/Camera.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <QCamera>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QtMultimedia>
#include <QVBoxLayout>

#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_map>

Q_DECLARE_METATYPE(QCameraInfo)

namespace sight::module::ui::qt
{

namespace video
{

//-----------------------------------------------------------------------------

CameraDeviceDlg::CameraDeviceDlg(std::string xmlResolutionConfig) :
    QDialog()
{
    QVBoxLayout* mainLayout     = new QVBoxLayout();
    QHBoxLayout* buttonLayout   = new QHBoxLayout();
    QHBoxLayout* selectorLayout = new QHBoxLayout();
    QPushButton* validateButton = new QPushButton("Validate");
    QPushButton* cancelButton   = new QPushButton("Cancel");

    QLabel* deviceLabel = new QLabel("Camera device: ");
    m_devicesComboBox = new QComboBox();
    m_camSettings     = new QListWidget();

    selectorLayout->addWidget(deviceLabel);
    selectorLayout->addWidget(m_devicesComboBox);

    // Detect available devices.
    const QList<QCameraInfo> devices = QCameraInfo::availableCameras();

    std::map<std::string, QCameraInfo> nameToUID;
    std::vector<std::string> nameList;

    //We should keep the same order as given by Qt, and uniquely identify cameras with the same name.

    // First run: collect all detected device names and UIDs
    std::size_t index = 0;
    for(const QCameraInfo& camInfo : devices)
    {
        //MacOs appends random number when cameras has same names, remove it to do it ourself.
        const std::string qtCamName = camInfo.description().toStdString();
        const std::string camName   = qtCamName.substr(0, qtCamName.rfind("#") - 1);

        // check if the name already exists
        const auto multipleName = std::count(nameList.begin(), nameList.end(), camName);
        std::string uniqueName  = camName;
        if(multipleName > 0)
        {
            uniqueName = camName + " #" + std::to_string(multipleName + 1);
        }

        nameList.push_back(camName);
        // prefix by index to keep the order in the map
        nameToUID[std::to_string(index) + ". " + uniqueName] = camInfo;

        ++index;
    }

    //Second Run: Add generated unique name into the comboBox.
    for(auto& p : nameToUID)
    {
        const auto& deviceName = p.first;
        auto& deviceInfo       = p.second;
        m_devicesComboBox->addItem(QString(deviceName.c_str()), QVariant::fromValue(deviceInfo));
    }

    // Select camera on which the resolution should be applied onto
    if(index > 1 && xmlResolutionConfig != "preferences" && xmlResolutionConfig != "prompt")
    {
        //No need of m_camSettings widget
        buttonLayout->addWidget(validateButton);
        buttonLayout->addWidget(cancelButton);

        mainLayout->addLayout(selectorLayout);
        mainLayout->addLayout(buttonLayout);

        this->setModal(true);
        this->setLayout(mainLayout);
        this->setWindowTitle("Camera device selector");
    }
    else
    {
        buttonLayout->addWidget(validateButton);
        buttonLayout->addWidget(cancelButton);

        mainLayout->addLayout(selectorLayout);
        mainLayout->addWidget(m_camSettings);
        mainLayout->addLayout(buttonLayout);

        this->setModal(true);
        this->setLayout(mainLayout);
        this->setWindowTitle("Camera device selector");
    }

    this->onSelectDevice(m_devicesComboBox->currentIndex());

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onSelectDevice(int)));
    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------

bool CameraDeviceDlg::getSelectedCamera(data::Camera::sptr& camera, std::string& resolutionXMLOption)
{
    int index = m_devicesComboBox->currentIndex();
    if(index >= 0)
    {
        QCameraInfo camInfo = qvariant_cast<QCameraInfo>(m_devicesComboBox->itemData(index));
        QCamera cam(camInfo);
        cam.load();

        QCameraImageCapture imageCapture(&cam);
        const QList<QSize> supportedResolutions = imageCapture.supportedResolutions();

        [[maybe_unused]] data::Camera::PixelFormat format = data::Camera::PixelFormat::INVALID;

        QListWidgetItem* item = m_camSettings->currentItem();

        const auto resolutionWarning =
            [this](size_t source_res_x, size_t source_res_y, int target_res_x, int target_res_y)
            {
                QMessageBox::warning(
                    this,
                    "Warning",
                    QString(
                        "The selected resolution (%3x%4) does not match the calibration (%1x%2).\n"
                        "Please select a video source with a resolution of %1x%2."
                    ).arg(source_res_x).arg(source_res_y).arg(target_res_x).arg(target_res_y)
                );
            };

        if((item && resolutionXMLOption == "preferences")
           || (item && resolutionXMLOption == "prompt"))
        {
            QCameraViewfinderSettings settings = qvariant_cast<QCameraViewfinderSettings>(item->data(Qt::UserRole));
            camera->setMaximumFrameRate(static_cast<float>(settings.maximumFrameRate()));

            if((camera->getWidth() != 0 || camera->getHeight() != 0)
               && !(camera->getWidth() == static_cast<size_t>(settings.resolution().width())
                    && camera->getHeight() == static_cast<size_t>(settings.resolution().height()))
               && camera->getIsCalibrated())
            {
                resolutionWarning(
                    camera->getWidth(),
                    camera->getHeight(),
                    settings.resolution().width(),
                    settings.resolution().height()
                );
                return false;
            }

            camera->setWidth(static_cast<std::size_t>(settings.resolution().width()));
            camera->setHeight(static_cast<std::size_t>(settings.resolution().height()));

            PixelFormatTranslatorType::left_const_iterator iter;
            iter = pixelFormatTranslator.left.find(settings.pixelFormat());
            try
            {
                sight::ui::base::Preferences resolutionPreference;
                const auto prefValue = std::to_string(settings.resolution().width()) + "x" + std::to_string(
                    settings.resolution().height()
                );
                resolutionPreference.put(SCamera::s_RESOLUTION_PREF_KEY, prefValue);
            }
            catch(boost::property_tree::ptree_error&)
            {
                SIGHT_ERROR(" Couldn't save preference");
            }

            if(iter != pixelFormatTranslator.left.end())
            {
                format = iter->second;
            }
            else
            {
                SIGHT_ERROR("No compatible pixel format found");
            }
        }
        else if(!resolutionXMLOption.empty())
        {
            const QSize xmlResolutionValue = this->getResolution(resolutionXMLOption, supportedResolutions);
            if(xmlResolutionValue.isNull())
            {
                return false;
            }

            camera->setMaximumFrameRate(30.f);

            if((camera->getWidth() != 0 || camera->getHeight() != 0)
               && !(camera->getWidth() == static_cast<size_t>(xmlResolutionValue.width())
                    && camera->getHeight() == static_cast<size_t>(xmlResolutionValue.height()))
               && camera->getIsCalibrated())
            {
                resolutionWarning(
                    camera->getWidth(),
                    camera->getHeight(),
                    xmlResolutionValue.width(),
                    xmlResolutionValue.height()
                );
                return false;
            }

            camera->setHeight(static_cast<size_t>(xmlResolutionValue.height()));
            camera->setWidth(static_cast<size_t>(xmlResolutionValue.width()));
        }
        else
        {
            SIGHT_ERROR("No camera setting selected, using default...");
            camera->setMaximumFrameRate(30.f);
            camera->setHeight(0);
            camera->setWidth(0);
            return false;
        }

//FIXME : Setting the pixel format generate an error (gstreamer)
#ifndef __linux__
        camera->setPixelFormat(format);
#endif
        camera->setCameraSource(data::Camera::DEVICE);
        camera->setCameraID(camInfo.deviceName().toStdString());
        //Use our description.
        camera->setDescription(m_devicesComboBox->currentText().toStdString());
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void CameraDeviceDlg::onSelectDevice(int index)
{
    m_camSettings->clear();
    if(index >= 0)
    {
        QCameraInfo camInfo = qvariant_cast<QCameraInfo>(m_devicesComboBox->itemData(index));
        QCamera* cam        = new QCamera(camInfo);
        cam->load();

#ifdef __linux__
        //NOTE : Work around for the camera resolution settings on linux (maybe on OSX too)
        QCameraImageCapture* imageCapture          = new QCameraImageCapture(cam);
        QList<QSize> res                           = imageCapture->supportedResolutions();
        QList<QVideoFrame::PixelFormat> pixFormats = imageCapture->supportedBufferFormats();

        for(const QSize& supportedSize : res)
        {
            for(const QVideoFrame::PixelFormat& pixFormat : pixFormats)
            {
                data::Camera::PixelFormat format = data::Camera::PixelFormat::INVALID;

                PixelFormatTranslatorType::left_const_iterator iter;
                iter = pixelFormatTranslator.left.find(pixFormat);

                if(iter != pixelFormatTranslator.left.end())
                {
                    format = iter->second;
                }
                else
                {
                    SIGHT_ERROR("No compatible pixel format found");
                }

                //Create QCameraViewfinderSettings from the information of the QCameraImageCapture
                QCameraViewfinderSettings settings;
                //TODO : Can we get the maximum frameRate from an other way ?
                settings.setMaximumFrameRate(30.0);
                settings.setResolution(supportedSize);
                //FIXME : Setting the pixel format generate an error (gstreamer) (see getSelectedCamera method)
                settings.setPixelFormat(pixFormat);

                std::stringstream stream;
                stream << "[" << settings.resolution().width() << "X" << settings.resolution().height() << "]";
                stream << "\t" << settings.maximumFrameRate() << " fps";
                stream << "\tFormat:" << data::Camera::getPixelFormatName(format);
                QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(stream.str()));
                item->setData(Qt::UserRole, QVariant::fromValue(settings));
                m_camSettings->addItem(item);
            }
        }

        delete imageCapture;
#else
        QList<QCameraViewfinderSettings> settingsList = cam->supportedViewfinderSettings();
        for(const QCameraViewfinderSettings& settings : settingsList)
        {
            data::Camera::PixelFormat format = data::Camera::PixelFormat::INVALID;

            PixelFormatTranslatorType::left_const_iterator iter;
            iter = pixelFormatTranslator.left.find(settings.pixelFormat());

            if(iter != pixelFormatTranslator.left.end())
            {
                format = iter->second;
            }
            else
            {
                SIGHT_ERROR("No compatible pixel format found");
            }

            std::stringstream stream;
            stream << "[" << settings.resolution().width() << "X" << settings.resolution().height() << "]";
            stream << "\t" << settings.maximumFrameRate() << " fps";
            stream << "\tFormat:" << data::Camera::getPixelFormatName(format);
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(stream.str()));
            item->setData(Qt::UserRole, QVariant::fromValue(settings));
            m_camSettings->addItem(item);
        }
#endif //linux

        cam->unload();

        delete cam;
    }
}

//-----------------------------------------------------------------------------
QSize CameraDeviceDlg::getResolution(const std::string& resolutionXMLOption, const QList<QSize>& supportedResolutions)
{
    if(resolutionXMLOption == "min")
    {
        const auto criteria = [&](const QSize& a, const QSize& b)
                              {
                                  return (a.width() * a.height()) <= (b.width() * b.height());
                              };
        const QSize min = *std::min_element(supportedResolutions.begin(), supportedResolutions.end(), criteria);
        return min;
    }
    else if(resolutionXMLOption == "max")
    {
        const auto criteria = [&](const QSize& a, const QSize& b)
                              {
                                  return (a.width() * a.height()) <= (b.width() * b.height());
                              };
        const QSize max = *std::max_element(supportedResolutions.begin(), supportedResolutions.end(), criteria);
        return max;
    }
    else if(resolutionXMLOption == "preferences")
    {
        try
        {
            sight::ui::base::Preferences resolutionPreference;
            auto resolutionPreferenceStr = resolutionPreference.get<std::string>(SCamera::s_RESOLUTION_PREF_KEY);
            std::regex resPattern("(\\d*)[Xx](\\d*)");
            std::smatch match;
            std::regex_match(resolutionPreferenceStr, match, resPattern);
            if(!match.empty())
            {
                int xmlWidth  = std::stoi(std::string(match[1].first, match[1].second));
                int xmlHeight = std::stoi(std::string(match[2].first, match[2].second));
                const QSize res {xmlWidth, xmlHeight};

                // find the resolution among the supportedResolutions list
                int i = supportedResolutions.indexOf(res);
                if(i == -1)
                {
                    QMessageBox::critical(
                        this,
                        "Error",
                        "The requested resolution is not supported."
                    );
                }
                else
                {
                    return res;
                }
            }
            else
            {
                QMessageBox::critical(
                    this,
                    "Error",
                    "The requested resolution is not supported."
                );
            }
        }
        catch(boost::property_tree::ptree_error&)
        {
            SIGHT_ERROR("Couldn't get preference. The required key doesn't exist.");
        }
    }
    else
    {
        std::regex resPattern("(\\d*)[Xx](\\d*)");
        std::smatch match;
        std::regex_match(resolutionXMLOption, match, resPattern);
        if(!match.empty())
        {
            int xmlWidth  = std::stoi(std::string(match[1].first, match[1].second));
            int xmlHeight = std::stoi(std::string(match[2].first, match[2].second));
            const QSize res {xmlWidth, xmlHeight};

            // find the resolution among the supportedResolutions list
            int i = supportedResolutions.indexOf(res);
            if(i == -1)
            {
                QMessageBox::critical(
                    this,
                    "Error",
                    "The requested resolution is not supported."
                );
            }
            else
            {
                return res;
            }
        }
        else
        {
            QMessageBox::critical(
                this,
                "Error",
                "The requested resolution is not supported."
            );
        }
    }

    return {0, 0};
}

} // video

} // sight::module::ui::qt
