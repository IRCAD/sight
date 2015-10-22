/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <arData/Camera.hpp>

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

typedef std::map<QVideoFrame::PixelFormat, ::arData::Camera::PixelFormat> PixelFormatTranslatorType;
PixelFormatTranslatorType pixelFormatTranslator
{
    {QVideoFrame::PixelFormat::Format_Invalid,                ::arData::Camera::PixelFormat::INVALID               },
    {QVideoFrame::PixelFormat::Format_ARGB32,                 ::arData::Camera::PixelFormat::ARGB32                },
    {QVideoFrame::PixelFormat::Format_ARGB32_Premultiplied,   ::arData::Camera::PixelFormat::ARGB32_PREMULTIPLIED  },
    {QVideoFrame::PixelFormat::Format_RGB32,                  ::arData::Camera::PixelFormat::RGB32                 },
    {QVideoFrame::PixelFormat::Format_RGB24,                  ::arData::Camera::PixelFormat::RGB24                 },
    {QVideoFrame::PixelFormat::Format_RGB565,                 ::arData::Camera::PixelFormat::RGB565                },
    {QVideoFrame::PixelFormat::Format_RGB555,                 ::arData::Camera::PixelFormat::RGB555                },
    {QVideoFrame::PixelFormat::Format_ARGB8565_Premultiplied, ::arData::Camera::PixelFormat::ARGB8565_PREMULTIPLIED},
    {QVideoFrame::PixelFormat::Format_BGRA32,                 ::arData::Camera::PixelFormat::BGRA32                },
    {QVideoFrame::PixelFormat::Format_BGRA32_Premultiplied,   ::arData::Camera::PixelFormat::BGRA32_PREMULTIPLIED  },
    {QVideoFrame::PixelFormat::Format_BGR32,                  ::arData::Camera::PixelFormat::BGR32                 },
    {QVideoFrame::PixelFormat::Format_BGR24,                  ::arData::Camera::PixelFormat::BGR24                 },
    {QVideoFrame::PixelFormat::Format_BGR565,                 ::arData::Camera::PixelFormat::BGR565                },
    {QVideoFrame::PixelFormat::Format_BGR555,                 ::arData::Camera::PixelFormat::BGR555                },
    {QVideoFrame::PixelFormat::Format_BGRA5658_Premultiplied, ::arData::Camera::PixelFormat::BGRA5658_PREMULTIPLIED},
    {QVideoFrame::PixelFormat::Format_AYUV444,                ::arData::Camera::PixelFormat::AYUV444               },
    {QVideoFrame::PixelFormat::Format_AYUV444_Premultiplied,  ::arData::Camera::PixelFormat::AYUV444_PREMULTIPLIED },
    {QVideoFrame::PixelFormat::Format_YUV444,                 ::arData::Camera::PixelFormat::YUV444                },
    {QVideoFrame::PixelFormat::Format_YUV420P,                ::arData::Camera::PixelFormat::YUV420P               },
    {QVideoFrame::PixelFormat::Format_YV12,                   ::arData::Camera::PixelFormat::YV12                  },
    {QVideoFrame::PixelFormat::Format_UYVY,                   ::arData::Camera::PixelFormat::UYVY                  },
    {QVideoFrame::PixelFormat::Format_YUYV,                   ::arData::Camera::PixelFormat::YUYV                  },
    {QVideoFrame::PixelFormat::Format_NV12,                   ::arData::Camera::PixelFormat::NV12                  },
    {QVideoFrame::PixelFormat::Format_NV21,                   ::arData::Camera::PixelFormat::NV21                  },
    {QVideoFrame::PixelFormat::Format_IMC1,                   ::arData::Camera::PixelFormat::IMC1                  },
    {QVideoFrame::PixelFormat::Format_IMC2,                   ::arData::Camera::PixelFormat::IMC2                  },
    {QVideoFrame::PixelFormat::Format_IMC3,                   ::arData::Camera::PixelFormat::IMC3                  },
    {QVideoFrame::PixelFormat::Format_IMC4,                   ::arData::Camera::PixelFormat::IMC4                  },
    {QVideoFrame::PixelFormat::Format_Y8,                     ::arData::Camera::PixelFormat::Y8                    },
    {QVideoFrame::PixelFormat::Format_Y16,                    ::arData::Camera::PixelFormat::Y16                   },
    {QVideoFrame::PixelFormat::Format_Jpeg,                   ::arData::Camera::PixelFormat::JPEG                  },
    {QVideoFrame::PixelFormat::Format_CameraRaw,              ::arData::Camera::PixelFormat::CAMERARAW             },
    {QVideoFrame::PixelFormat::Format_AdobeDng,               ::arData::Camera::PixelFormat::ADOBEDNG              },
    {QVideoFrame::PixelFormat::Format_User,                   ::arData::Camera::PixelFormat::USER                  }
};

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

bool CameraDeviceDlg::getSelectedCamera(::arData::Camera::sptr camera)
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
            camera->setMaximumFrameRate(settings.maximumFrameRate());
            camera->setMaximumFrameRate(settings.maximumFrameRate());
            camera->setHeight(settings.resolution().height());
            camera->setWidth(settings.resolution().width());

            PixelFormatTranslatorType::const_iterator iter = pixelFormatTranslator.find(settings.pixelFormat());
            if(iter != pixelFormatTranslator.end())
            {
                format = iter->second;
            }
        }
        else
        {
            camera->setMaximumFrameRate(30.f);
            camera->setHeight(0);
            camera->setWidth(0);
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
            PixelFormatTranslatorType::const_iterator iter = pixelFormatTranslator.find(settings.pixelFormat());
            ::arData::Camera::PixelFormat format = ::arData::Camera::PixelFormat::INVALID;
            if(iter != pixelFormatTranslator.end())
            {
                format = iter->second;
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
