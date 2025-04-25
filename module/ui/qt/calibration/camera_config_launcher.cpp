/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "module/ui/qt/calibration/camera_config_launcher.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/path.hpp>

#include <data/calibration_info.hpp>
#include <data/camera.hpp>
#include <data/map.hpp>
#include <data/vector.hpp>

#include <io/__/service/io_types.hpp>
#include <io/__/service/reader.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/lexical_cast.hpp>

#include <QHBoxLayout>
#include <QInputDialog>
#include <QStringList>

namespace sight::module::ui::qt::calibration
{

camera_config_launcher::camera_config_launcher() noexcept =
    default;

//------------------------------------------------------------------------------

camera_config_launcher::~camera_config_launcher() noexcept =
    default;

//------------------------------------------------------------------------------

void camera_config_launcher::configuring()
{
    this->initialize();
    service::config_t configuration = this->get_config();

    SIGHT_ASSERT(
        "There must be one (and only one) <config/> element.",
        configuration.count("config") == 1
    );
    const service::config_t& srvconfig = configuration;
    const service::config_t& config    = srvconfig.get_child("config");

    const service::config_t& intrinsic = config.get_child("intrinsic");
    const service::config_t& extrinsic = config.get_child("extrinsic");

    m_intrinsic_launcher.parse_config(intrinsic, this->get_sptr());
    m_extrinsic_launcher.parse_config(extrinsic, this->get_sptr());
}

//------------------------------------------------------------------------------

void camera_config_launcher::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* layout = new QHBoxLayout();

    m_camera_combo_box = new QComboBox();
    layout->addWidget(m_camera_combo_box);

    QIcon add_icon(QString::fromStdString(
                       core::runtime::get_module_resource_file_path(
                           "sight::module::ui::icons",
                           "plus.svg"
                       )
                       .string()
    ));
    m_add_button = new QPushButton(add_icon, "");
    m_add_button->setToolTip("Add a new camera.");
    layout->addWidget(m_add_button);

    QIcon import_icon(QString::fromStdString(
                          core::runtime::get_module_resource_file_path(
                              "sight::module::ui::icons",
                              "import.svg"
                          )
                          .string()
    ));
    m_import_button = new QPushButton(import_icon, "");
    m_import_button->setToolTip("Import an intrinsic calibration.");
    layout->addWidget(m_import_button);

    QIcon remove_icon(QString::fromStdString(
                          core::runtime::get_module_resource_file_path(
                              "sight::module::ui::icons",
                              "minus.svg"
                          )
                          .string()
    ));
    m_remove_button = new QPushButton(remove_icon, "");
    m_remove_button->setToolTip("Remove the camera.");
    layout->addWidget(m_remove_button);

    m_extrinsic_button = new QPushButton("Extrinsic");
    layout->addWidget(m_extrinsic_button);
    m_extrinsic_button->setCheckable(true);

    qt_container->set_layout(layout);

    std::size_t nb_cam = 0;
    {
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT("Missing cameraSet.", camera_set);
        nb_cam = camera_set->size();
    }
    if(nb_cam == 0)
    {
        this->add_camera();

        m_extrinsic_button->setEnabled(false);
        m_remove_button->setEnabled(false);
    }
    else
    {
        for(std::size_t i = 0 ; i < nb_cam ; ++i)
        {
            m_camera_combo_box->addItem(QString("Camera %1").arg(i + 1));
        }

        const bool more_than_one_camera = (nb_cam > 1);

        m_extrinsic_button->setEnabled(more_than_one_camera);
        m_remove_button->setEnabled(more_than_one_camera);

        this->start_intrinsic_config(0);
    }

    QObject::connect(m_camera_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(on_camera_changed(int)));
    QObject::connect(m_add_button, &QPushButton::clicked, this, &self_t::on_add_clicked);
    QObject::connect(m_import_button, &QPushButton::clicked, this, &self_t::on_import_clicked);
    QObject::connect(m_remove_button, &QPushButton::clicked, this, &self_t::on_remove_clicked);
    QObject::connect(m_extrinsic_button, &QPushButton::toggled, this, &self_t::on_extrinsic_toggled);
}

//------------------------------------------------------------------------------

void camera_config_launcher::stopping()
{
    m_intrinsic_launcher.stop_config();
    m_extrinsic_launcher.stop_config();

    this->destroy();
}

//------------------------------------------------------------------------------

void camera_config_launcher::updating()
{
}

//------------------------------------------------------------------------------

void camera_config_launcher::on_camera_changed(int _index)
{
    {
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT(
            "Bad index: " << _index,
            _index >= 0 && static_cast<std::size_t>(_index) < camera_set->size()
        );
    }

    if(_index == 0)
    {
        m_extrinsic_button->setChecked(false);
        m_extrinsic_button->setEnabled(false);
    }
    else
    {
        m_extrinsic_button->setEnabled(true);
    }

    if(m_extrinsic_button->isChecked())
    {
        this->start_extrinsic_config(static_cast<std::size_t>(_index));
    }
    else
    {
        this->start_intrinsic_config(static_cast<std::size_t>(_index));
    }
}

