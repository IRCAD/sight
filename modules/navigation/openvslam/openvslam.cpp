/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "openvslam.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/profiling.hpp>
#include <core/runtime/path.hpp>
#include <core/tools/os.hpp>

#include <io/http/download.hpp>
#include <io/opencv/frame_tl.hpp>

#include <navigation/openvslam/helper.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>
#include <openvslam/data/landmark.h>
#include <openvslam/feature/orb_params.h>
#include <openvslam/publish/frame_publisher.h>
#include <openvslam/publish/map_publisher.h>
#include <openvslam/system.h>

#include <memory>

namespace sight::module::navigation::openvslam
{

static const core::com::slots::key_t ENABLE_LOCALIZATION_SLOT   = "enable_localization";
static const core::com::slots::key_t ACTIVATE_LOCALIZATION_SLOT = "activateLocalization";

static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT = "set_double_parameter";
static const core::com::slots::key_t SET_INT_PARAMETER_SLOT    = "set_int_parameter";
static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT   = "set_bool_parameter";
static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT   = "set_enum_parameter";

static const core::com::slots::key_t RESET_POINTCLOUD_SLOT = "reset_point_cloud";

static const core::com::slots::key_t SAVE_MAP_SLOT = "save_map";
static const core::com::slots::key_t LOAD_MAP_SLOT = "load_map";

static const core::com::slots::key_t SAVE_TRAJECTORIES_SLOT = "save_trajectories";

static const core::com::slots::key_t PAUSE_TRACKER_SLOT = "pause_tracker";

static const core::com::signals::key_t TRACKING_INITIALIZED_SIG     = "tracking_initialized";
static const core::com::signals::key_t TRACKING_NOT_INITIALIZED_SIG = "trackingNotInitialized";
static const core::com::signals::key_t TRACKED_SIG                  = "tracked";
static const core::com::signals::key_t TRACKING_LOST_SIG            = "tracking_lost";

static const core::com::signals::key_t VOCFILE_UNLOADED_SIG        = "voc_file_unloaded";
static const core::com::signals::key_t VOCFILE_LOADING_STARTED_SIG = "voc_file_loading_started";
static const core::com::signals::key_t VOCFILE_LOADED_SIG          = "voc_file_loaded";

static const core::com::signals::key_t MAP_LOADED_SIG = "map_loaded";

static const std::string DOWNSAMPLE_CONFIG = "downsampleWidth";
static const std::string MODE_CONFIG       = "mode";

static std::string s_window_name;

const core::com::slots::key_t INTERNAL_DOWNLOAD_VOC_FILE_SLOT = "InternalDownloadVocFile";

//------------------------------------------------------------------------------

openvslam::openvslam() noexcept :
    notifier(m_signals),
    m_sig_tracking_initialized(new_signal<signal_t>(TRACKING_INITIALIZED_SIG)),
    m_sig_tracking_not_initialized(new_signal<signal_t>(TRACKING_NOT_INITIALIZED_SIG)),
    m_sig_tracked(new_signal<signal_t>(TRACKED_SIG)),
    m_sig_tracking_lost(new_signal<signal_t>(TRACKING_LOST_SIG)),
    m_sig_voc_file_unloaded(new_signal<signal_t>(VOCFILE_UNLOADED_SIG)),
    m_sig_voc_file_loading_started(new_signal<signal_t>(VOCFILE_LOADING_STARTED_SIG)),
    m_sig_voc_file_loaded(new_signal<signal_t>(VOCFILE_LOADED_SIG)),
    m_sig_map_loaded(new_signal<signal_t>(MAP_LOADED_SIG))
{
    new_slot(ENABLE_LOCALIZATION_SLOT, &openvslam::enable_localization, this);
    new_slot(ACTIVATE_LOCALIZATION_SLOT, &openvslam::activate_localization, this);

    new_slot(SET_DOUBLE_PARAMETER_SLOT, &openvslam::set_double_parameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &openvslam::set_int_parameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &openvslam::set_bool_parameter, this);
    new_slot(SET_ENUM_PARAMETER_SLOT, &openvslam::set_enum_parameter, this);

    new_slot(RESET_POINTCLOUD_SLOT, &openvslam::reset_point_cloud, this);

    new_slot(SAVE_MAP_SLOT, &openvslam::save_map, this);
    new_slot(LOAD_MAP_SLOT, &openvslam::load_map, this);

    new_slot(SAVE_TRAJECTORIES_SLOT, &openvslam::save_trajectories, this);

    new_slot(PAUSE_TRACKER_SLOT, &openvslam::pause, this);

    new_slot(
        INTERNAL_DOWNLOAD_VOC_FILE_SLOT,
        [this]()
        {
            SIGHT_INFO("Downloading orb_vocab.dbow2...");

            this->notifier::info("Downloading Vocabulary");
            m_sig_voc_file_loading_started->async_emit();

            try
            {
                std::string url;
                // Check first if the ENV SIGHT_OPENVSLAM_VOC_URL is set.
                const std::string env_download_url = core::tools::os::get_env("SIGHT_OPENVSLAM_VOC_URL");

                if(env_download_url.empty())
                {
                    url = "https://cloud.ircad.fr/s/2QxjHtxNpjXDAqK/download";
                }
                else
                {
                    url = env_download_url;
                }

                io::http::download_file(url, m_vocabulary_path);
                m_sig_voc_file_loaded->async_emit();
            }
            catch(core::exception& e)
            {
                SIGHT_FATAL("orb_vocab.dbow2 file hasn't been downloaded: " + std::string(e.what()));
                m_sig_voc_file_unloaded->async_emit();
            }

            this->notifier::success("Vocabulary downloaded");
        });
}

//------------------------------------------------------------------------------

openvslam::~openvslam() noexcept
{
    if(this->started())
    {
        this->stop().wait();
    }
}

//------------------------------------------------------------------------------

void openvslam::configuring(const config_t& _config)
{
    this->service::tracker::configuring(_config);

    m_down_sample_width = _config.get<std::size_t>(DOWNSAMPLE_CONFIG, m_down_sample_width);
    const std::string mode = _config.get<std::string>(MODE_CONFIG, "MONO");

    // if mode is not set: assuming MONO
    if(mode == "STEREO")
    {
        //TODO: STEREO Mode.
        m_tracking_mode = tracking_mode::mono;
        SIGHT_ERROR("'STEREO' mode is not handle for now. Switching back to 'MONO'");
    }
    else if(mode == "DEPTH")
    {
        //TODO: DEPTH Mode.
        m_tracking_mode = tracking_mode::mono;
        SIGHT_ERROR("'DEPTH' mode is not handle for now. Switching back to 'MONO'");
    }
    else
    {
        // Here mode should be MONO !
        SIGHT_ASSERT("Mode '" + mode + "' is not a valid mode (MONO, STEREO, DEPTH).", mode == "MONO");
        m_tracking_mode = tracking_mode::mono;
    }
}

//------------------------------------------------------------------------------

void openvslam::starting()
{
    const auto& user_path = core::tools::os::get_user_config_dir("openvslam");
    m_vocabulary_path = std::filesystem::path(user_path) / "orb_vocab.dbow2";

    if(!std::filesystem::exists(m_vocabulary_path))
    {
        this->slot(INTERNAL_DOWNLOAD_VOC_FILE_SLOT)->async_run();
    }

    m_sig_voc_file_loaded->async_emit();

    // input parameters
    const auto frame_tl = m_timeline.lock();
    SIGHT_ASSERT("The input " << TIMELINE_INPUT << " is not valid.", frame_tl);

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The input " << CAMERA_INPUT << " is not valid.", camera);

    auto camera_matrix_tl = m_camera_matrix_tl.lock();

    if(camera_matrix_tl)
    {
        camera_matrix_tl->init_pool_size(50);
    }

    auto pointcloud = std::make_shared<data::mesh>();
    m_point_cloud = pointcloud;

    if(m_tracking_mode != tracking_mode::mono)
    {
        const auto frame_tl_2 = m_timeline2.lock();
        SIGHT_ASSERT("The input " << TIMELIN_E2_INPUT << " is not valid.", frame_tl_2);
    }

    m_pointcloud_worker = core::thread::worker::make();

    m_timer = m_pointcloud_worker->create_timer();
    m_timer->set_function([this](){update_point_cloud();});
    m_timer->set_duration(std::chrono::milliseconds(1000)); // update pointcloud every seconds.
}

//------------------------------------------------------------------------------

void openvslam::stopping()
{
    this->stop_tracking();
    m_point_cloud = nullptr;

    if(m_show_features)
    {
        // Ensure that opencv windows is closed.
        cv::destroyWindow(s_window_name);
    }

    m_pointcloud_worker->stop();
    m_pointcloud_worker.reset();
    m_sig_voc_file_unloaded->async_emit();
}

//------------------------------------------------------------------------------

void openvslam::updating()
{
    //Does nothing.
}

//------------------------------------------------------------------------------

void openvslam::start_tracking()
{
    this->start_tracking("");
}

//------------------------------------------------------------------------------

void openvslam::start_tracking(const std::string& _map_file)
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);

    if(m_slam_system == nullptr)
    {
        const auto camera = m_camera.lock();
        const auto config = sight::navigation::openvslam::helper::create_monocular_config(
            *camera,
            m_orb_parameters,
            m_initializer_parameters
        );

        m_slam_system = std::make_unique< ::openvslam::system>(config, m_vocabulary_path);

        m_slam_system->startup();

        m_ovs_map_publisher   = m_slam_system->get_map_publisher();
        m_ovs_frame_publisher = m_slam_system->get_frame_publisher();

        SIGHT_ASSERT("Map Publisher shouldn't be null", m_ovs_map_publisher);
        SIGHT_ASSERT("Frame Publisher shouldn't be null", m_ovs_frame_publisher);

        if(!_map_file.empty())
        {
            m_slam_system->load_map_database(_map_file);
        }

        // Launch the pointcloud thread.
        if(!m_timer->is_running())
        {
            m_timer->start();
        }

        m_is_tracking = true;
    }
}

