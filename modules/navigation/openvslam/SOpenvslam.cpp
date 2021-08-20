/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "SOpenvslam.hpp"

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>
#include <openvslam/data/landmark.h>
#include <openvslam/feature/orb_params.h>
#include <openvslam/publish/frame_publisher.h>
#include <openvslam/publish/map_publisher.h>
#include <openvslam/system.h>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/Profiling.hpp>
#include <core/runtime/operations.hpp>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <io/opencv/FrameTL.hpp>

#include <navigation/openvslam/Helper.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace sight::module::navigation::openvslam
{

static const core::com::Slots::SlotKeyType s_ENABLE_LOCALIZATION_SLOT   = "enableLocalization";
static const core::com::Slots::SlotKeyType s_ACTIVATE_LOCALIZATION_SLOT = "activateLocalization";

static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT   = "setEnumParameter";

static const core::com::Slots::SlotKeyType s_RESET_POINTCLOUD_SLOT = "resetPointCloud";

static const core::com::Slots::SlotKeyType s_SAVE_MAP_SLOT = "saveMap";
static const core::com::Slots::SlotKeyType s_LOAD_MAP_SLOT = "loadMap";

static const core::com::Slots::SlotKeyType s_SAVE_TRAJECTORIES_SLOT = "saveTrajectories";

static const core::com::Slots::SlotKeyType s_PAUSE_TRACKER_SLOT = "pauseTracker";

static const core::com::Signals::SignalKeyType s_TRACKING_INITIALIZED_SIG     = "trackingInitialized";
static const core::com::Signals::SignalKeyType s_TRACKING_NOT_INITIALIZED_SIG = "trackingNotInitialized";
static const core::com::Signals::SignalKeyType s_TRACKED_SIG                  = "tracked";
static const core::com::Signals::SignalKeyType s_TRACKING_LOST_SIG            = "trackingLost";

static const core::com::Signals::SignalKeyType s_VOCFILE_UNLOADED_SIG        = "vocFileUnloaded";
static const core::com::Signals::SignalKeyType s_VOCFILE_LOADING_STARTED_SIG = "vocFileLoadingStarted";
static const core::com::Signals::SignalKeyType s_VOCFILE_LOADED_SIG          = "vocFileLoaded";

static const core::com::Signals::SignalKeyType s_MAP_LOADED_SIG = "mapLoaded";

static const std::string s_DOWNSAMPLE_CONFIG = "downsampleWidth";
static const std::string s_MODE_CONFIG       = "mode";
static std::string s_windowName;

//------------------------------------------------------------------------------

SOpenvslam::SOpenvslam() noexcept
{
    m_sigTrackingInitialized    = newSignal<SignalType>(s_TRACKING_INITIALIZED_SIG);
    m_sigTrackingNotInitialized = newSignal<SignalType>(s_TRACKING_NOT_INITIALIZED_SIG);

    m_sigTracked      = newSignal<SignalType>(s_TRACKED_SIG);
    m_sigTrackingLost = newSignal<SignalType>(s_TRACKING_LOST_SIG);

    m_sigVocFileUnloaded       = newSignal<SignalType>(s_VOCFILE_UNLOADED_SIG);
    m_sigVocFileLoadingStarted = newSignal<SignalType>(s_VOCFILE_LOADING_STARTED_SIG);
    m_sigVocFileLoaded         = newSignal<SignalType>(s_VOCFILE_LOADED_SIG);

    m_sigMapLoaded = newSignal<SignalType>(s_MAP_LOADED_SIG);

    newSlot(s_ENABLE_LOCALIZATION_SLOT, &SOpenvslam::enableLocalization, this);
    newSlot(s_ACTIVATE_LOCALIZATION_SLOT, &SOpenvslam::activateLocalization, this);

    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SOpenvslam::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SOpenvslam::setIntParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SOpenvslam::setBoolParameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SOpenvslam::setEnumParameter, this);

    newSlot(s_RESET_POINTCLOUD_SLOT, &SOpenvslam::resetPointCloud, this);

    newSlot(s_SAVE_MAP_SLOT, &SOpenvslam::saveMap, this);
    newSlot(s_LOAD_MAP_SLOT, &SOpenvslam::loadMap, this);

    newSlot(s_SAVE_TRAJECTORIES_SLOT, &SOpenvslam::saveTrajectories, this);

    newSlot(s_PAUSE_TRACKER_SLOT, &SOpenvslam::pause, this);

    m_pointcloudWorker = core::thread::Worker::New();

    m_timer = m_pointcloudWorker->createTimer();
    m_timer->setFunction(std::bind(&SOpenvslam::updatePointCloud, this));
    m_timer->setDuration(std::chrono::milliseconds(1000)); // update pointcloud every seconds.
}

//------------------------------------------------------------------------------

