/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <data/camera.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

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

namespace sight::module::ui::qt::video
{

//-----------------------------------------------------------------------------

CameraDeviceDlg::CameraDeviceDlg(std::string _xml_resolution_config) :
    m_devicesComboBox(new QComboBox()),
    m_camSettings(new QListWidget())
{
    auto* main_layout     = new QVBoxLayout();
    auto* button_layout   = new QHBoxLayout();
    auto* selector_layout = new QHBoxLayout();
    auto* validate_button = new QPushButton("Validate");
    auto* cancel_button   = new QPushButton("Cancel");

    auto* device_label = new QLabel("Camera device: ");

    selector_layout->addWidget(device_label);
    selector_layout->addWidget(m_devicesComboBox);

    // Detect available devices.
    const QList<QCameraInfo> devices = QCameraInfo::availableCameras();

    std::map<std::string, QCameraInfo> name_to_uid;
    std::vector<std::string> name_list;

    //We should keep the same order as given by Qt, and uniquely identify cameras with the same name.

    // First run: collect all detected device names and UIDs
    std::size_t index = 0;
    for(const QCameraInfo& cam_info : devices)
    {
        //MacOs appends random number when cameras has same names, remove it to do it ourself.
        const std::string qt_cam_name = cam_info.description().toStdString();
        const std::string cam_name    = qt_cam_name.substr(0, qt_cam_name.rfind('#') - 1);

        // check if the name already exists
        const auto multiple_name = std::count(name_list.begin(), name_list.end(), cam_name);
        std::string unique_name  = cam_name;
        if(multiple_name > 0)
        {
            unique_name = cam_name + " #" + std::to_string(multiple_name + 1);
        }

        name_list.push_back(cam_name);
        // prefix by index to keep the order in the map
        name_to_uid[std::to_string(index) + ". " + unique_name] = cam_info;

        ++index;
    }

    //Second Run: Add generated unique name into the comboBox.
    for(auto& p : name_to_uid)
    {
        const auto& device_name = p.first;
        auto& device_info       = p.second;
        m_devicesComboBox->addItem(QString(device_name.c_str()), QVariant::fromValue(device_info));
    }

    // Select camera on which the resolution should be applied onto
    if(index > 1 && _xml_resolution_config != "preferences" && _xml_resolution_config != "prompt")
    {
        //No need of m_camSettings widget
        button_layout->addWidget(validate_button);
        button_layout->addWidget(cancel_button);

        main_layout->addLayout(selector_layout);
        main_layout->addLayout(button_layout);

        this->setModal(true);
        this->setLayout(main_layout);
        this->setWindowTitle("Camera device selector");
    }
    else
    {
        button_layout->addWidget(validate_button);
        button_layout->addWidget(cancel_button);

        main_layout->addLayout(selector_layout);
        main_layout->addWidget(m_camSettings);
        main_layout->addLayout(button_layout);

        this->setModal(true);
        this->setLayout(main_layout);
        this->setWindowTitle("Camera device selector");
    }

    this->onSelectDevice(m_devicesComboBox->currentIndex());

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onSelectDevice(int)));
    QObject::connect(validate_button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------

bool CameraDeviceDlg::getSelectedCamera(data::camera::sptr& _camera, std::string& _resolution_xml_option)
{
    int index = m_devicesComboBox->currentIndex();
    if(index >= 0)
    {
        auto cam_info = qvariant_cast<QCameraInfo>(m_devicesComboBox->itemData(index));
        QCamera cam(cam_info);
        cam.load();

        QCameraImageCapture image_capture(&cam);
        const QList<QSize> supported_resolutions = image_capture.supportedResolutions();

        [[maybe_unused]] data::camera::PixelFormat format = data::camera::PixelFormat::INVALID;

        QListWidgetItem* item = m_camSettings->currentItem();

        const auto resolution_warning =
            [this](size_t _source_res_x, size_t _source_res_y, int _target_res_x, int _target_res_y)
            {
                QMessageBox::warning(
                    this,
                    "Warning",
                    QString(
                        "The selected resolution (%3x%4) does not match the calibration (%1x%2).\n"
                        "Please select a video source with a resolution of %1x%2."
                    ).arg(_source_res_x).arg(_source_res_y).arg(_target_res_x).arg(_target_res_y)
                );
            };

        if(((item != nullptr) && _resolution_xml_option == "preferences")
           || ((item != nullptr) && _resolution_xml_option == "prompt"))
        {
            auto settings = qvariant_cast<QCameraViewfinderSettings>(item->data(Qt::UserRole));
            _camera->setMaximumFrameRate(static_cast<float>(settings.maximumFrameRate()));

            if((_camera->getWidth() != 0 || _camera->getHeight() != 0)
               && !(_camera->getWidth() == static_cast<size_t>(settings.resolution().width())
                    && _camera->getHeight() == static_cast<size_t>(settings.resolution().height()))
               && _camera->getIsCalibrated())
            {
                resolution_warning(
                    _camera->getWidth(),
                    _camera->getHeight(),
                    settings.resolution().width(),
                    settings.resolution().height()
                );
                return false;
            }

            _camera->setWidth(static_cast<std::size_t>(settings.resolution().width()));
            _camera->setHeight(static_cast<std::size_t>(settings.resolution().height()));

            pixel_format_translator_t::left_const_iterator iter;
            iter = pixel_format_translator.left.find(settings.pixelFormat());
            try
            {
                sight::ui::preferences resolution_preference;
                const auto pref_value = std::to_string(settings.resolution().width()) + "x" + std::to_string(
                    settings.resolution().height()
                );
                resolution_preference.put(camera::s_RESOLUTION_PREF_KEY, pref_value);
            }
            catch(boost::property_tree::ptree_error&)
            {
                SIGHT_ERROR(" Couldn't save preference");
            }

            if(iter != pixel_format_translator.left.end())
            {
                format = iter->second;
            }
            else
            {
                SIGHT_ERROR("No compatible pixel format found");
            }
        }
        else if(!_resolution_xml_option.empty())
        {
            const QSize xml_resolution_value = this->getResolution(_resolution_xml_option, supported_resolutions);
            if(xml_resolution_value.isNull())
            {
                return false;
            }

            _camera->setMaximumFrameRate(30.F);

            if((_camera->getWidth() != 0 || _camera->getHeight() != 0)
               && !(_camera->getWidth() == static_cast<size_t>(xml_resolution_value.width())
                    && _camera->getHeight() == static_cast<size_t>(xml_resolution_value.height()))
               && _camera->getIsCalibrated())
            {
                resolution_warning(
                    _camera->getWidth(),
                    _camera->getHeight(),
                    xml_resolution_value.width(),
                    xml_resolution_value.height()
                );
                return false;
            }

            _camera->setHeight(static_cast<size_t>(xml_resolution_value.height()));
            _camera->setWidth(static_cast<size_t>(xml_resolution_value.width()));
        }
        else
        {
            SIGHT_ERROR("No camera setting selected, using default...");
            _camera->setMaximumFrameRate(30.F);
            _camera->setHeight(0);
            _camera->setWidth(0);
            return false;
        }

//FIXME : Setting the pixel format generate an error (gstreamer)
#ifndef __linux__
        _camera->setPixelFormat(format);
#endif
        _camera->setCameraSource(data::camera::DEVICE);
        _camera->setCameraID(cam_info.deviceName().toStdString());
        //Use our description.
        _camera->setDescription(m_devicesComboBox->currentText().toStdString());
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void CameraDeviceDlg::onSelectDevice(int _index)
{
    m_camSettings->clear();
    if(_index >= 0)
    {
        auto cam_info = qvariant_cast<QCameraInfo>(m_devicesComboBox->itemData(_index));
        auto* cam     = new QCamera(cam_info);
        cam->load();

#ifdef __linux__
        //NOTE : Work around for the camera resolution settings on linux (maybe on OSX too)
        auto* image_capture                         = new QCameraImageCapture(cam);
        QList<QSize> res                            = image_capture->supportedResolutions();
        QList<QVideoFrame::PixelFormat> pix_formats = image_capture->supportedBufferFormats();

        for(const QSize& supported_size : res)
        {
            for(const QVideoFrame::PixelFormat& pix_format : pix_formats)
            {
                data::camera::PixelFormat format = data::camera::PixelFormat::INVALID;

                pixel_format_translator_t::left_const_iterator iter;
                iter = pixel_format_translator.left.find(pix_format);

                if(iter != pixel_format_translator.left.end())
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
                settings.setResolution(supported_size);
                //FIXME : Setting the pixel format generate an error (gstreamer) (see getSelectedCamera method)
                settings.setPixelFormat(pix_format);

                std::stringstream stream;
                stream << "[" << settings.resolution().width() << "X" << settings.resolution().height() << "]";
                stream << "\t" << settings.maximumFrameRate() << " fps";
                stream << "\tFormat:" << data::camera::getPixelFormatName(format);
                auto* item = new QListWidgetItem(QString::fromStdString(stream.str()));
                item->setData(Qt::UserRole, QVariant::fromValue(settings));
                m_camSettings->addItem(item);
            }
        }

        delete image_capture;
#else
        QList<QCameraViewfinderSettings> settingsList = cam->supportedViewfinderSettings();
        for(const QCameraViewfinderSettings& settings : settingsList)
        {
            data::camera::PixelFormat format = data::camera::PixelFormat::INVALID;

            pixel_format_translator_t::left_const_iterator iter;
            iter = pixel_format_translator.left.find(settings.pixelFormat());

            if(iter != pixel_format_translator.left.end())
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
            stream << "\tFormat:" << data::camera::getPixelFormatName(format);
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
QSize CameraDeviceDlg::getResolution(
    const std::string& _resolution_xml_option,
    const QList<QSize>& _supported_resolutions
)
{
    if(_resolution_xml_option == "min")
    {
        const auto criteria = [&](const QSize& _a, const QSize& _b)
                              {
                                  return (_a.width() * _a.height()) <= (_b.width() * _b.height());
                              };
        const QSize min = *std::min_element(_supported_resolutions.begin(), _supported_resolutions.end(), criteria);
        return min;
    }

    if(_resolution_xml_option == "max")
    {
        const auto criteria = [&](const QSize& _a, const QSize& _b)
                              {
                                  return (_a.width() * _a.height()) <= (_b.width() * _b.height());
                              };
        const QSize max = *std::max_element(_supported_resolutions.begin(), _supported_resolutions.end(), criteria);
        return max;
    }

    if(_resolution_xml_option == "preferences")
    {
        try
        {
            sight::ui::preferences resolution_preference;
            auto resolution_preference_str = resolution_preference.get<std::string>(camera::s_RESOLUTION_PREF_KEY);
            std::regex res_pattern("(\\d*)[Xx](\\d*)");
            std::smatch match;
            std::regex_match(resolution_preference_str, match, res_pattern);
            if(!match.empty())
            {
                int xml_width  = std::stoi(std::string(match[1].first, match[1].second));
                int xml_height = std::stoi(std::string(match[2].first, match[2].second));
                const QSize res {xml_width, xml_height};

                // find the resolution among the supportedResolutions list
                int i = _supported_resolutions.indexOf(res);
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
        std::regex res_pattern("(\\d*)[Xx](\\d*)");
        std::smatch match;
        std::regex_match(_resolution_xml_option, match, res_pattern);
        if(!match.empty())
        {
            int xml_width  = std::stoi(std::string(match[1].first, match[1].second));
            int xml_height = std::stoi(std::string(match[2].first, match[2].second));
            const QSize res {xml_width, xml_height};

            // find the resolution among the supportedResolutions list
            int i = _supported_resolutions.indexOf(res);
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

} // namespace sight::module::ui::qt::video