//------------------------------------------------------------------------------

void openvslam::stop_tracking()
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);

    if(m_timer->is_running())
    {
        m_timer->set_one_shot(false);
        m_timer->stop();
    }

    if(m_slam_system)
    {
        // Save if asked, and clear paths.
        if(!m_save_map_path.empty())
        {
            m_slam_system->save_map_database(m_save_map_path);
            m_save_map_path.clear();
        }

        //cspell: disable
        // Save trajectories at stop.
        if(m_trajectories_save_path)
        {
            const std::string folder        = m_trajectories_save_path->get_file().remove_filename().string();
            const std::string base_filename =
                m_trajectories_save_path->get_file().filename().replace_extension("").string();

            m_slam_system->save_frame_trajectory(
                std::string(folder) + "/" + base_filename + "_frames_traj.txt",
                m_trajectories_format
            );
            m_slam_system->save_frame_trajectory(
                std::string(folder) + "/" + base_filename + "_keyframes_traj.txt",
                m_trajectories_format
            );
            m_trajectories_save_path.reset();
            m_trajectories_format = "KITTI"; // default format.
        }

        // Wait until the loop BA is finished.
        while(m_slam_system->loop_BA_is_running())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        m_slam_system->request_terminate();
        m_slam_system->shutdown();
        m_slam_system.reset();
        m_sig_tracking_not_initialized->async_emit();
        m_sig_tracking_lost->async_emit();
        m_is_tracking  = false;
        m_localization = false;

        m_ovs_map_publisher.reset();
        m_ovs_frame_publisher.reset();

        //cspell: enable
    }
}

