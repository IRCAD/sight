/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include "videoQt/helper/formats.hpp"

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
#include <map>
#include <sstream>

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

    // Add cameras
    const QList<QCameraInfo> devices = QCameraInfo::availableCameras();
    std::map<std::string, std::vector<QCameraInfo> > nameToUID;
    // First run: collect all detected device names and UIDs
    for(const QCameraInfo& camInfo : devices)
    {
        const auto camName = camInfo.description().toStdString();
        if(nameToUID.count(camName))
        {
            auto& uids = nameToUID.at(camName);
            uids.push_back(camInfo);
        }
        else
        {
            nameToUID[camName] = std::vector<QCameraInfo>(1, camInfo);
        }
    }
    // Second run: disambiguate if several cameras with the same name were detected.
    for(auto& p : nameToUID)
    {
        auto const& deviceName = p.first;
        auto& devicesInfo      = p.second;
        if(devicesInfo.size() == 1)
        {
            // Camera is unique
            m_devicesComboBox->addItem(devicesInfo[0].description(), QVariant::fromValue(devicesInfo[0]));
        }
        else
        {
            // Several identical cameras detected, disambiguate
            unsigned int n = 1;
            std::sort(devicesInfo.begin(), devicesInfo.end(),
                      [] (QCameraInfo const& c1, QCameraInfo const& c2)
                    {
                        return c1.deviceName().toStdString() < c2.deviceName().toStdString();
                    });
            for(auto& deviceInfo: devicesInfo)
            {
                std::string uniqueName = deviceName + " #" + std::to_string(n);
                m_devicesComboBox->addItem(QString(uniqueName.c_str()), QVariant::fromValue(deviceInfo));
                ++n;
            }
        }
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
    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidate()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
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

            ::videoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
            iter = ::videoQt::helper::pixelFormatTranslator.left.find(settings.pixelFormat());

            if(iter != ::videoQt::helper::pixelFormatTranslator.left.end())
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
        camera->setDescription(camInfo.description().toStdString());
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

                ::videoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
                iter = ::videoQt::helper::pixelFormatTranslator.left.find(pixFormat);

                if(iter != ::videoQt::helper::pixelFormatTranslator.left.end())
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
                settings.setMaximumFrameRate(30.0f);
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

            ::videoQt::helper::PixelFormatTranslatorType::left_const_iterator iter;
            iter = ::videoQt::helper::pixelFormatTranslator.left.find(settings.pixelFormat());

            if(iter != ::videoQt::helper::pixelFormatTranslator.left.end())
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

void CameraDeviceDlg::onValidate()
{
    this->close();
}

//-----------------------------------------------------------------------------

} // editor
} // videoQt