//------------------------------------------------------------------------------

void camera_config_launcher::on_add_clicked()
{
    m_extrinsic_button->setEnabled(true);
    m_remove_button->setEnabled(true);

    this->add_camera();
}

//------------------------------------------------------------------------------

void camera_config_launcher::on_import_clicked()
{
    auto vector = std::make_shared<data::vector>();
    auto reader = sight::service::add<io::service::reader>("sight::module::io::session::reader");
    reader->set_inout(vector, io::service::DATA_KEY);

    try
    {
        service::config_t config;
        config.add("dialog.<xmlattr>.extension", ".cam");
        config.add("dialog.<xmlattr>.description", "Cameras");
        reader->configure(config);

        reader->start();
        reader->open_location_dialog();
        reader->update();
        reader->stop();
    }
    catch(std::exception const& e)
    {
        sight::ui::dialog::message dlg;
        const auto msg = "Cannot read file: " + std::string(e.what());
        dlg.set_title("Read error");
        dlg.set_message(msg);
        dlg.set_icon(sight::ui::dialog::message::icons::critical);
        SIGHT_ERROR(msg);

        throw;
    }

    sight::service::remove(reader);

    QStringList cameras;
    std::map<std::string, data::camera::csptr> camera_map;
    std::size_t n_set = 0;

    for(const auto& object : *vector)
    {
        const auto& camera_set = std::dynamic_pointer_cast<data::camera_set>(object);

        if(camera_set)
        {
            for(std::size_t n_camera = 0, end = camera_set->size() ; n_camera != end ; ++n_camera)
            {
                const auto& camera    = camera_set->get_camera(n_camera);
                const auto& camera_id =
                    camera->get_camera_id() + " [" + std::to_string(n_set) + ", " + std::to_string(n_camera) + "]";

                camera_map.insert(std::make_pair(camera_id, camera));
                cameras << QString::fromStdString(camera_id);
            }

            ++n_set;
        }
    }

    if(n_set == 0)
    {
        sight::ui::dialog::message::show(
            "No CameraSet in file",
            "There are no CameraSet present in the loaded file.",
            sight::ui::dialog::message::critical
        );
    }
    else if(cameras.empty())
    {
        sight::ui::dialog::message::show(
            "No Cameras in file",
            "There are CameraSet present in the loaded CameraSet, but no Cameras were found",
            sight::ui::dialog::message::critical
        );
    }
    else
    {
        auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
        bool ok           = false;
        auto selected     = QInputDialog::getItem(
            qt_container->get_qt_container(),
            "Please select a camera",
            "Camera",
            cameras,
            0,
            false,
            &ok
        );

        if(ok)
        {
            const auto selected_std    = selected.toStdString();
            const auto selected_camera = camera_map[selected_std];
            const auto cam_idx         = m_camera_combo_box->currentIndex();
            const auto camera_set      = m_camera_set.lock();
            auto camera                = camera_set->get_camera(std::size_t(cam_idx));
            camera->deep_copy(selected_camera);
            camera->signal<data::camera::intrinsic_calibrated_signal_t>(
                data::camera::INTRINSIC_CALIBRATED_SIG
            )
            ->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void camera_config_launcher::on_remove_clicked()
{
    const auto index = static_cast<std::size_t>(m_camera_combo_box->currentIndex());
    if(index > 0)
    {
        m_camera_combo_box->blockSignals(true);

        {
            const auto camera_set = m_camera_set.lock();
            // Remove camera
            data::camera::sptr camera = camera_set->get_camera(index);
            camera_set->remove_camera(camera);

            auto sig = camera_set->signal<data::camera_set::removed_camera_signal_t>(
                data::camera_set::REMOVED_CAMERA_SIG
            );
            sig->async_emit(camera);

            // Remove calibrationInfo
            std::string calibration_info_key = "calibrationInfo_" + std::to_string(index);
            const auto activity              = m_activity.lock();
            activity->erase(calibration_info_key);

            const std::size_t nb_cam = camera_set->size();
            if(nb_cam == 1)
            {
                m_extrinsic_button->setEnabled(false);
                m_remove_button->setEnabled(false);
            }

            // Renamed all items from 1 to nbCam
            m_camera_combo_box->clear();
            for(std::size_t i = 0 ; i < nb_cam ; ++i)
            {
                m_camera_combo_box->addItem(QString("Camera %1").arg(i + 1));
            }
        }

        // select first camera
        m_camera_combo_box->setCurrentIndex(0);
        this->start_intrinsic_config(0);
        m_camera_combo_box->blockSignals(false);
    }
    else
    {
        sight::ui::dialog::message::show("Warning", "The first camera can not be deleted");
    }
}

//------------------------------------------------------------------------------

void camera_config_launcher::on_extrinsic_toggled(bool _checked)
{
    std::size_t index = 0;
    {
        const auto camera_set = m_camera_set.lock();
        index = static_cast<std::size_t>(m_camera_combo_box->currentIndex());
        SIGHT_ASSERT("Bad index: " << index, index < camera_set->size());
    }
    if(_checked)
    {
        this->start_extrinsic_config(index);
    }
    else
    {
        this->start_intrinsic_config(index);
    }
}

//------------------------------------------------------------------------------

void camera_config_launcher::start_intrinsic_config(std::size_t _index)
{
    sight::app::field_adaptor_t replace_map;
    {
        const auto camera_set     = m_camera_set.lock();
        data::camera::sptr camera = camera_set->get_camera(_index);

        std::string calibration_info_key = "calibrationInfo_" + std::to_string(_index);

        const auto activity = m_activity.lock();
        auto calib_info     = std::dynamic_pointer_cast<data::calibration_info>((*activity)[calibration_info_key]);

        const auto board_properties = m_board_properties.lock();

        replace_map["camera"]           = camera->get_id();
        replace_map["calibrationInfo"]  = calib_info->get_id();
        replace_map["board_properties"] = board_properties->get_id();
    }

    m_extrinsic_launcher.stop_config();
    m_intrinsic_launcher.stop_config();
    m_intrinsic_launcher.start_config(this->get_sptr(), replace_map);
}

//------------------------------------------------------------------------------

void camera_config_launcher::start_extrinsic_config(std::size_t _index)
{
    sight::app::field_adaptor_t replace_map;
    {
        const std::size_t camera_idx = std::max(_index, std::size_t(1));

        const auto camera_set = m_camera_set.lock();

        data::camera::sptr camera1 = camera_set->get_camera(0);
        data::camera::sptr camera2 = camera_set->get_camera(camera_idx);

        // Check if the two cameras are calibrated
        if(!camera1->get_is_calibrated() || !camera2->get_is_calibrated())
        {
            sight::ui::dialog::message::show("Calibration", "Cameras must be intrinsically calibrated.");
            m_extrinsic_button->setChecked(false);
            return;
        }

        // cspell: ignore Extr
        // Add 2 calibration info in Activity if not exist
        std::string calibration_info1_key = "calibrationInfoExtr0_" + std::to_string(camera_idx);
        std::string calibration_info2_key = "calibrationInfoExtr1_" + std::to_string(camera_idx);
        const auto activity               = m_activity.lock();
        data::calibration_info::sptr calib_info1;
        data::calibration_info::sptr calib_info2;
        // Get the calibrationInfo from the activity if it exists or create it.
        if(activity->find(calibration_info1_key) == activity->end()
           || activity->find(calibration_info2_key) == activity->end())
        {
            calib_info1 = std::make_shared<data::calibration_info>();
            calib_info2 = std::make_shared<data::calibration_info>();

            (*activity)[calibration_info1_key] = calib_info1;
            (*activity)[calibration_info2_key] = calib_info2;
        }
        else
        {
            calib_info1 = std::dynamic_pointer_cast<data::calibration_info>((*activity)[calibration_info1_key]);
            calib_info2 = std::dynamic_pointer_cast<data::calibration_info>((*activity)[calibration_info2_key]);
        }

        const auto board_properties = m_board_properties.lock();

        replace_map["camera1"]          = camera1->get_id();
        replace_map["camera2"]          = camera2->get_id();
        replace_map["calibrationInfo1"] = calib_info1->get_id();
        replace_map["calibrationInfo2"] = calib_info2->get_id();
        replace_map["camIndex"]         = std::to_string(_index);
        replace_map["board_properties"] = board_properties->get_id();
        replace_map["cameraSet"]        = camera_set->get_id();
    }

    m_extrinsic_launcher.stop_config();
    m_intrinsic_launcher.stop_config();
    m_extrinsic_launcher.start_config(this->get_sptr(), replace_map);
}

//------------------------------------------------------------------------------

void camera_config_launcher::add_camera()
{
    std::size_t nb_cam = 0;
    {
        const auto camera_set = m_camera_set.lock();
        nb_cam = camera_set->size();

        data::camera::sptr camera = std::make_shared<data::camera>();

        // Add the CalibrationInfo in activity to be saved in activity
        std::string calibration_info_key        = "calibrationInfo_" + std::to_string(nb_cam);
        data::calibration_info::sptr calib_info = std::make_shared<data::calibration_info>();

        const auto activity = m_activity.lock();
        (*activity)[calibration_info_key] = calib_info;

        // Add the camera
        camera_set->add_camera(camera);
        auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(
            data::camera_set::ADDED_CAMERA_SIG
        );
        sig->async_emit(camera);
    }

    m_camera_combo_box->blockSignals(true);
    m_camera_combo_box->addItem(QString("Camera %1").arg(nb_cam + 1));
    m_camera_combo_box->setCurrentIndex(static_cast<int>(nb_cam));
    m_extrinsic_button->setChecked(false);
    this->start_intrinsic_config(nb_cam);
    m_camera_combo_box->blockSignals(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
