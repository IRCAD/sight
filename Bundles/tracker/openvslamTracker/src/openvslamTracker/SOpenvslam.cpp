/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "openvslamTracker/SOpenvslam.hpp"

#include <cvIO/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <openvslamIO/Helper.hpp>

#include <boost/filesystem/operations.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <openvslam/camera/perspective.h>
#include <openvslam/config.h>
#include <openvslam/data/landmark.h>
#include <openvslam/feature/orb_params.h>
#include <openvslam/publish/frame_publisher.h>
#include <openvslam/publish/map_publisher.h>
#include <openvslam/system.h>

namespace openvslamTracker
{

fwServicesRegisterMacro( ::arServices::ITracker, ::openvslamTracker::SOpenvslam);

const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_ENABLE_LOCALIZATION_SLOT   = "enableLocalization";
const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_ACTIVATE_LOCALIZATION_SLOT = "activateLocalization";

const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_SET_INT_PARAMETER_SLOT    = "setIntParameter";

const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_RESET_POINTCLOUD_SLOT = "resetPointCloud";

const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_SAVE_MAP_SLOT = "saveMap";
const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_LOAD_MAP_SLOT = "loadMap";

const ::fwCom::Slots::SlotKeyType openvslamTracker::SOpenvslam::s_PAUSE_TRACKER = "pauseTracker";

static const ::fwCom::Signals::SignalKeyType s_TRACKING_INITIALIZED     = "trackingInitialized";
static const ::fwCom::Signals::SignalKeyType s_TRACKING_NOT_INITIALIZED = "trackingNotInitialized";
static const ::fwCom::Signals::SignalKeyType s_TRACKED                  = "tracked";
static const ::fwCom::Signals::SignalKeyType s_TRACKING_LOST            = "trackingLost";

static const ::fwCom::Signals::SignalKeyType s_VOCFILE_UNLOADED        = "vocFileUnloaded";
static const ::fwCom::Signals::SignalKeyType s_VOCFILE_LOADING_STARTED = "vocFileLoadingStarted";
static const ::fwCom::Signals::SignalKeyType s_VOCFILE_LOADED          = "vocFileLoaded";

static const ::fwCom::Signals::SignalKeyType s_MAP_LOADED = "mapLoaded";

static const ::fwServices::IService::KeyType s_VIDEOPOINTS_INPUT     = "videoPoint";
static const ::fwServices::IService::KeyType s_CAMERA_MATRIXTL_INOUT = "cameraMatrixTL";
static const ::fwServices::IService::KeyType s_TIMELINE2_INPUT       = "timeline2";
static const ::fwServices::IService::KeyType s_CAMERA_INPUT          = "camera";
static const ::fwServices::IService::KeyType s_SCALE_INPUT           = "scale";

static const ::fwServices::IService::KeyType s_POINTCLOUD_OUTPUT = "pointCloud";

static const std::string s_DOWNSAMPLE_CONFIG = "downsampleWidth";
static const std::string s_MODE_CONFIG       = "mode";

//------------------------------------------------------------------------------

SOpenvslam::SOpenvslam() noexcept
{
    m_sigTrackingInitialized    = newSignal< SignalType >(s_TRACKING_INITIALIZED);
    m_sigTrackingNotInitialized = newSignal< SignalType >(s_TRACKING_NOT_INITIALIZED);

    m_sigTracked      = newSignal< SignalType >(s_TRACKED);
    m_sigTrackingLost = newSignal< SignalType >(s_TRACKING_LOST);

    m_sigVocFileUnloaded       = newSignal< SignalType >(s_VOCFILE_UNLOADED);
    m_sigVocFileLoadingStarted = newSignal< SignalType >(s_VOCFILE_LOADING_STARTED);
    m_sigVocFileLoaded         = newSignal< SignalType >(s_VOCFILE_LOADED);

    m_sigMapLoaded = newSignal< SignalType >(s_MAP_LOADED);

    newSlot(s_ENABLE_LOCALIZATION_SLOT, &SOpenvslam::enableLocalization, this);
    newSlot(s_ACTIVATE_LOCALIZATION_SLOT, &SOpenvslam::activateLocalization, this);

    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SOpenvslam::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SOpenvslam::setIntParameter, this);

    newSlot(s_RESET_POINTCLOUD_SLOT, &SOpenvslam::resetPointCloud, this);

    newSlot(s_SAVE_MAP_SLOT, &SOpenvslam::saveMap, this);
    newSlot(s_LOAD_MAP_SLOT, &SOpenvslam::loadMap, this);

