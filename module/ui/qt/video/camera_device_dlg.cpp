/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "camera_device_dlg.hpp"

#include "module/ui/qt/video/formats.hpp"

#include <data/camera.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

#include <QCamera>
#include <QCameraDevice>
#include <QCameraFormat>
#include <QHBoxLayout>
#include <QImageCapture>
#include <QLabel>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMessageBox>
#include <QPushButton>
#include <QtMultimedia>
#include <QVBoxLayout>

#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_map>

Q_DECLARE_METATYPE(QCameraDevice);
Q_DECLARE_METATYPE(QCameraFormat);

namespace sight::module::ui::qt::video
{

//-----------------------------------------------------------------------------

camera_device_dlg::camera_device_dlg(std::string _xml_resolution_config) :
    m_devices_combo_box(new QComboBox()),
    m_cam_settings(new QListWidget())
{
    auto* main_layout     = new QVBoxLayout();
    auto* button_layout   = new QHBoxLayout();
    auto* selector_layout = new QHBoxLayout();
    auto* validate_button = new QPushButton("Validate");
    auto* cancel_button   = new QPushButton("Cancel");

    auto* device_label = new QLabel("Camera device: ");

    selector_layout->addWidget(device_label);
    selector_layout->addWidget(m_devices_combo_box);

    // Detect available devices.
    const auto& devices = QMediaDevices::videoInputs();

    std::map<std::string, QCameraDevice> name_to_uid;
    std::vector<std::string> name_list;

    //We should keep the same order as given by Qt, and uniquely identify cameras with the same name.

    // First run: collect all detected device names and UIDs
    std::size_t index = 0;
    for(const auto& cam_info : devices)
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
        m_devices_combo_box->addItem(QString(device_name.c_str()), QVariant::fromValue(device_info));
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
        main_layout->addWidget(m_cam_settings);
        main_layout->addLayout(button_layout);

        this->setModal(true);
        this->setLayout(main_layout);
        this->setWindowTitle("Camera device selector");
    }

    this->on_select_device(m_devices_combo_box->currentIndex());

    QObject::connect(m_devices_combo_box, SIGNAL(activated(int)), this, SLOT(on_select_device(int)));
    QObject::connect(validate_button, &QPushButton::clicked, this, &camera_device_dlg::accept);
    QObject::connect(cancel_button, &QPushButton::clicked, this, &camera_device_dlg::reject);
}

//-----------------------------------------------------------------------------

bool camera_device_dlg::get_selected_camera(data::camera::sptr& _camera, std::string& _resolution_xml_option)
{
    int index = m_devices_combo_box->currentIndex();
    if(index >= 0)
    {
        const auto cam_info              = qvariant_cast<QCameraDevice>(m_devices_combo_box->itemData(index));
        const auto supported_resolutions = cam_info.photoResolutions();

        [[maybe_unused]] enum data::camera::pixel_format_t format = data::camera::pixel_format_t::invalid;

        QListWidgetItem* item = m_cam_settings->currentItem();

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
            const auto settings = qvariant_cast<QCameraFormat>(item->data(Qt::UserRole));
            _camera->set_maximum_frame_rate(settings.maxFrameRate());

            if((_camera->get_width() != 0 || _camera->get_height() != 0)
               && !(_camera->get_width() == static_cast<size_t>(settings.resolution().width())
                    && _camera->get_height() == static_cast<size_t>(settings.resolution().height()))
               && _camera->get_is_calibrated())
            {
                resolution_warning(
                    _camera->get_width(),
                    _camera->get_height(),
                    settings.resolution().width(),
                    settings.resolution().height()
                );
                return false;
            }

            const auto resolution = settings.resolution();
            _camera->set_width(static_cast<std::size_t>(resolution.width()));
            _camera->set_height(static_cast<std::size_t>(resolution.height()));

            pixel_format_translator_t::left_const_iterator iter;
            iter = pixel_format_translator.left.find(settings.pixelFormat());
            try
            {
                sight::ui::preferences resolution_preference;
                const auto pref_value = std::to_string(settings.resolution().width()) + "x" + std::to_string(
                    settings.resolution().height()
                );
                resolution_preference.put(camera::RESOLUTION_PREF_KEY, pref_value);
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
            const QSize xml_resolution_value = this->get_resolution(_resolution_xml_option, supported_resolutions);
            if(xml_resolution_value.isNull())
            {
                return false;
            }

            _camera->set_maximum_frame_rate(30.F);

            if((_camera->get_width() != 0 || _camera->get_height() != 0)
               && !(_camera->get_width() == static_cast<size_t>(xml_resolution_value.width())
                    && _camera->get_height() == static_cast<size_t>(xml_resolution_value.height()))
               && _camera->get_is_calibrated())
            {
                resolution_warning(
                    _camera->get_width(),
                    _camera->get_height(),
                    xml_resolution_value.width(),
                    xml_resolution_value.height()
                );
                return false;
            }

            _camera->set_height(static_cast<size_t>(xml_resolution_value.height()));
            _camera->set_width(static_cast<size_t>(xml_resolution_value.width()));
        }
        else
        {
            SIGHT_ERROR("No camera setting selected, using default...");
            _camera->set_maximum_frame_rate(30.F);
            _camera->set_height(0);
            _camera->set_width(0);
            return false;
        }

//FIXME : Setting the pixel format generate an error (gstreamer)
#ifndef __linux__
        _camera->set_pixel_format(format);
#endif
        _camera->set_camera_source(data::camera::device);
        _camera->set_camera_id(cam_info.id().toStdString());
        //Use our description.
        _camera->set_description(m_devices_combo_box->currentText().toStdString());
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void camera_device_dlg::on_select_device(int _index)
{
    m_cam_settings->clear();
    if(_index >= 0)
    {
        const auto& cam_info = qvariant_cast<QCameraDevice>(m_devices_combo_box->itemData(_index));

        for(const auto& settings : cam_info.videoFormats())
        {
            auto format = data::camera::pixel_format_t::invalid;

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
            stream << "\t" << settings.maxFrameRate() << " fps";
            stream << "\tFormat:" << data::camera::get_pixel_format_name(format);
            auto* item = new QListWidgetItem(QString::fromStdString(stream.str()));
            item->setData(Qt::UserRole, QVariant::fromValue(settings));
            m_cam_settings->addItem(item);
        }
    }
}

//-----------------------------------------------------------------------------
QSize camera_device_dlg::get_resolution(
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
            auto resolution_preference_str = resolution_preference.get<std::string>(camera::RESOLUTION_PREF_KEY);
            std::regex res_pattern("(\\d*)[Xx](\\d*)");
            std::smatch match;
            std::regex_match(resolution_preference_str, match, res_pattern);
            if(!match.empty())
            {
                int xml_width  = std::stoi(std::string(match[1].first, match[1].second));
                int xml_height = std::stoi(std::string(match[2].first, match[2].second));
                const QSize res {xml_width, xml_height};

                // find the resolution among the supportedResolutions list
                if(_supported_resolutions.indexOf(res) >= 0)
                {
                    return res;
                }

                QMessageBox::critical(
                    this,
                    "Error",
                    "The requested resolution is not supported."
                );
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
            if(_supported_resolutions.indexOf(res) >= 0)
            {
                return res;
            }

            QMessageBox::critical(
                this,
                "Error",
                "The requested resolution is not supported."
            );
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
