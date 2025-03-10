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

#include "camera.hpp"

#include "modules/ui/qt/video/camera_device_dlg.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/runtime/path.hpp>

#include <data/object.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/preferences.hpp>
#include <ui/qt/container/widget.hpp>

#include <QByteArray>
#include <QCameraDevice>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMediaDevices>
#include <QMessageBox>
#include <QPushButton>

#include <filesystem>

namespace sight::module::ui::qt::video
{

static const core::com::signals::key_t CONFIGURED_CAMERAS_SIG = "configured_cameras";
static const core::com::signals::key_t CONFIGURED_DEVICE_SIG  = "configured_device";
static const core::com::signals::key_t CONFIGURED_FILE_SIG    = "configured_file";
static const core::com::signals::key_t CONFIGURED_STREAM_SIG  = "configured_stream";

static const core::com::slots::key_t CONFIGURE_DEVICE_SLOT = "configureDevice";
static const core::com::slots::key_t CONFIGURE_FILE_SLOT   = "configureFile";
static const core::com::slots::key_t CONFIGURE_STREAM_SLOT = "configureStream";

static const std::string VIDEO_SUPPORT_CONFIG        = "videoSupport";
static const std::string USE_ABSOLUTE_PATH           = "useAbsolutePath";
static const std::string CREATE_CAMERA_NUMBER_CONFIG = "createCameraNumber";
static const std::string LABEL_CONFIG                = "label";
static const std::string RESOLUTION_CONFIG           = "resolution";

const std::string camera::RESOLUTION_PREF_KEY = "camera_resolution";

//------------------------------------------------------------------------------

camera::camera() :
    m_sig_configured_cameras(new_signal<configured_signal_t>(CONFIGURED_CAMERAS_SIG))
{
    new_signal<configured_signal_t>(CONFIGURED_DEVICE_SIG);
    new_signal<configured_signal_t>(CONFIGURED_FILE_SIG);
    new_signal<configured_signal_t>(CONFIGURED_STREAM_SIG);

    new_slot(CONFIGURE_DEVICE_SLOT, &camera::on_choose_device, this);
    new_slot(CONFIGURE_FILE_SLOT, &camera::on_choose_file, this);
    new_slot(CONFIGURE_STREAM_SLOT, &camera::on_choose_stream, this);
}

//------------------------------------------------------------------------------

void camera::configuring()
{
    const service::config_t config = this->get_config();

    m_b_video_support    = config.get<bool>(VIDEO_SUPPORT_CONFIG, false);
    m_use_absolute_path  = config.get<bool>(USE_ABSOLUTE_PATH, false);
    m_num_create_cameras = config.get<std::size_t>(CREATE_CAMERA_NUMBER_CONFIG, m_num_create_cameras);
    m_label              = config.get<std::string>(LABEL_CONFIG, m_label);

    m_resolution = config.get<std::string>(RESOLUTION_CONFIG, "preferences");

    this->initialize();
}

//------------------------------------------------------------------------------

void camera::starting()
{
    this->create();

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    QPointer<QHBoxLayout> layout = new QHBoxLayout();

    if(!m_label.empty())
    {
        const QPointer<QLabel> source_label = new QLabel(QString::fromStdString(m_label));
        layout->addWidget(source_label);
    }

    const QString service_id = QString::fromStdString(base_id());

    m_devices_combo_box = new QComboBox();
    m_devices_combo_box->setObjectName(service_id);
    layout->addWidget(m_devices_combo_box);

    m_devices_combo_box->addItem("Device...", "device");

    // Add video file
    if(m_b_video_support)
    {
        m_devices_combo_box->addItem("File...", "file");
        m_devices_combo_box->addItem("Stream...", "stream");
    }

    // Add button to edit the preferences when and set `m_preferenceMode` to true
    if(m_resolution == "preferences")
    {
        auto path = core::runtime::get_module_resource_path("sight::module::ui::icons");
        // Add preference setting button
        auto* set_pref_button = new QPushButton();
        set_pref_button->setProperty("class", "signal-button");
        set_pref_button->setIcon(QIcon(QString::fromStdString((path / "gear.svg").string())));
        set_pref_button->setToolTip("Set camera resolution preference");
        layout->addWidget(set_pref_button);
        m_preference_mode = true;

        QObject::connect(set_pref_button, &QPushButton::clicked, this, &self_t::set_preference);
    }

    qt_container->set_layout(layout);

    QObject::connect(m_devices_combo_box, qOverload<int>(&QComboBox::activated), this, &camera::on_activated);
    QObject::connect(m_devices_combo_box, qOverload<int>(&QComboBox::currentIndexChanged), this, &camera::on_apply);

    // Create camera data if necessary
    auto camera_set = m_camera_set.lock();
    if(camera_set)
    {
        const std::size_t num_cameras = camera_set->size();
        if(num_cameras == 0)
        {
            SIGHT_ASSERT("No camera data in the CameraSet.", m_num_create_cameras != 0);

            for(std::size_t i = 0 ; i < m_num_create_cameras ; ++i)
            {
                auto camera = std::make_shared<data::camera>();
                camera_set->add_camera(camera);

                const auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(
                    data::camera_set::ADDED_CAMERA_SIG
                );

                sig->async_emit(camera);
            }

            SIGHT_INFO("No camera data in the CameraSet, " << m_num_create_cameras << " will be created.");
        }
        else
        {
            SIGHT_WARN_IF(
                "CameraSet contains camera data but the service is configured to create "
                << m_num_create_cameras << " cameras.",
                m_num_create_cameras != 0
            );
        }
    }
}

//------------------------------------------------------------------------------

void camera::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void camera::updating()
{
}

//------------------------------------------------------------------------------

void camera::on_apply(int _index)
{
    switch(_index)
    {
        case 0:
            this->on_choose_device();
            break;

        case 1:
            this->on_choose_file();
            break;

        case 2:
            this->on_choose_stream();
            break;

        default:
            SIGHT_ASSERT("Invalid index: " << _index, false);
    }
}

//------------------------------------------------------------------------------
void camera::on_activated(int _index)
{
    // If the current index did change, onCurrentIndexChanged will be called, we wouldn't want onApply to be called
    // twice
    if(m_old_index == _index)
    {
        on_apply(_index);
    }

    m_old_index = _index;
}

//------------------------------------------------------------------------------

void camera::on_choose_file()
{
    std::vector<data::camera::sptr> cameras = this->get_cameras();

    // Check preferences
    const std::filesystem::path video_dir_preference_path(sight::ui::preferences().get(
                                                              "VIDEO_DIR_PREF",
                                                              std::string()
    ));

    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("All files", "*.*");
    dialog_file.add_filter("videos", "*.avi *.m4v *.mkv *.mp4 *.ogv");
    dialog_file.add_filter("images", "*.bmp *.jpeg *.jpg *.png *.tiff");
    dialog_file.add_filter("realsense files (*.bag)", "*.bag *.rosbag");
    dialog_file.set_option(sight::ui::dialog::location::read);
    dialog_file.set_option(sight::ui::dialog::location::file_must_exist);

    std::size_t count = 0;
    for(auto& camera : cameras)
    {
        std::filesystem::path video_path;

        if(count == 1 && cameras.size() == 2)
        {
            // Try to guess the second stream path for RGBD cameras
            auto file = cameras[0]->get_video_file();

            if(std::filesystem::is_directory(video_dir_preference_path))
            {
                file = video_dir_preference_path / file;
                file = file.lexically_normal();
            }

            const auto dir = file.parent_path();

            if(!dir.empty())
            {
                const auto parent_dir = dir.parent_path();
                const auto cur_dir    = *(--dir.end());

                const auto find_valid_image_path =
                    [&](std::set<std::string> _folders)
                    {
                        for(const auto& leaf_dir : _folders)
                        {
                            const auto dir = parent_dir / leaf_dir;

                            if(std::filesystem::exists(dir))
                            {
                                std::filesystem::directory_iterator current_entry(dir);
                                std::filesystem::directory_iterator end_entry;
                                while(current_entry != end_entry)
                                {
                                    std::filesystem::path entry_path = *current_entry;
                                    if(entry_path.has_stem())
                                    {
                                        return entry_path;
                                    }

                                    ++current_entry;
                                }
                            }
                        }

                        return std::filesystem::path();
                    };

                static const std::set<std::string> s_DEPTH_FOLDERS = {{"d", "D", "depth", "Depth", "DEPTH"}};
                static const std::set<std::string> s_COLOR_FOLDERS = {{"c", "C", "color", "Color", "COLOR", "RGB"}};

                if(s_DEPTH_FOLDERS.find(cur_dir.string()) != s_DEPTH_FOLDERS.end())
                {
                    video_path = find_valid_image_path(s_COLOR_FOLDERS);
                }
                else if(s_COLOR_FOLDERS.find(cur_dir.string()) != s_COLOR_FOLDERS.end())
                {
                    video_path = find_valid_image_path(s_DEPTH_FOLDERS);
                }
            }
        }

        if(video_path.empty())
        {
            dialog_file.set_title("Choose a file to load for video source #" + std::to_string(count++));

            auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
            if(result)
            {
                default_directory->set_folder(result->get_file().parent_path());
                dialog_file.save_default_location(default_directory);
                video_path = result->get_file();
            }
        }

        if(!video_path.empty())
        {
            if(std::filesystem::is_directory(video_dir_preference_path))
            {
                if(!m_use_absolute_path)
                {
                    const auto video_relative_path = std::filesystem::relative(
                        video_path,
                        video_dir_preference_path
                    );
                    const std::filesystem::path concatenated_path = video_dir_preference_path / video_relative_path;
                    if(std::filesystem::exists(concatenated_path))
                    {
                        video_path = video_relative_path;
                    }
                    else
                    {
                        SIGHT_WARN(
                            "Relative path '" + video_relative_path.string()
                            + "' generated from preferences is not valid."
                        );
                    }
                }
            }
            else
            {
                SIGHT_WARN(
                    "Video directory '" + video_dir_preference_path.string()
                    + "' stored in preference is not valid."
                );
            }

            {
                data::mt::locked_ptr<data::camera> lock(camera);
                camera->set_camera_source(data::camera::file);
                camera->set_video_file(video_path.string());
            }
            const data::camera::modified_signal_t::sptr sig =
                camera->signal<data::camera::modified_signal_t>(data::camera::MODIFIED_SIG);
            sig->async_emit();

            this->signal<configured_signal_t>(CONFIGURED_FILE_SIG)->async_emit();
        }
    }

    m_sig_configured_cameras->async_emit();
}

//------------------------------------------------------------------------------

void camera::on_choose_stream()
{
    std::vector<data::camera::sptr> cameras = this->get_cameras();

    std::size_t count = 0;
    for(auto& camera : cameras)
    {
        sight::ui::dialog::input input;
        input.set_title("Enter stream url for video source #" + std::to_string(count++));

        const auto& [streamSource, ok] = input.get_input();
        if(ok && !streamSource.empty())
        {
            {
                data::mt::locked_ptr<data::camera> lock(camera);
                camera->set_camera_source(data::camera::stream);
                camera->set_stream_url(streamSource);
            }
            const data::camera::modified_signal_t::sptr sig =
                camera->signal<data::camera::modified_signal_t>(data::camera::MODIFIED_SIG);
            sig->async_emit();

            this->signal<configured_signal_t>(CONFIGURED_STREAM_SIG)->async_emit();
        }
    }

    m_sig_configured_cameras->async_emit();
}

//------------------------------------------------------------------------------

void camera::on_choose_device()
{
    if(m_preference_mode)
    {
        try
        {
            sight::ui::preferences resolution_preference;
            m_camera_resolution_preference = resolution_preference.get<std::string>(RESOLUTION_PREF_KEY);
        }
        catch(boost::property_tree::ptree_error&)
        {
            SIGHT_ERROR("Couldn't get preference. The required key doesn't exist.");
        }
        if(!m_camera_resolution_preference.empty())
        {
            m_resolution = m_camera_resolution_preference;
        }
    }

    std::vector<data::camera::sptr> cameras = this->get_cameras();
    const auto& devices                     = QMediaDevices::videoInputs();
    if(devices.isEmpty())
    {
        auto* error_message_box = new QMessageBox(
            QMessageBox::Critical,
            "Error",
            "No device available. Please connect a camera device and relaunch the application."
        );
        error_message_box->exec();
    }
    else
    {
        bool configured   = true;
        std::size_t count = 0;
        for(auto& camera : cameras)
        {
            module::ui::qt::video::camera_device_dlg cam_dialog(m_resolution);
            cam_dialog.setWindowTitle(QString("Camera device selector for video source #%1").arg(count++));

            if(((devices.size() > 1 && m_resolution != "preferences") || m_resolution == "prompt")
               && cam_dialog.exec() != QDialog::Accepted)
            {
                return;
            }

            bool is_selected = false;
            {
                data::mt::locked_ptr<data::camera> lock(camera);

                is_selected = cam_dialog.get_selected_camera(camera, m_resolution);
                configured &= is_selected;
            }

            if(is_selected)
            {
                const data::camera::modified_signal_t::sptr sig =
                    camera->signal<data::camera::modified_signal_t>(data::camera::MODIFIED_SIG);
                sig->async_emit();

                this->signal<configured_signal_t>(CONFIGURED_DEVICE_SIG)->async_emit();
            }
            else if(m_preference_mode)
            {
                [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(
                    this,
                    &camera::set_preference,
                    Qt::QueuedConnection
                );

                SIGHT_ASSERT("The slot `set_preference` was not found.", ok);
            }
        }

        if(configured)
        {
            m_sig_configured_cameras->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

std::vector<data::camera::sptr> camera::get_cameras() const
{
    std::vector<data::camera::sptr> cameras;

    auto camera_set = m_camera_set.lock();
    if(camera_set)
    {
        for(std::size_t i = 0, num_cameras = camera_set->size() ; i < num_cameras ; ++i)
        {
            cameras.push_back(camera_set->get_camera(i));
        }
    }
    else
    {
        const auto camera = m_camera.lock();
        if(camera)
        {
            cameras.push_back(camera.get_shared());
        }
    }

    return cameras;
}

//------------------------------------------------------------------------------

void camera::set_preference()
{
    // set m_resolution to "prompt" mode in order to display the camera selection dialog anytime the button is clicked
    m_resolution      = "prompt";
    m_preference_mode = false;
    this->on_choose_device();
    m_preference_mode = true;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::video
