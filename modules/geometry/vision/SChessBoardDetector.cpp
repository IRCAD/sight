/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "SChessBoardDetector.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/CalibrationInfo.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Image.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include <ui/base/preferences/helper.hpp>

#include <thread>

namespace sight::module::geometry::vision
{

static const core::com::Slots::SlotKeyType s_RECORD_POINTS_SLOT          = "recordPoints";
static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

static const core::com::Signals::SignalKeyType s_CHESSBOARD_DETECTED_SIG = "chessboardDetected";
static const core::com::Signals::SignalKeyType s_CHESSBOARD_FOUND_SIG    = "chessboardFound";

static const service::IService::KeyType s_IMAGE_INPUT     = "image";
static const service::IService::KeyType s_CALINFO_INOUT   = "calInfo";
static const service::IService::KeyType s_DETECTION_INOUT = "detection";

// ----------------------------------------------------------------------------

SChessBoardDetector::SChessBoardDetector() noexcept
{
    m_sigChessboardDetected = newSignal< ChessboardDetectedSignalType >( s_CHESSBOARD_DETECTED_SIG );
    m_sigChessboardFound    = newSignal< ChessboardFoundSignalType >( s_CHESSBOARD_FOUND_SIG );

    newSlot( s_RECORD_POINTS_SLOT, &SChessBoardDetector::recordPoints, this );
    newSlot( s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessBoardDetector::updateChessboardSize, this );
}

// ----------------------------------------------------------------------------

SChessBoardDetector::~SChessBoardDetector() noexcept
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::configuring()
{
    SLM_ASSERT("This service must have the same number of 'image' keys and 'calInfo' keys",
               this->getKeyGroupSize(s_IMAGE_INPUT) == this->getKeyGroupSize(s_CALINFO_INOUT));

    const ConfigType config      = this->getConfigTree();
    const ConfigType boardConfig = config.get_child("board");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SLM_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SLM_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_scaleKey = boardConfig.get<std::string>("<xmlattr>.scale");
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::starting()
{
    this->updateChessboardSize();

    const size_t imageGroupSize = this->getKeyGroupSize(s_IMAGE_INPUT);

    m_images.resize(imageGroupSize);
    m_pointLists.resize(imageGroupSize);
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updating()
{
    const size_t imageGroupSize = this->getKeyGroupSize(s_IMAGE_INPUT);

    // Run parallel detections in separate threads.
    std::vector<std::thread> detectionJobs;
    for(size_t i = 1; i < imageGroupSize; ++i)
    {
        detectionJobs.push_back(std::thread(&SChessBoardDetector::doDetection, this, i));
    }

    // Detection in the first image is done on the service's worker.
    this->doDetection(0);

    for(auto& detectionJob : detectionJobs)
    {
        detectionJob.join();
    }

    const bool allDetected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    m_sigChessboardDetected->asyncEmit(allDetected);

    if(allDetected)
    {
        m_sigChessboardFound->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::stopping()
{
    m_images.clear();
    m_pointLists.clear();
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SChessBoardDetector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::recordPoints()
{
    const size_t calibGroupSize = this->getKeyGroupSize(s_CALINFO_INOUT);

    const bool allDetected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    if (allDetected)
    {
        for(size_t i = 0; i < calibGroupSize; ++i)
        {
            auto calInfo = this->getInOut< data::CalibrationInfo >(s_CALINFO_INOUT, i);
            SLM_ASSERT("Missing 'calibInfo' in-out.", calInfo);
            data::mt::ObjectWriteLock calInfoLock(calInfo);

            if(m_pointLists[i])
            {
                calInfo->addRecord(m_images[i], m_pointLists[i]);

                // Notify
                data::CalibrationInfo::AddedRecordSignalType::sptr sig;
                sig = calInfo->signal< data::CalibrationInfo::AddedRecordSignalType >(
                    data::CalibrationInfo::s_ADDED_RECORD_SIG);

                sig->asyncEmit();
            }
            else
            {
                calInfo->addRecord(m_images[i], data::PointList::New());
            }
        }
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updateChessboardSize()
{
    const std::string widthStr = ui::base::preferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoul(widthStr);
    }

    const std::string heightStr = ui::base::preferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoul(heightStr);
    }

    const std::string scaleStr = ui::base::preferences::getPreference(m_scaleKey);
    if (!scaleStr.empty())
    {
        m_scale = std::stof(scaleStr);

        if(m_scale > 1.f)
        {
            m_scale = 1.f;
            SLM_ERROR("It is pointless to upscale the image for chessboard detection.");
        }
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::doDetection(size_t _imageIndex)
{
    const auto img = this->getInput< data::Image >(s_IMAGE_INPUT, _imageIndex);
    SLM_ASSERT("Missing 'image' input.", img);

    data::mt::ObjectReadLock imgLock(img);
    const bool isValid = data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(img);

    if(isValid)
    {
        const ::cv::Mat cvImg = io::opencv::Image::moveToCv(img);

        m_pointLists[_imageIndex] =
            sight::geometry::vision::helper::detectChessboard(cvImg, m_width, m_height, m_scale);

        if (m_pointLists[_imageIndex] != nullptr)
        {
            m_images[_imageIndex] = data::Image::New();
            m_images[_imageIndex]->deepCopy(img);
        }
        else
        {
            m_images[_imageIndex] = nullptr;
        }

        const bool outputDetection = (this->getKeyGroupSize(s_DETECTION_INOUT) == this->getKeyGroupSize(s_IMAGE_INPUT));
        if(outputDetection)
        {
            auto outPl = this->getInOut< data::PointList >(s_DETECTION_INOUT, _imageIndex);
            data::mt::ObjectWriteLock writeLockOutPl(outPl);
            if (m_pointLists[_imageIndex] != nullptr)
            {
                outPl->deepCopy(m_pointLists[_imageIndex]);
            }
            else
            {
                outPl->getPoints().clear();
            }

            auto sig = outPl->signal< data::PointList::ModifiedSignalType >(data::PointList::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
