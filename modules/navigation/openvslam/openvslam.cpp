/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include <io/http/Download.hpp>
#include <io/opencv/frame_tl.hpp>

#include <navigation/openvslam/Helper.hpp>

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

static const core::com::slots::key_t ENABLE_LOCALIZATION_SLOT   = "enableLocalization";
static const core::com::slots::key_t ACTIVATE_LOCALIZATION_SLOT = "activateLocalization";

static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::slots::key_t SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT   = "setEnumParameter";

static const core::com::slots::key_t RESET_POINTCLOUD_SLOT = "resetPointCloud";

static const core::com::slots::key_t SAVE_MAP_SLOT = "saveMap";
static const core::com::slots::key_t LOAD_MAP_SLOT = "loadMap";

static const core::com::slots::key_t SAVE_TRAJECTORIES_SLOT = "saveTrajectories";

static const core::com::slots::key_t PAUSE_TRACKER_SLOT = "pauseTracker";

static const core::com::signals::key_t TRACKING_INITIALIZED_SIG     = "trackingInitialized";
static const core::com::signals::key_t TRACKING_NOT_INITIALIZED_SIG = "trackingNotInitialized";
static const core::com::signals::key_t TRACKED_SIG                  = "tracked";
static const core::com::signals::key_t TRACKING_LOST_SIG            = "trackingLost";

static const core::com::signals::key_t VOCFILE_UNLOADED_SIG        = "vocFileUnloaded";
static const core::com::signals::key_t VOCFILE_LOADING_STARTED_SIG = "vocFileLoadingStarted";
static const core::com::signals::key_t VOCFILE_LOADED_SIG          = "vocFileLoaded";

static const core::com::signals::key_t MAP_LOADED_SIG = "mapLoaded";

static const std::string s_DOWNSAMPLE_CONFIG = "downsampleWidth";
static const std::string s_MODE_CONFIG       = "mode";

static std::string s_windowName;

const core::com::slots::key_t INTERNAL_DOWNLOAD_VOC_FILE_SLOT = "InternalDownloadVocFile";

//------------------------------------------------------------------------------