    newSlot(s_PAUSE_TRACKER, &SOpenvslam::pause, this);

    m_pointcloudWorker = ::fwThread::Worker::New();

    m_timer = m_pointcloudWorker->createTimer();
    m_timer->setFunction(std::bind(&SOpenvslam::updatePointCloud, this));
    m_timer->setDuration(std::chrono::milliseconds(1000));  // update pointcloud every seconds.

}

//------------------------------------------------------------------------------

SOpenvslam::~SOpenvslam() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenvslam::configuring()
{
    this->::arServices::ITracker::configuring();
    const ConfigType cfg = this->getConfigTree();

    m_downSampleWidth = cfg.get< size_t >(s_DOWNSAMPLE_CONFIG, m_downSampleWidth);
    const std::string mode = cfg.get<std::string>(s_MODE_CONFIG, "MONO");

    // if mode is not set: assuming MONO
    if(mode == "STEREO")
    {
        //TODO: STEREO Mode.
        m_trackingMode = MONO;
        SLM_ERROR("'STEREO' mode is not handle for now. Switching back to 'MONO'");
    }
    else if(mode == "DEPTH")
    {
        //TODO: DEPTH Mode.
        m_trackingMode = MONO;
        SLM_ERROR("'DEPTH' mode is not handle for now. Switching back to 'MONO'");
    }
    else
    {
        // Here mode should be MONO !
        SLM_ASSERT("Mode '"+ mode +"' is not a valid mode (MONO, STEREO, DEPTH).", mode == "MONO");
        m_trackingMode = MONO;
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::starting()
{
    // input parameters
    m_frameTL = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT);
    SLM_ASSERT("The input "+ s_TIMELINE_INPUT +" is not valid.", m_frameTL);

    m_camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("The input " + s_CAMERA_INPUT +" is not valid.", m_camera);

    m_cameraMatrixTL = this->getInOut< ::arData::MatrixTL >(s_CAMERA_MATRIXTL_INOUT);
    if (m_cameraMatrixTL)
    {
        m_cameraMatrixTL->initPoolSize(50);
    }

    m_pointCloud = ::fwData::Mesh::New();
    this->setOutput(s_POINTCLOUD_OUTPUT, m_pointCloud);

    if(m_trackingMode != MONO)
    {
        m_frameTL2 = this->getInput< ::arData::FrameTL >(s_TIMELINE2_INPUT);
        SLM_ASSERT("The input "+ s_TIMELINE2_INPUT +" is not valid.", m_frameTL2);
    }

}

//------------------------------------------------------------------------------

void SOpenvslam::stopping()
{

    this->stopTracking();
}

//------------------------------------------------------------------------------

void SOpenvslam::updating()
{
}

//------------------------------------------------------------------------------

void SOpenvslam::startTracking()
{
    this->startTracking("");
}

//------------------------------------------------------------------------------

void SOpenvslam::startTracking(const std::string& _mapFile)
{
    std::unique_lock< std::mutex > lock(m_slamLock);

    if(m_vocabularyPath.empty())
    {
        m_sigVocFileLoadingStarted->asyncEmit();
        m_vocabularyPath = ::fwRuntime::getBundleResourceFilePath("openvslamTracker", "orb_vocab.dbow2").string();
        m_sigVocFileLoaded->asyncEmit();
    }
    if(m_slamSystem == nullptr)
    {
        const auto config = ::openvslamIO::Helper::createMonocularConfig(m_camera, m_orbParameters,
                                                                         m_initializerParameters);

        m_slamSystem = std::unique_ptr< ::openvslam::system >(new ::openvslam::system(config, m_vocabularyPath));

        m_slamSystem->startup();

        m_ovsMapPublisher   = m_slamSystem->get_map_publisher();
        m_ovsFramePublisher = m_slamSystem->get_frame_publisher();

        SLM_ASSERT("Map Publisher souldn't be null", m_ovsMapPublisher);
        SLM_ASSERT("Frame Publisher souldn't be null", m_ovsFramePublisher);

        // Launch the pointcloud thread.
        if(!m_timer->isRunning())
        {
            m_timer->start();
        }

        if(!_mapFile.empty())
        {
            m_slamSystem->load_map_database(_mapFile);
        }

        m_isTracking = true;
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::stopTracking()
{
    std::unique_lock<std::mutex> lock(m_slamLock);

    if(m_timer->isRunning())
    {
        m_timer->stop();
    }

    if(m_slamSystem)
    {
        if(!m_saveMapPath.empty())
        {
            m_slamSystem->save_map_database(m_saveMapPath);
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
    if(m_slamSystem )
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
    if(m_slamSystem)
    {
        m_localization = true;
        m_slamSystem->disable_mapping_module();
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
        m_orbParameters.iniFastThr = static_cast<unsigned int >(_val);
    }
    else if(_key == "minThFAST")
    {
        m_orbParameters.minFastThr = static_cast<unsigned int >(_val);
    }
    else if(_key == "initializer.numRansacIterations")
    {
        m_initializerParameters.numRansacIterations = static_cast<unsigned int >(_val);
    }
    else if(_key == "initializer.minNumTriangulatedPts")
    {
        m_initializerParameters.minNumTriangulatedPts = static_cast<unsigned int >(_val);
    }
    else if(_key == "initializer.numBAIterations")
    {
        m_initializerParameters.numBAIterations = static_cast< unsigned int >(_val);
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
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
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
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
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::loadMap()
{
    static ::boost::filesystem::path sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select openvslam map file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(sDefaultPath) );
    dialogFile.addFilter("openvlsam map files", "*.map");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(sDefaultPath) );
        this->stopTracking();
        std::string mapFile = result->getPath().string();
        this->startTracking(mapFile);
        m_sigMapLoaded->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::saveMap()
{
    static ::boost::filesystem::path sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to save Openvslam map");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(sDefaultPath) );
    dialogFile.addFilter("openvslam files", "*.map");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(sDefaultPath) );
        m_saveMapPath = result->getPath().string();
    }

    std::unique_lock<std::mutex> lock(m_slamLock);
    if (m_slamSystem)
    {
        //If system is running save now.
        m_slamSystem->save_map_database(m_saveMapPath);
    }
    else
    {
        ::fwGui::dialog::MessageDialog warning;
        warning.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        warning.setTitle("Save Map offline");
        warning.setMessage(
            "OpenVSLAM is currently offline, map cannot be saved now.\
        file path can be stored and map will be automatically saved at next openvlsam stop (start/stop). ");
        warning.addButton(::fwGui::dialog::IMessageDialog::Buttons::NO );
        warning.addButton(::fwGui::dialog::IMessageDialog::Buttons::YES );
        warning.setDefaultButton(::fwGui::dialog::IMessageDialog::Buttons::NO );
        const auto answer = warning.show();
        if(answer == ::fwGui::dialog::IMessageDialog::Buttons::NO)
        {
            m_saveMapPath.clear();
        }
    }
}

//------------------------------------------------------------------------------

void SOpenvslam::pause()
{
    std::unique_lock<std::mutex> lock(m_slamLock);
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

    // Clear Sight mesh
    m_pointCloud->clearCells();
    m_pointCloud->clearPoints();
    auto sigMesh = m_pointCloud->signal< ::fwData::Object::ModifiedSignalType >
                       (::fwData::Object::s_MODIFIED_SIG);
    sigMesh->asyncEmit();

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

void SOpenvslam::tracking(::fwCore::HiResClock::HiResClockType& timestamp)
{

    std::unique_lock<std::mutex> lock(m_slamLock);
    if (m_slamSystem && !m_isPaused)
    {
        CSPTR(::arData::FrameTL::BufferType) bufferFrame = m_frameTL->getClosestBuffer(timestamp);
        if(bufferFrame == nullptr)
        {
            return;
        }
        const std::uint8_t* frameData = &bufferFrame->getElement(0);

        // this is the main image
        ::cv::Mat imgLeft = ::cvIO::FrameTL::moveToCv(m_frameTL, frameData);

        //TODO: downscale image if necessary (scaling issue needs to be resolved.).

        ::cv::Mat imgDepth;    // this is the depth image (only if DEPTH)

        if(m_trackingMode != MONO)
        {
            CSPTR(::arData::FrameTL::BufferType) bufferFrame2 = m_frameTL2->getClosestBuffer(timestamp);
            if(bufferFrame2 == nullptr)
            {
                return;
            }

            const std::uint8_t* frameData2 = &bufferFrame2->getElement(0);

            ::cv::Mat imgRight = ::cvIO::FrameTL::moveToCv(m_frameTL2, frameData2);

            // the two frames need to have same size
            if(imgLeft.cols != imgRight.cols || imgLeft.rows != imgRight.rows)
            {
                SLM_ERROR("First frame and Second Frame should have the same size");
                return;
            }

            if(m_trackingMode == DEPTH)
            {
                // FIXME : DEPTH image should be in float, but frameTL only provide uint8!
                imgRight.convertTo(imgDepth, CV_32F);
            }

        }    // STEREO/DEPTH

        Eigen::Matrix4d pos;
        try
        {
            // The position returned by feed_* function shouldn't be used.
            // since if tracking is lost the position can contain NaN or Inf values.
            m_slamSystem->feed_monocular_frame(imgLeft, timestamp);

            //TODO: STEREO & DEPTH Mode.

            // Use the publisher position instead.
            pos = m_ovsMapPublisher->get_current_cam_pose();

            const auto im = m_ovsFramePublisher->draw_frame();

            if(m_showFeatures)
            {
                ::cv::imshow("Openvslam internal frame", im);
                ::cv::waitKey(1);
            }

        }
        catch (std::exception& e)
        {
            OSLM_ERROR("Error processing openvslam: " << e.what());
            return;
        }

        ::fwData::Float::csptr floatObj = this->getInput< ::fwData::Float >(s_SCALE_INPUT);

        // FIXME : Arbitrary scale, the real scale should be computed with respect to a real object in the 3D Scene.
        if (floatObj && floatObj->value() > 0)
        {
            m_scale = m_scale / floatObj->value();
        }

        //scale needs to be adapted with the downscale ratio, so that map can fit video.
        // m_scale = m_scale * ( m_camera->getWidth() / m_downSampleWidth); //This seems to be wrong...

        if (!pos.isZero())
        {
            // fill in the camera position matrix
            if(m_cameraMatrixTL)
            {
                const auto inv = pos.inverse();

                float matrix[16];

                for(int i = 0; i < 4; ++i)
                {
                    for(int j = 0; j < 4; ++j)
                    {
                        matrix[i*4+j] = static_cast<float>(inv(i, j));
                    }
                }

                matrix[3]  *= m_scale;
                matrix[7]  *= m_scale;
                matrix[11] *= m_scale;

                SPTR(::arData::MatrixTL::BufferType) data = m_cameraMatrixTL->createBuffer(timestamp);
                data->setElement(matrix, 0);
                m_cameraMatrixTL->pushObject(data);

                ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = m_cameraMatrixTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG );

                sig->emit(timestamp);
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
        ::fwGui::dialog::MessageDialog::showMessageDialog("Vocabulary",
                                                          "Vocabulary file : "+_filePath + " can not be loaded.",
                                                          ::fwGui::dialog::MessageDialog::WARNING);
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
    if (m_pointCloud && !m_localization && !m_isPaused)
    {
        std::vector< ::openvslam::data::landmark*> landmarks;
        std::set< ::openvslam::data::landmark*> local_landmarks;

        m_ovsMapPublisher->get_landmarks(landmarks, local_landmarks);

        if (landmarks.empty())
        {
            return;
        }

        ::fwData::mt::ObjectWriteLock lockTFM(m_pointCloud);
        m_pointCloud->clearCells();
        m_pointCloud->clearPoints();

        ::fwDataTools::helper::Mesh helper(m_pointCloud);

        unsigned int i = 0;
        if(m_localMap)
        {
            for(const auto lm : local_landmarks)
            {
                if (!lm || lm->will_be_erased())
                {
                    continue;
                }

                const openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                helper.insertNextPoint(static_cast<float>(pos_w(0)) * m_scale,
                                       static_cast<float>(pos_w(1)) * m_scale,
                                       static_cast<float>(pos_w(2)) * m_scale);
                helper.insertNextCell(i);
                ++i;
            }
        }
        else
        {
            for(const auto lm : landmarks)
            {
                if (!lm || lm->will_be_erased())
                {
                    continue;
                }

                const openvslam::Vec3_t pos_w = lm->get_pos_in_world();

                helper.insertNextPoint(static_cast<float>(pos_w(0)) * m_scale,
                                       static_cast<float>(pos_w(1)) * m_scale,
                                       static_cast<float>(pos_w(2)) * m_scale);
                helper.insertNextCell(i);
                ++i;
            }
        }

        if(i != 0)
        {
            m_sigTrackingInitialized->asyncEmit();
            m_sigTracked->asyncEmit();
            auto sigMesh = m_pointCloud->signal< ::fwData::Object::ModifiedSignalType >
                               (::fwData::Object::s_MODIFIED_SIG);
            sigMesh->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

} // namespace orbslamTracker