//------------------------------------------------------------------------------

void openvslam::enable_localization(bool _enable)
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);
    if(m_slam_system)
    {
        m_localization = _enable;
        if(_enable)
        {
            m_slam_system->disable_mapping_module();
        }
        else
        {
            m_slam_system->enable_mapping_module();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::activate_localization()
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);
    if(m_slam_system)
    {
        m_localization = true;
        m_slam_system->disable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void openvslam::deactivate_localization()
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);
    if(m_slam_system)
    {
        m_localization = false;
        m_slam_system->enable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void openvslam::set_int_parameter(int _val, std::string _key)
{
    if(_key == "nFeatures")
    {
        m_orb_parameters.max_num_key_pts = static_cast<unsigned int>(_val);
    }
    else if(_key == "nLevels")
    {
        m_orb_parameters.num_levels = static_cast<unsigned int>(_val);
    }
    else if(_key == "iniThFAST")
    {
        m_orb_parameters.ini_fast_thr = static_cast<unsigned int>(_val);
    }
    else if(_key == "minThFAST")
    {
        m_orb_parameters.min_fast_thr = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.numRansacIterations")
    {
        m_initializer_parameters.num_ransac_iterations = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.minNumTriangulatedPts")
    {
        m_initializer_parameters.min_num_triangulated_pts = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.numBAIterations")
    {
        m_initializer_parameters.num_ba_iterations = static_cast<unsigned int>(_val);
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::set_double_parameter(double _val, std::string _key)
{
    if(_key == "scaleFactor")
    {
        m_orb_parameters.scale_factor = static_cast<float>(_val);
    }
    else if(_key == "initializer.parallaxDegThr")
    {
        m_initializer_parameters.parallax_deg_thr = static_cast<float>(_val);
    }
    else if(_key == "initializer.reprojectionErrThr")
    {
        m_initializer_parameters.reprojection_err_thr = static_cast<float>(_val);
    }
    else if(_key == "initializer.scalingFactor")
    {
        m_initializer_parameters.scaling_factor = static_cast<float>(_val);
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::set_bool_parameter(bool _val, std::string _key)
{
    if(_key == "showFeatures")
    {
        m_show_features = _val;
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::set_enum_parameter(std::string _val, std::string _key)
{
    if(_key == "mapType")
    {
        if(_val == "Global")
        {
            m_local_map = false;
        }
        else if(_val == "Local")
        {
            m_local_map = true;
        }
        else
        {
            SIGHT_ERROR(std::string("Value'") + _val + "' is not handled for key '" + _key + "'");
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::load_map()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title("Select openvslam map file");
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("openvlsam map files", "*.map");
    dialog_file.set_option(ui::dialog::location::read);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());

    if(result)
    {
        m_sig_map_loaded->async_emit();

        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->stop_tracking();
        const std::string map_file = result->get_file().string();
        this->start_tracking(map_file);
    }
}

//------------------------------------------------------------------------------

void openvslam::save_map()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title("Choose a file to save Openvslam map");
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("openvslam files", "*.map");
    dialog_file.set_option(ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(!result)
    {
        return;
    }

    default_directory->set_folder(result->get_file().parent_path());
    dialog_file.save_default_location(default_directory);
    m_save_map_path = result->get_file().string();

    const std::unique_lock<std::mutex> lock(m_slam_lock);

    if(m_slam_system)
    {
        //If system is running save now.
        m_slam_system->save_map_database(m_save_map_path);
    }
    else
    {
        sight::ui::dialog::message warning;
        warning.set_icon(ui::dialog::message::warning);
        warning.set_title("Save Map offline");
        warning.set_message(
            "OpenVSLAM is currently offline, map cannot be saved now.\
        file path can be stored and map will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.add_button(ui::dialog::message::buttons::no);
        warning.add_button(ui::dialog::message::buttons::yes);
        warning.set_default_button(ui::dialog::message::buttons::no);
        const auto answer = warning.show();

        if(answer == sight::ui::dialog::message::buttons::no)
        {
            m_save_map_path.clear();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::save_trajectories()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_folder;
    dialog_folder.set_title("Choose a folder & name to save trajectories files.");

    // Use SINGLE_FILE type, so we can use filters, only the basename of files will be used.
    dialog_folder.set_type(ui::dialog::location::single_file);
    dialog_folder.set_default_location(default_directory);
    dialog_folder.set_option(ui::dialog::location::write);

    // Use filter to store the format (matrix or vector & quaternions).
    dialog_folder.add_filter("Matrix Format", " KITTI");
    dialog_folder.add_filter("Vector & Quat Format", " TUM");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_folder.show());

    if(!result)
    {
        return;
    }

    m_trajectories_save_path = result;
    default_directory->set_folder(result->get_file().remove_filename());
    dialog_folder.save_default_location(default_directory);
    const std::string trajectories_folder   = result->get_file().remove_filename().string();
    const std::string trajectories_filename = result->get_file().filename().replace_extension("").string(); // keep only
                                                                                                            // the
    // base filename.
    m_trajectories_format = dialog_folder.get_selected_extensions().front();

    const std::unique_lock<std::mutex> lock(m_slam_lock);

    // If openvslam is still alive.
    if(m_slam_system)
    {
        //cspell: disable
        // Save frame & keyframes trajectory using choosen folder and basename
        m_slam_system->save_frame_trajectory(
            std::string(trajectories_folder) + "/" + trajectories_filename + "_frames_traj.txt",
            m_trajectories_format
        );
        m_slam_system->save_frame_trajectory(
            std::string(trajectories_folder) + "/" + trajectories_filename + "_keyframes_traj.txt",
            m_trajectories_format
        );
        //cspell: enable
    }
    // If Openvslam is offline we cannot save trajectories anymore.
    else
    {
        sight::ui::dialog::message warning;
        warning.set_icon(ui::dialog::message::warning);
        warning.set_title("Openvslam is offline");
        warning.set_message(
            "OpenVSLAM is currently offline, trajectories cannot be saved now.\
        filenames can be stored and trajectories will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.add_button(ui::dialog::message::buttons::no);
        warning.add_button(ui::dialog::message::buttons::yes);
        warning.set_default_button(ui::dialog::message::buttons::no);
        const auto answer = warning.show();
        if(answer == sight::ui::dialog::message::buttons::no)
        {
            m_trajectories_save_path.reset();
            m_trajectories_format = "KITTI"; // Default format.
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::pause()
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);
    if(m_slam_system)
    {
        if(m_is_paused)
        {
            m_slam_system->resume_tracker();
            m_is_paused = false;
        }
        else
        {
            m_slam_system->pause_tracker();
            m_is_paused = true;
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::reset_point_cloud()
{
    if(m_timer->is_running())
    {
        m_timer->stop();
    }

    auto pointcloud = m_point_cloud.lock();

    // Clear Sight mesh
    pointcloud->clear();
    auto sig_mesh = pointcloud->signal<data::object::modified_signal_t>
                        (data::object::MODIFIED_SIG);
    sig_mesh->async_emit();

    const std::unique_lock<std::mutex> lock(m_slam_lock);

    // Clear openvlsam point cloud
    if(m_slam_system != nullptr)
    {
        m_slam_system->request_reset();

        m_sig_tracking_lost->async_emit();
        m_sig_tracking_not_initialized->async_emit();
    }

    m_timer->start();
}

//------------------------------------------------------------------------------

void openvslam::tracking(core::clock::type& _timestamp)
{
    const std::unique_lock<std::mutex> lock(m_slam_lock);
    if(m_slam_system && !m_is_paused)
    {
        // Use a lambda expression to scope the lock of timeline and preserve constness of imgLeft.
        const cv::Mat img_left = [&]() -> cv::Mat
                                 {
                                     const auto frame_tl     = m_timeline.lock();
                                     const auto buffer_frame = frame_tl->get_closest_buffer(_timestamp);
                                     if(buffer_frame == nullptr)
                                     {
                                         // return empty image.
                                         return {};
                                     }

                                     const std::uint8_t* frame_data = &buffer_frame->get_element(0);

                                     // this is the main image
                                     return io::opencv::frame_tl::move_to_cv(frame_tl.get_shared(), frame_data);
                                 }();

        if(img_left.empty())
        {
            return;
        }

        //TODO: downscale image if necessary (scaling issue needs to be resolved.).

        const cv::Mat img_depth; // this is the depth image (only if DEPTH)

        if(m_tracking_mode != tracking_mode::mono)
        {
            const auto frame_tl_2    = m_timeline2.lock();
            const auto buffer_frame2 = frame_tl_2->get_closest_buffer(_timestamp);

            if(buffer_frame2 == nullptr)
            {
                return;
            }

            const std::uint8_t* frame_data2 = &buffer_frame2->get_element(0);

            cv::Mat img_right = io::opencv::frame_tl::move_to_cv(frame_tl_2.get_shared(), frame_data2);

            // the two frames need to have same size
            if(img_left.cols != img_right.cols || img_left.rows != img_right.rows)
            {
                SIGHT_ERROR("First frame and Second Frame should have the same size");
                return;
            }

            if(m_tracking_mode == tracking_mode::depth)
            {
                img_right.convertTo(img_depth, CV_32F);
            }
        } // STEREO/DEPTH

        Eigen::Matrix4d pos;
        try
        {
            // The position returned by feed_* function shouldn't be used.
            // since if tracking is lost the position can contain NaN or Inf values.
            m_slam_system->feed_monocular_frame(img_left, _timestamp);

            //TODO: STEREO & DEPTH Mode.

            // Use the publisher position instead.
            pos = m_ovs_map_publisher->get_current_cam_pose();

            if(m_show_features)
            {
                const auto im = m_ovs_frame_publisher->draw_frame();
                s_window_name = this->get_id() + " Openvslam internal frame";
                cv::namedWindow(s_window_name);
                cv::imshow(s_window_name, im);
                //cv::waitKey(1); // FIXME: Linux cannot call cv::waitKey() if this isn't on UI thread.
            }
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Error processing openvslam: " << e.what());
            return;
        }

        const auto float_obj = m_scale.lock();
        float scale          = 1.0F;
        if(float_obj)
        {
            // FIXME : Arbitrary scale, the real scale should be computed with respect to a real object in the 3D scene.
            if(float_obj->value() > 0.)
            {
                scale = scale / static_cast<float>(float_obj->value());
            }
        }

        //scale needs to be adapted with the downscale ratio, so that map can fit video.
        // m_scale = m_scale * ( m_camera->getWidth() / m_downSampleWidth); //This seems to be wrong...

        if(!pos.isZero())
        {
            auto camera_matrix_tl = m_camera_matrix_tl.lock();
            // fill in the camera position matrix
            if(camera_matrix_tl)
            {
                const auto inv = pos.inverse();

                std::array<float, 16> matrix {};

                for(std::size_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::size_t j = 0 ; j < 4 ; ++j)
                    {
                        matrix[i * 4 + j] = static_cast<float>(inv(std::int64_t(i), std::int64_t(j)));
                    }
                }

                matrix[3]  *= scale;
                matrix[7]  *= scale;
                matrix[11] *= scale;

                SPTR(data::matrix_tl::buffer_t) data = camera_matrix_tl->create_buffer(_timestamp);
                data->set_element(matrix, 0);
                camera_matrix_tl->push_object(data);

                data::timeline::signals::pushed_t::sptr sig;
                sig = camera_matrix_tl->signal<data::timeline::signals::pushed_t>(
                    data::timeline::signals::PUSHED
                );

                sig->async_emit(_timestamp);
            }
        }
        else
        {
            // not yet initialized
            m_sig_tracking_lost->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::update_point_cloud()
{
    // Do not update the pointcloud if localization mode is enabled (no points will be added to openvslam's map),
    // or if tracker is paused.
    auto pointcloud = m_point_cloud.lock();

    if(!m_is_paused && pointcloud.get_shared())
    {
        float scale = 1.F;
        {
            const auto s = m_scale.lock();
            if(s && s->value() > 0.)
            {
                scale = scale / static_cast<float>(s->value());
            }
        }

        std::vector< ::openvslam::data::landmark*> landmarks;
        std::set< ::openvslam::data::landmark*> local_landmarks;

        const auto nblandmarks = m_ovs_map_publisher->get_landmarks(landmarks, local_landmarks);

        // Do not update if number of landmarks hasn't changed, of if isn't any landmarks in the map.
        if(m_number_of_landmarks == nblandmarks || nblandmarks == 0)
        {
            m_sig_tracking_lost->async_emit();
            return;
        }

        m_number_of_landmarks = nblandmarks;

        pointcloud->clear();

        const auto dump_lock = pointcloud->dump_lock();

        unsigned int i = 0;
        if(m_local_map)
        {
            for(auto* const lm : local_landmarks)
            {
                if((lm == nullptr) || lm->will_be_erased())
                {
                    continue;
                }

                const ::openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                pointcloud->push_point(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );
                pointcloud->push_cell(i);
                ++i;
            }
        }
        else
        {
            for(auto* const lm : landmarks)
            {
                if((lm == nullptr) || lm->will_be_erased())
                {
                    continue;
                }

                const ::openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                pointcloud->push_point(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );

                pointcloud->push_cell(i);
                ++i;
            }
        }

        m_sig_tracking_initialized->async_emit();
        m_sig_tracked->async_emit();
        auto sig_mesh = pointcloud->signal<data::object::modified_signal_t>
                            (data::object::MODIFIED_SIG);
        sig_mesh->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::navigation::openvslam