SOpenvslam::~SOpenvslam() noexcept
{
    if(this->isStarted())
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::configuring()
{
    this->service::ITracker::configuring();
    const ConfigType cfg = this->getConfigTree();

    m_downSampleWidth = cfg.get<size_t>(s_DOWNSAMPLE_CONFIG, m_downSampleWidth);
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

void SOpenvslam::starting()
{
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

    auto pointcloud = data::Mesh::New();
    m_pointCloud = pointcloud;

    if(m_trackingMode != TrackingMode::MONO)
    {
        const auto frameTL2 = m_timeline2.lock();
        SIGHT_ASSERT("The input " << s_TIMELINE2_INPUT << " is not valid.", frameTL2);
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::stopping()
{
    this->stopTracking();
    m_pointCloud = nullptr;

    if(m_showFeatures)
    {
        // Ensure that opencv windows is closed.
        cv::destroyWindow(s_windowName);
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::updating()
{
    //Does nothing.
}

//------------------------------------------------------------------------------

void SOpenvslam::startTracking()
{
    this->startTracking("");
}

//------------------------------------------------------------------------------

void SOpenvslam::startTracking(const std::string& _mapFile)
{
    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_vocabularyPath.empty())
    {
        m_sigVocFileLoadingStarted->asyncEmit();
        m_vocabularyPath =
            core::runtime::getModuleResourceFilePath(
                "sight::module::navigation::openvslam",
                "orb_vocab.dbow2"
            ).string();
        m_sigVocFileLoaded->asyncEmit();
    }

    if(m_slamSystem == nullptr)
    {
        const auto camera = m_camera.lock();
        const auto config = sight::navigation::openvslam::Helper::createMonocularConfig(
            *camera,
            m_orbParameters,
            m_initializerParameters
        );

        m_slamSystem = std::unique_ptr< ::openvslam::system>(new ::openvslam::system(config, m_vocabularyPath));

        m_slamSystem->startup();

        m_ovsMapPublisher   = m_slamSystem->get_map_publisher();
        m_ovsFramePublisher = m_slamSystem->get_frame_publisher();

        SIGHT_ASSERT("Map Publisher souldn't be null", m_ovsMapPublisher);
        SIGHT_ASSERT("Frame Publisher souldn't be null", m_ovsFramePublisher);

        if(!_mapFile.empty())
        {
            m_slamSystem->load_map_database(_mapFile);
        }

        // Launch the pointcloud thread.
        if(!m_timer->isRunning())
        {
            m_timer->start();
        }

        m_isTracking = true;
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::stopTracking()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_timer->isRunning())
    {
        m_timer->setOneShot(false);
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

        // Save trajectories at stop.
        if(m_trajectoriesSavePath)
        {
            const std::string folder       = m_trajectoriesSavePath->getFile().remove_filename().string();
            const std::string baseFilename =
                m_trajectoriesSavePath->getFile().filename().replace_extension("").string();

            m_slamSystem->save_frame_trajectory(folder + "/" + baseFilename + "_frames_traj.txt", m_trajectoriesFormat);
            m_slamSystem->save_frame_trajectory(
                folder + "/" + baseFilename + "_keyframes_traj.txt",
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
        m_sigTrackingNotInitialized->asyncEmit();
        m_sigTrackingLost->asyncEmit();
        m_isTracking   = false;
        m_localization = false;

        m_ovsMapPublisher.reset();
        m_ovsFramePublisher.reset();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::enableLocalization(bool _enable)
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

void SOpenvslam::activateLocalization()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        m_localization = true;
        m_slamSystem->disable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::deactivateLocalization()
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem)
    {
        m_localization = false;
        m_slamSystem->enable_mapping_module();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::setIntParameter(int _val, std::string _key)
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

void SOpenvslam::setDoubleParameter(double _val, std::string _key)
{
    if(_key == "scaleFactor")
    {
        m_orbParameters.scaleFactor = static_cast<float>(_val);
    }
    else if(_key == "initializer.parallaxDegThr")
    {
        m_initializerParameters.parallaxDegThr = static_cast<float>(_val);
    }
    else if(_key == "initializer.reprojErrThr")
    {
        m_initializerParameters.reprojErrThr = static_cast<float>(_val);
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

void SOpenvslam::setBoolParameter(bool _val, std::string _key)
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

void SOpenvslam::setEnumParameter(std::string _val, std::string _key)
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
            SIGHT_ERROR("Value'" + _val + "' is not handled for key '" + _key + "'");
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::loadMap()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select openvslam map file");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("openvlsam map files", "*.map");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());

    if(result)
    {
        m_sigMapLoaded->asyncEmit();

        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->stopTracking();
        const std::string mapFile = result->getFile().string();
        this->startTracking(mapFile);
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::saveMap()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to save Openvslam map");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("openvslam files", "*.map");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(!result)
    {
        return;
    }

    defaultDirectory->setFolder(result->getFile().parent_path());
    dialogFile.saveDefaultLocation(defaultDirectory);
    m_saveMapPath = result->getFile().string();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_slamSystem)
    {
        //If system is running save now.
        m_slamSystem->save_map_database(m_saveMapPath);
    }
    else
    {
        sight::ui::base::dialog::MessageDialog warning;
        warning.setIcon(ui::base::dialog::IMessageDialog::WARNING);
        warning.setTitle("Save Map offline");
        warning.setMessage(
            "OpenVSLAM is currently offline, map cannot be saved now.\
        file path can be stored and map will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.addButton(ui::base::dialog::IMessageDialog::Buttons::NO);
        warning.addButton(ui::base::dialog::IMessageDialog::Buttons::YES);
        warning.setDefaultButton(ui::base::dialog::IMessageDialog::Buttons::NO);
        const auto answer = warning.show();

        if(answer == sight::ui::base::dialog::IMessageDialog::Buttons::NO)
        {
            m_saveMapPath.clear();
        }
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::saveTrajectories()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFolder;
    dialogFolder.setTitle("Choose a folder & name to save trajectories files.");

    // Use SINGLE_FILE type, so we can use filters, only the basename of files will be used.
    dialogFolder.setType(ui::base::dialog::LocationDialog::SINGLE_FILE);
    dialogFolder.setDefaultLocation(defaultDirectory);
    dialogFolder.setOption(ui::base::dialog::ILocationDialog::WRITE);

    // Use filter to store the format (matrix or vector & quaternions).
    dialogFolder.addFilter("Matrix Format", " KITTI");
    dialogFolder.addFilter("Vector & Quat Format", " TUM");

    auto result = core::location::SingleFile::dynamicCast(dialogFolder.show());

    if(!result)
    {
        return;
    }

    m_trajectoriesSavePath = result;
    defaultDirectory->setFolder(result->getFile().remove_filename());
    dialogFolder.saveDefaultLocation(defaultDirectory);
    const std::string trajFolder   = result->getFile().remove_filename().string();
    const std::string trajFilename = result->getFile().filename().replace_extension("").string(); // keep only the
                                                                                                  // base filename.
    m_trajectoriesFormat = dialogFolder.getCurrentSelection();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    // If openvslam is still alive.
    if(m_slamSystem)
    {
        // Save frame & keyframes trajectory using choosen folder and basename
        m_slamSystem->save_frame_trajectory(trajFolder + "/" + trajFilename + "_frames_traj.txt", m_trajectoriesFormat);
        m_slamSystem->save_frame_trajectory(
            trajFolder + "/" + trajFilename + "_keyframes_traj.txt",
            m_trajectoriesFormat
        );
    }
    // If Openvslam is offline we cannot save trajectories anymore.
    else
    {
        sight::ui::base::dialog::MessageDialog warning;
        warning.setIcon(ui::base::dialog::IMessageDialog::WARNING);
        warning.setTitle("Openvslam is offline");
        warning.setMessage(
            "OpenVSLAM is currently offline, trajectories cannot be saved now.\
        filenames can be stored and trajectories will be automatically saved at next openvlsam stop (start/stop). "
        );
        warning.addButton(ui::base::dialog::IMessageDialog::Buttons::NO);
        warning.addButton(ui::base::dialog::IMessageDialog::Buttons::YES);
        warning.setDefaultButton(ui::base::dialog::IMessageDialog::Buttons::NO);
        const auto answer = warning.show();
        if(answer == sight::ui::base::dialog::IMessageDialog::Buttons::NO)
        {
            m_trajectoriesSavePath.reset();
            m_trajectoriesFormat = "KITTI"; // Default format.
        }
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::pause()
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

void SOpenvslam::resetPointCloud()
{
    if(m_timer->isRunning())
    {
        m_timer->stop();
    }

    auto pointcloud = m_pointCloud.lock();

    // Clear Sight mesh
    pointcloud->clear();
    auto sigMesh = pointcloud->signal<data::Object::ModifiedSignalType>
                       (data::Object::s_MODIFIED_SIG);
    sigMesh->asyncEmit();

    const std::unique_lock<std::mutex> lock(m_slamLock);

    // Clear openvlsam point cloud
    if(m_slamSystem != nullptr)
    {
        m_slamSystem->request_reset();

        m_sigTrackingLost->asyncEmit();
        m_sigTrackingNotInitialized->asyncEmit();
    }

    m_timer->start();
}

//------------------------------------------------------------------------------

void SOpenvslam::tracking(core::HiResClock::HiResClockType& timestamp)
{
    const std::unique_lock<std::mutex> lock(m_slamLock);
    if(m_slamSystem && !m_isPaused)
    {
        // Use a lambda expression to scope the lock of timeline and preserve constness of imgLeft.
        const cv::Mat imgLeft = [&]() -> const cv::Mat
                                {
                                    const auto frameTL     = m_timeline.lock();
                                    const auto bufferFrame = frameTL->getClosestBuffer(timestamp);
                                    if(bufferFrame == nullptr)
                                    {
                                        // return empty image.
                                        return cv::Mat();
                                    }

                                    const std::uint8_t* frameData = &bufferFrame->getElement(0);

                                    // this is the main image
                                    return io::opencv::FrameTL::moveToCv(frameTL.get_shared(), frameData);
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

            cv::Mat imgRight = io::opencv::FrameTL::moveToCv(frameTL2.get_shared(), frameData2);

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
                s_windowName = this->getID() + " Openvslam internal frame";
                cv::namedWindow(s_windowName);
                cv::imshow(s_windowName, im);
                cv::waitKey(1);
            }
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Error processing openvslam: " << e.what());
            return;
        }

        const auto floatObj = m_scale.lock();
        float scale         = 1.0f;
        if(floatObj)
        {
            // FIXME : Arbitrary scale, the real scale should be computed with respect to a real object in the 3D Scene.
            if(floatObj->value() > 0)
            {
                scale = scale / floatObj->value();
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

                float matrix[16];

                for(int i = 0 ; i < 4 ; ++i)
                {
                    for(int j = 0 ; j < 4 ; ++j)
                    {
                        matrix[i * 4 + j] = static_cast<float>(inv(i, j));
                    }
                }

                matrix[3]  *= scale;
                matrix[7]  *= scale;
                matrix[11] *= scale;

                SPTR(data::MatrixTL::BufferType) data = cameraMatrixTL->createBuffer(timestamp);
                data->setElement(matrix, 0);
                cameraMatrixTL->pushObject(data);

                data::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = cameraMatrixTL->signal<data::TimeLine::ObjectPushedSignalType>(
                    data::TimeLine::s_OBJECT_PUSHED_SIG
                );

                sig->asyncEmit(timestamp);
            }
        }
        else
        {
            // not yet initialized
            m_sigTrackingLost->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::loadVocabulary(const std::string& _filePath)
{
    if(_filePath.empty())
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Vocabulary",
            "Vocabulary file : " + _filePath + " can not be loaded.",
            sight::ui::base::dialog::MessageDialog::WARNING
        );
        m_sigVocFileUnloaded->asyncEmit();
    }
    else
    {
        m_vocabularyPath = _filePath;
        m_sigVocFileLoaded->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::updatePointCloud()
{
    // Do not update the pointcloud if localization mode is enabled (no points will be added to openvslam's map),
    // or if tracker is paused.
    auto poincloud = m_pointCloud.lock();
    if(!m_isPaused && poincloud)
    {
        float scale = 1.f;
        {
            const auto s = m_scale.lock();
            if(s->value() > 0)
            {
                scale = scale / s->value();
            }
        }

        std::vector< ::openvslam::data::landmark*> landmarks;
        std::set< ::openvslam::data::landmark*> local_landmarks;

        const auto nblandmarks = m_ovsMapPublisher->get_landmarks(landmarks, local_landmarks);

        // Do not update if number of landmarks hasn't changed, of if isn't any landmarks in the map.
        if(m_numberOfLandmarks == nblandmarks || nblandmarks == 0)
        {
            m_sigTrackingLost->asyncEmit();
            return;
        }

        m_numberOfLandmarks = nblandmarks;

        poincloud->clear();

        const auto dumplLock = poincloud->lock();

        unsigned int i = 0;
        if(m_localMap)
        {
            for(const auto lm : local_landmarks)
            {
                if(!lm || lm->will_be_erased())
                {
                    continue;
                }

                const ::openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                poincloud->pushPoint(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );
                poincloud->pushCell(i);
                ++i;
            }
        }
        else
        {
            for(const auto lm : landmarks)
            {
                if(!lm || lm->will_be_erased())
                {
                    continue;
                }

                const ::openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                poincloud->pushPoint(
                    static_cast<float>(pos_w(0)) * scale,
                    static_cast<float>(pos_w(1)) * scale,
                    static_cast<float>(pos_w(2)) * scale
                );

                poincloud->pushCell(i);
                ++i;
            }
        }

        m_sigTrackingInitialized->asyncEmit();
        m_sigTracked->asyncEmit();
        auto sigMesh = poincloud->signal<data::Object::ModifiedSignalType>
                           (data::Object::s_MODIFIED_SIG);
        sigMesh->asyncEmit();

        std::cout << "updatePointCloud..." << __LINE__ << std::endl;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::navigation::openvslam