openvslam::openvslam() noexcept :
    notifier(m_signals),
    m_sigTrackingInitialized(new_signal<SignalType>(TRACKING_INITIALIZED_SIG)),
    m_sigTrackingNotInitialized(new_signal<SignalType>(TRACKING_NOT_INITIALIZED_SIG)),
    m_sigTracked(new_signal<SignalType>(TRACKED_SIG)),
    m_sigTrackingLost(new_signal<SignalType>(TRACKING_LOST_SIG)),
    m_sigVocFileUnloaded(new_signal<SignalType>(VOCFILE_UNLOADED_SIG)),
    m_sigVocFileLoadingStarted(new_signal<SignalType>(VOCFILE_LOADING_STARTED_SIG)),
    m_sigVocFileLoaded(new_signal<SignalType>(VOCFILE_LOADED_SIG)),
    m_sigMapLoaded(new_signal<SignalType>(MAP_LOADED_SIG))
{
    new_slot(ENABLE_LOCALIZATION_SLOT, &openvslam::enableLocalization, this);
    new_slot(ACTIVATE_LOCALIZATION_SLOT, &openvslam::activateLocalization, this);

    new_slot(SET_DOUBLE_PARAMETER_SLOT, &openvslam::setDoubleParameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &openvslam::setIntParameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &openvslam::setBoolParameter, this);
    new_slot(SET_ENUM_PARAMETER_SLOT, &openvslam::setEnumParameter, this);

    new_slot(RESET_POINTCLOUD_SLOT, &openvslam::resetPointCloud, this);

    new_slot(SAVE_MAP_SLOT, &openvslam::saveMap, this);
    new_slot(LOAD_MAP_SLOT, &openvslam::loadMap, this);

    new_slot(SAVE_TRAJECTORIES_SLOT, &openvslam::saveTrajectories, this);

    new_slot(PAUSE_TRACKER_SLOT, &openvslam::pause, this);

    new_slot(
        INTERNAL_DOWNLOAD_VOC_FILE_SLOT,
        [this]()
        {
            SIGHT_INFO("Downloading orb_vocab.dbow2...");

            this->notifier::info("Downloading Vocabulary");
            m_sigVocFileLoadingStarted->async_emit();

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

                io::http::downloadFile(url, m_vocabularyPath);
                m_sigVocFileLoaded->async_emit();
            }
            catch(core::exception& _e)
            {
                SIGHT_FATAL("orb_vocab.dbow2 file hasn't been downloaded: " + std::string(_e.what()));
                m_sigVocFileUnloaded->async_emit();
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

void openvslam::configuring()
{
    this->service::tracker::configuring();
    const config_t cfg = this->get_config();

    m_downSampleWidth = cfg.get<std::size_t>(s_DOWNSAMPLE_CONFIG, m_downSampleWidth);
    const std::string mode = cfg.get<std::string>(s_MODE_CONFIG, "MONO");

    // if mode is not set: assuming MONO
    if(mode == "STEREO")
    {
        //TODO: STEREO Mode.
        m_trackingMode = TrackingMode::MONO;
        SIGHT_ERROR("'STEREO' mode is not handle for now. Switching back to 'MONO'");
    }
    else if(mode == "DEPTH")
    {
        //TODO: DEPTH Mode.
        m_trackingMode = TrackingMode::MONO;
        SIGHT_ERROR("'DEPTH' mode is not handle for now. Switching back to 'MONO'");
    }
    else
    {
        // Here mode should be MONO !
        SIGHT_ASSERT("Mode '" + mode + "' is not a valid mode (MONO, STEREO, DEPTH).", mode == "MONO");
        m_trackingMode = TrackingMode::MONO;
    }
}

//------------------------------------------------------------------------------

void openvslam::starting()
{
    const auto& user_path = core::tools::os::get_user_config_dir("openvslam");
    m_vocabularyPath = std::filesystem::path(user_path) / "orb_vocab.dbow2";

    if(!std::filesystem::exists(m_vocabularyPath))
    {
        this->slot(INTERNAL_DOWNLOAD_VOC_FILE_SLOT)->async_run();
    }

    m_sigVocFileLoaded->async_emit();

    // input parameters
    const auto frameTL = m_timeline.lock();
    SIGHT_ASSERT("The input " << s_TIMELINE_INPUT << " is not valid.", frameTL);

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The input " << s_CAMERA_INPUT << " is not valid.", camera);

    auto cameraMatrixTL = m_cameraMatrixTL.lock();

    if(cameraMatrixTL)
    {
        cameraMatrixTL->initPoolSize(50);
    }

    auto pointcloud = std::make_shared<data::mesh>();
    m_pointCloud = pointcloud;

    if(m_trackingMode != TrackingMode::MONO)
    {
        const auto frameTL2 = m_timeline2.lock();
        SIGHT_ASSERT("The input " << s_TIMELINE2_INPUT << " is not valid.", frameTL2);
    }

    m_pointcloudWorker = core::thread::worker::make();

    m_timer = m_pointcloudWorker->create_timer();
    m_timer->set_function([this](){updatePointCloud();});
    m_timer->set_duration(std::chrono::milliseconds(1000)); // update pointcloud every seconds.
}

//------------------------------------------------------------------------------

void openvslam::stopping()
{
    this->stopTracking();
    m_pointCloud = nullptr;

    if(m_showFeatures)
    {
        // Ensure that opencv windows is closed.
        cv::destroyWindow(s_windowName);
    }

    m_pointcloudWorker->stop();
    m_pointcloudWorker.reset();
    m_sigVocFileUnloaded->async_emit();
}

//------------------------------------------------------------------------------

void openvslam::updating()
{
    //Does nothing.
}

//------------------------------------------------------------------------------

void openvslam::startTracking()
{
    this->startTracking("");
}

//------------------------------------------------------------------------------

void openvslam::startTracking(const std::string& _mapFile)
{
    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_slamSystem == nullptr)
    {
        const auto camera = m_camera.lock();
        const auto config = sight::navigation::openvslam::Helper::createMonocularConfig(
            *camera,
            m_orbParameters,
            m_initializerParameters
        );

        m_slamSystem = std::make_unique< ::openvslam::system>(config, m_vocabularyPath);

        m_slamSystem->startup();

        m_ovsMapPublisher   = m_slamSystem->get_map_publisher();
        m_ovsFramePublisher = m_slamSystem->get_frame_publisher();

        SIGHT_ASSERT("Map Publisher shouldn't be null", m_ovsMapPublisher);
        SIGHT_ASSERT("Frame Publisher shouldn't be null", m_ovsFramePublisher);

        if(!_mapFile.empty())
        {
            m_slamSystem->load_map_database(_mapFile);
        }

        // Launch the pointcloud thread.
        if(!m_timer->is_running())
        {
            m_timer->start();
        }

        m_isTracking = true;
    }
}

//------------------------------------------------------------------------------

void openvslam::stopTracking()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_timer->is_running())
    {
        m_timer->set_one_shot(false);
        m_timer->stop();
    }

    if(m_slamSystem)
    {
        // Save if asked, and clear paths.
        if(!m_saveMapPath.empty())
        {
            m_slamSystem->save_map_database(m_saveMapPath);
            m_saveMapPath.clear();
        }

        //cspell: disable
        // Save trajectories at stop.
        if(m_trajectoriesSavePath)
        {
            const std::string folder       = m_trajectoriesSavePath->get_file().remove_filename().string();
            const std::string baseFilename =
                m_trajectoriesSavePath->get_file().filename().replace_extension("").string();

            m_slamSystem->save_frame_trajectory(
                std::string(folder) + "/" + baseFilename + "_frames_traj.txt",
                m_trajectoriesFormat
            );
            m_slamSystem->save_frame_trajectory(
                std::string(folder) + "/" + baseFilename + "_keyframes_traj.txt",
                m_trajectoriesFormat
            );
            m_trajectoriesSavePath.reset();
            m_trajectoriesFormat = "KITTI"; // default format.
        }

        // Wait until the loop BA is finished.
        while(m_slamSystem->loop_BA_is_running())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        m_slamSystem->request_terminate();
        m_slamSystem->shutdown();
        m_slamSystem.reset();
        m_sigTrackingNotInitialized->async_emit();
        m_sigTrackingLost->async_emit();
        m_isTracking   = false;
        m_localization = false;

        m_ovsMapPublisher.reset();
        m_ovsFramePublisher.reset();

        //cspell: enable
    }
}

