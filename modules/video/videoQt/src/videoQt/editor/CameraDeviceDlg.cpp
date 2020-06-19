/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include "fwVideoQt/helper/formats.hpp"

#include <arData/Camera.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <QCamera>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QtMultimedia>
#include <QVBoxLayout>

#include <algorithm>
#include <sstream>
#include <unordered_map>

Q_DECLARE_METATYPE(QCameraInfo)

namespace videoQt
{
namespace editor
{

//-----------------------------------------------------------------------------

CameraDeviceDlg::CameraDeviceDlg() :
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

    std::map<std::string, QCameraInfo > nameToUID;
    std::vector< std::string> nameList;

    //We should keep the same order than given by Qt, and uniquely identify cameras with the same name.

    // First run: collect all detected device names and UIDs
    size_t index = 0;
    for(const QCameraInfo& camInfo : devices)
    {
        //MacOs appends random number when cameras has same names, remove it to do it ourself.
        const std::string qtCamName = camInfo.description().toStdString();
        const std::string camName   = qtCamName.substr(0, qtCamName.rfind("#") - 1);

        // check if the name already exists
        const auto multipleName = std::count(nameList.begin(), nameList.end(), camName);
        std::string uniqueName  = camName;
        if(  multipleName > 0)
        {
            uniqueName = camName + " #" + std::to_string(multipleName+1);
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
        m_devicesComboBox->addItem( QString(deviceName.c_str()), QVariant::fromValue(deviceInfo));
    }

    buttonLayout->addWidget(validateButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(selectorLayout);
    mainLayout->addWidget(m_camSettings);
    mainLayout->addLayout(buttonLayout);

    this->setModal(true);
    this->setLayout(mainLayout);
    this->setWindowTitle("Camera device selector");

    this->onSelectDevice(m_devicesComboBox->currentIndex());

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onSelectDevice(int)));
    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------

CameraDeviceDlg::~CameraDeviceDlg()
{
}

//-----------------------------------------------------------------------------

bool CameraDeviceDlg::getSelectedCamera(::arData::Camera::sptr& camera)
{
    int index = m_devicesComboBox->currentIndex();
    if(index >= 0)
    {
        QCameraInfo camInfo = qvariant_cast<QCameraInfo>(m_devicesComboBox->itemData(index));
        ::arData::Camera::PixelFormat format = ::arData::Camera::PixelFormat::INVALID;

        QListWidgetItem* item = m_camSettings->currentItem();
        if(item)
        {
            QCameraViewfinderSettings settings = qvariant_cast<QCameraViewfinderSettings>(item->data(Qt::UserRole));
            camera->setMaximumFrameRate(static_cast<float>(settings.maximumFrameRate()));
            camera->setHeight(static_cast<size_t>(settings.resolution().height()));
            camera->setWidth(static_cast<size_t>(settings.resolution().width()));

            ::fwVideoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
            iter = ::fwVideoQt::helper::pixelFormatTranslator.left.find(settings.pixelFormat());

            if(iter != ::fwVideoQt::helper::pixelFormatTranslator.left.end())
            {
                format = iter->second;
            }
            else
            {
                OSLM_ERROR("No compatible pixel format found");
            }
        }
        else
        {
            camera->setMaximumFrameRate(30.f);
            camera->setHeight(0);
            camera->setWidth(0);

            OSLM_ERROR("No camera setting selected, using default...");
        }

//FIXME : Setting the pixel format generate an error (gstreamer)
#ifndef __linux__
        camera->setPixelFormat(format);
#endif
        camera->setCameraSource(::arData::Camera::DEVICE);
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

        //NOTE : Work arround for the camera resolution settings on linux (maybe on OSX too)
        QCameraImageCapture* imageCapture            = new QCameraImageCapture(cam);
        QList<QSize> res                             = imageCapture->supportedResolutions();
        QList< QVideoFrame::PixelFormat > pixFormats = imageCapture->supportedBufferFormats();

        for(const QSize& supportedSize : res)
        {
            for(const QVideoFrame::PixelFormat& pixFormat : pixFormats)
            {
                ::arData::Camera::PixelFormat format = ::arData::Camera::PixelFormat::INVALID;

                ::fwVideoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
                iter = ::fwVideoQt::helper::pixelFormatTranslator.left.find(pixFormat);

                if(iter != ::fwVideoQt::helper::pixelFormatTranslator.left.end())
                {
                    format = iter->second;
                }
                else
                {
                    OSLM_ERROR("No compatible pixel format found");
                }

                //Create QCameraViewfinderSettings from the informations of the QCameraImageCapture
                QCameraViewfinderSettings settings;
                //TODO : Can we get the maximum frameRate from an other way ?
                settings.setMaximumFrameRate(30.0);
                settings.setResolution(supportedSize);
                //FIXME : Setting the pixel format generate an error (gstreamer) (see getSelectedCamera method)
                settings.setPixelFormat(pixFormat);

                std::stringstream stream;
                stream << "[" << settings.resolution().width() << "X" << settings.resolution().height() << "]";
                stream << "\t" << settings.maximumFrameRate() << " fps";
                stream << "\tFormat:" << ::arData::Camera::getPixelFormatName(format);
                QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(stream.str()));
                item->setData(Qt::UserRole, QVariant::fromValue(settings));
                m_camSettings->addItem(item);
            }

        }

        delete imageCapture;
#else
        QList<QCameraViewfinderSettings> settingsList = cam->supportedViewfinderSettings();
        for(const QCameraViewfinderSettings& settings : settingsList )
        {
            ::arData::Camera::PixelFormat format = ::arData::Camera::PixelFormat::INVALID;

            ::fwVideoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
            iter = ::fwVideoQt::helper::pixelFormatTranslator.left.find(settings.pixelFormat());

            if(iter != ::fwVideoQt::helper::pixelFormatTranslator.left.end())
            {
                format = iter->second;
            }
            else
            {
                OSLM_ERROR("No compatible pixel format found");
            }

            std::stringstream stream;
            stream << "[" << settings.resolution().width() << "X" << settings.resolution().height() << "]";
            stream << "\t" << settings.maximumFrameRate() << " fps";
            stream << "\tFormat:" << ::arData::Camera::getPixelFormatName(format);
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

} // editor
} // videoQt
