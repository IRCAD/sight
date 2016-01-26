/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <arData/Camera.hpp>

#include "videoQt/helper/formats.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCamera>
#include <QObject>

#include <sstream>
#include <map>

Q_DECLARE_METATYPE(QCameraInfo)

namespace videoQt
{
namespace editor
{


//-----------------------------------------------------------------------------

CameraDeviceDlg::CameraDeviceDlg() : QDialog()
{
    QVBoxLayout *mainLayout     = new QVBoxLayout();
    QHBoxLayout *buttonLayout   = new QHBoxLayout();
    QHBoxLayout *selectorLayout = new QHBoxLayout();
    QPushButton *validateButton = new QPushButton("Validate");
    QPushButton *cancelButton   = new QPushButton("Cancel");

    QLabel* deviceLabel = new QLabel("Camera device: ");
    m_devicesComboBox = new QComboBox();
    m_camSettings     = new QListWidget();

    selectorLayout->addWidget(deviceLabel);
    selectorLayout->addWidget(m_devicesComboBox);

    // Add cameras
    const QList<QCameraInfo> devices = QCameraInfo::availableCameras();
    for(const QCameraInfo& camInfo : devices)
    {
        m_devicesComboBox->addItem(camInfo.description(), QVariant::fromValue(camInfo));
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

        camera->setPixelFormat(format);
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
        QCamera cam(camInfo);
        cam.load();
        QList<QCameraViewfinderSettings> settingsList = cam.supportedViewfinderSettings();
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
        cam.unload();
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