//------------------------------------------------------------------------------

void openvslam::enableLocalization(bool _enable)
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        m_localization = _enable;
        if(_enable)
        {
            m_slamSystem->disable_mapping_module();
        }
        else
        {
            m_slamSystem->enable_mapping_module();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::activateLocalization()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        m_localization = true;
        m_slamSystem->disable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void openvslam::deactivateLocalization()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        m_localization = false;
        m_slamSystem->enable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void openvslam::setIntParameter(int _val, std::string _key)
{
    if(_key == "nFeatures")
    {
        m_orbParameters.maxNumKeyPts = static_cast<unsigned int>(_val);
    }
    else if(_key == "nLevels")
    {
        m_orbParameters.numLevels = static_cast<unsigned int>(_val);
    }
    else if(_key == "iniThFAST")
    {
        m_orbParameters.iniFastThr = static_cast<unsigned int>(_val);
    }
    else if(_key == "minThFAST")
    {
        m_orbParameters.minFastThr = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.numRansacIterations")
    {
        m_initializerParameters.numRansacIterations = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.minNumTriangulatedPts")
    {
        m_initializerParameters.minNumTriangulatedPts = static_cast<unsigned int>(_val);
    }
    else if(_key == "initializer.numBAIterations")
    {
        m_initializerParameters.numBAIterations = static_cast<unsigned int>(_val);
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::setDoubleParameter(double _val, std::string _key)
{
    if(_key == "scaleFactor")
    {
        m_orbParameters.scaleFactor = static_cast<float>(_val);
    }
    else if(_key == "initializer.parallaxDegThr")
    {
        m_initializerParameters.parallaxDegThr = static_cast<float>(_val);
    }
    else if(_key == "initializer.reprojectionErrThr")
    {
        m_initializerParameters.reprojectionErrThr = static_cast<float>(_val);
    }
    else if(_key == "initializer.scalingFactor")
    {
        m_initializerParameters.scalingFactor = static_cast<float>(_val);
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::setBoolParameter(bool _val, std::string _key)
{
    if(_key == "showFeatures")
    {
        m_showFeatures = _val;
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void openvslam::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "mapType")
    {
        if(_val == "Global")
        {
            m_localMap = false;
        }
        else if(_val == "Local")
        {
            m_localMap = true;
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

void openvslam::loadMap()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle("Select openvslam map file");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("openvlsam map files", "*.map");
    dialogFile.setOption(ui::dialog::location::READ);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());

    if(result)
    {
        m_sigMapLoaded->async_emit();

        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->stopTracking();
        const std::string mapFile = result->get_file().string();
        this->startTracking(mapFile);
    }
}

//------------------------------------------------------------------------------

void openvslam::saveMap()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle("Choose a file to save Openvslam map");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("openvslam files", "*.map");
    dialogFile.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(!result)
    {
        return;
    }

    defaultDirectory->set_folder(result->get_file().parent_path());
    dialogFile.saveDefaultLocation(defaultDirectory);
    m_saveMapPath = result->get_file().string();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_slamSystem)
    {
        //If system is running save now.
        m_slamSystem->save_map_database(m_saveMapPath);
    }
    else
    {
        sight::ui::dialog::message warning;
        warning.setIcon(ui::dialog::message::WARNING);
        warning.setTitle("Save Map offline");
        warning.setMessage(
            "OpenVSLAM is currently offline, map cannot be saved now.\
        file path can be stored and map will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.addButton(ui::dialog::message::Buttons::NO);
        warning.addButton(ui::dialog::message::Buttons::YES);
        warning.setDefaultButton(ui::dialog::message::Buttons::NO);
        const auto answer = warning.show();

        if(answer == sight::ui::dialog::message::Buttons::NO)
        {
            m_saveMapPath.clear();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::saveTrajectories()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFolder;
    dialogFolder.setTitle("Choose a folder & name to save trajectories files.");

    // Use SINGLE_FILE type, so we can use filters, only the basename of files will be used.
    dialogFolder.setType(ui::dialog::location::SINGLE_FILE);
    dialogFolder.setDefaultLocation(defaultDirectory);
    dialogFolder.setOption(ui::dialog::location::WRITE);

    // Use filter to store the format (matrix or vector & quaternions).
    dialogFolder.addFilter("Matrix Format", " KITTI");
    dialogFolder.addFilter("Vector & Quat Format", " TUM");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFolder.show());

    if(!result)
    {
        return;
    }

    m_trajectoriesSavePath = result;
    defaultDirectory->set_folder(result->get_file().remove_filename());
    dialogFolder.saveDefaultLocation(defaultDirectory);
    const std::string trajectories_folder   = result->get_file().remove_filename().string();
    const std::string trajectories_filename = result->get_file().filename().replace_extension("").string(); // keep only
                                                                                                            // the
    // base filename.
    m_trajectoriesFormat = dialogFolder.getSelectedExtensions().front();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    // If openvslam is still alive.
    if(m_slamSystem)
    {
        //cspell: disable
        // Save frame & keyframes trajectory using choosen folder and basename
        m_slamSystem->save_frame_trajectory(
            std::string(trajectories_folder) + "/" + trajectories_filename + "_frames_traj.txt",
            m_trajectoriesFormat
        );
        m_slamSystem->save_frame_trajectory(
            std::string(trajectories_folder) + "/" + trajectories_filename + "_keyframes_traj.txt",
            m_trajectoriesFormat
        );
        //cspell: enable
    }
    // If Openvslam is offline we cannot save trajectories anymore.
    else
    {
        sight::ui::dialog::message warning;
        warning.setIcon(ui::dialog::message::WARNING);
        warning.setTitle("Openvslam is offline");
        warning.setMessage(
            "OpenVSLAM is currently offline, trajectories cannot be saved now.\
        filenames can be stored and trajectories will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.addButton(ui::dialog::message::Buttons::NO);
        warning.addButton(ui::dialog::message::Buttons::YES);
        warning.setDefaultButton(ui::dialog::message::Buttons::NO);
        const auto answer = warning.show();
        if(answer == sight::ui::dialog::message::Buttons::NO)
        {
            m_trajectoriesSavePath.reset();
            m_trajectoriesFormat = "KITTI"; // Default format.
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::pause()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        if(m_isPaused)
        {
            m_slamSystem->resume_tracker();
            m_isPaused = false;
        }
        else
        {
            m_slamSystem->pause_tracker();
            m_isPaused = true;
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::resetPointCloud()
{
    if(m_timer->is_running())
    {
        m_timer->stop();
    }

    auto pointcloud = m_pointCloud.lock();

    // Clear Sight mesh
    pointcloud->clear();
    auto sigMesh = pointcloud->signal<data::object::ModifiedSignalType>
                       (data::object::MODIFIED_SIG);
    sigMesh->async_emit();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    // Clear openvlsam point cloud
    if(m_slamSystem != nullptr)
    {
        m_slamSystem->request_reset();

        m_sigTrackingLost->async_emit();
        m_sigTrackingNotInitialized->async_emit();
    }

    m_timer->start();
}

//------------------------------------------------------------------------------

void openvslam::tracking(core::hires_clock::type& timestamp)
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem && !m_isPaused)
    {
        // Use a lambda expression to scope the lock of timeline and preserve constness of imgLeft.
        const cv::Mat imgLeft = [&]() -> cv::Mat
                                {
                                    const auto frameTL     = m_timeline.lock();
                                    const auto bufferFrame = frameTL->getClosestBuffer(timestamp);
                                    if(bufferFrame == nullptr)
                                    {
                                        // return empty image.
                                        return {};
                                    }

                                    const std::uint8_t* frameData = &bufferFrame->getElement(0);

                                    // this is the main image
                                    return io::opencv::frame_tl::moveToCv(frameTL.get_shared(), frameData);
                                }();

        if(imgLeft.empty())
        {
            return;
        }

        //TODO: downscale image if necessary (scaling issue needs to be resolved.).

        const cv::Mat imgDepth; // this is the depth image (only if DEPTH)

        if(m_trackingMode != TrackingMode::MONO)
        {
            const auto frameTL2     = m_timeline2.lock();
            const auto bufferFrame2 = frameTL2->getClosestBuffer(timestamp);

            if(bufferFrame2 == nullptr)
            {
                return;
            }

            const std::uint8_t* frameData2 = &bufferFrame2->getElement(0);

            cv::Mat imgRight = io::opencv::frame_tl::moveToCv(frameTL2.get_shared(), frameData2);

            // the two frames need to have same size
            if(imgLeft.cols != imgRight.cols || imgLeft.rows != imgRight.rows)
            {
                SIGHT_ERROR("First frame and Second Frame should have the same size");
                return;
            }

            if(m_trackingMode == TrackingMode::DEPTH)
            {
                imgRight.convertTo(imgDepth, CV_32F);
            }
        } // STEREO/DEPTH

        Eigen::Matrix4d pos;
        try
        {
            // The position returned by feed_* function shouldn't be used.
            // since if tracking is lost the position can contain NaN or Inf values.
            m_slamSystem->feed_monocular_frame(imgLeft, timestamp);

            //TODO: STEREO & DEPTH Mode.

            // Use the publisher position instead.
            pos = m_ovsMapPublisher->get_current_cam_pose();

            if(m_showFeatures)
            {
                const auto im = m_ovsFramePublisher->draw_frame();
                s_windowName = this->get_id() + " Openvslam internal frame";
                cv::namedWindow(s_windowName);
                cv::imshow(s_windowName, im);
                //cv::waitKey(1); // FIXME: Linux cannot call cv::waitKey() if this isn't on UI thread.
            }
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Error processing openvslam: " << e.what());
            return;
        }

        const auto floatObj = m_scale.lock();
        float scale         = 1.0F;
        if(floatObj)
        {
            // FIXME : Arbitrary scale, the real scale should be computed with respect to a real object in the 3D Scene.
            if(floatObj->value() > 0.)
            {
                scale = scale / static_cast<float>(floatObj->value());
            }
        }

        //scale needs to be adapted with the downscale ratio, so that map can fit video.
        // m_scale = m_scale * ( m_camera->getWidth() / m_downSampleWidth); //This seems to be wrong...

        if(!pos.isZero())
        {
            auto cameraMatrixTL = m_cameraMatrixTL.lock();
            // fill in the camera position matrix
            if(cameraMatrixTL)
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

                SPTR(data::matrix_tl::BufferType) data = cameraMatrixTL->createBuffer(timestamp);
                data->setElement(matrix, 0);
                cameraMatrixTL->pushObject(data);

                data::timeline::signals::pushed_t::sptr sig;
                sig = cameraMatrixTL->signal<data::timeline::signals::pushed_t>(
                    data::timeline::signals::PUSHED
                );

                sig->async_emit(timestamp);
            }
        }
        else
        {
            // not yet initialized
            m_sigTrackingLost->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void openvslam::updatePointCloud()
{
    // Do not update the pointcloud if localization mode is enabled (no points will be added to openvslam's map),
    // or if tracker is paused.
    auto pointcloud = m_pointCloud.lock();

    if(!m_isPaused && pointcloud.get_shared())
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

        const auto nblandmarks = m_ovsMapPublisher->get_landmarks(landmarks, local_landmarks);

        // Do not update if number of landmarks hasn't changed, of if isn't any landmarks in the map.
        if(m_numberOfLandmarks == nblandmarks || nblandmarks == 0)
        {
            m_sigTrackingLost->async_emit();
            return;
        }

        m_numberOfLandmarks = nblandmarks;

        pointcloud->clear();

        const auto dumpLock = pointcloud->dump_lock();

        unsigned int i = 0;
        if(m_localMap)
        {
            for(auto* const lm : local_landmarks)
            {
                if((lm == nullptr) || lm->will_be_erased())
                {
                    continue;
                }

                const ::openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                pointcloud->pushPoint(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );
                pointcloud->pushCell(i);
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

                pointcloud->pushPoint(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );

                pointcloud->pushCell(i);
                ++i;
            }
        }

        m_sigTrackingInitialized->async_emit();
        m_sigTracked->async_emit();
        auto sigMesh = pointcloud->signal<data::object::ModifiedSignalType>
                           (data::object::MODIFIED_SIG);
        sigMesh->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::navigation::openvslam
