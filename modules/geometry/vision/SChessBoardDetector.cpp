/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Image.hpp>

#include <ui/__/Preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include <thread>

namespace sight::module::geometry::vision
{

static const core::com::slots::key_t RECORD_POINTS_SLOT          = "recordPoints";
static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

static const core::com::signals::key_t CHESSBOARD_DETECTED_SIG = "chessboardDetected";
static const core::com::signals::key_t CHESSBOARD_FOUND_SIG    = "chessboardFound";

// ----------------------------------------------------------------------------

SChessBoardDetector::SChessBoardDetector() noexcept :
    m_sigChessboardDetected(new_signal<ChessboardDetectedSignalType>(CHESSBOARD_DETECTED_SIG)),
    m_sigChessboardFound(new_signal<ChessboardFoundSignalType>(CHESSBOARD_FOUND_SIG))
{
    new_slot(RECORD_POINTS_SLOT, &SChessBoardDetector::recordPoints, this);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &SChessBoardDetector::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SChessBoardDetector::~SChessBoardDetector() noexcept =
    default;

// ----------------------------------------------------------------------------

void SChessBoardDetector::configuring()
{
    SIGHT_ASSERT(
        "This service must have the same number of 'image' keys and 'calInfo' keys",
        m_image.size() == m_calInfo.size()
    );

    const ConfigType config      = this->getConfiguration();
    const ConfigType boardConfig = config.get_child("board");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_scaleKey = boardConfig.get<std::string>("<xmlattr>.scale");
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::starting()
{
    this->updateChessboardSize();

    const std::size_t imageGroupSize = m_image.size();

    m_images.resize(imageGroupSize);
    m_pointLists.resize(imageGroupSize);
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updating()
{
    const std::size_t imageGroupSize = m_image.size();

    // Run parallel detections in separate threads.
    std::vector<std::thread> detectionJobs;
    for(std::size_t i = 1 ; i < imageGroupSize ; ++i)
    {
        detectionJobs.emplace_back(&SChessBoardDetector::doDetection, this, i);
    }

    // Detection in the first image is done on the service's worker.
    this->doDetection(0);

    for(auto& detectionJob : detectionJobs)
    {
        detectionJob.join();
    }

    const bool allDetected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    m_sigChessboardDetected->async_emit(allDetected);

    if(allDetected)
    {
        m_sigChessboardFound->async_emit();
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::stopping()
{
    m_images.clear();
    m_pointLists.clear();
}

// ----------------------------------------------------------------------------

service::connections_t SChessBoardDetector::getAutoConnections() const
{
    connections_t connections;
    connections.push(s_IMAGE_INPUT, data::Image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INPUT, data::Image::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::recordPoints()
{
    const std::size_t calibGroupSize = m_calInfo.size();

    const bool allDetected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    if(allDetected)
    {
        for(std::size_t i = 0 ; i < calibGroupSize ; ++i)
        {
            auto calInfo = m_calInfo[i].lock();
            SIGHT_ASSERT("Missing 'calibInfo' in-out.", calInfo);

            if(m_pointLists[i])
            {
                calInfo->addRecord(m_images[i], m_pointLists[i]);

                // Notify
                auto sig = calInfo->signal<data::CalibrationInfo::AddedRecordSignalType>(
                    data::CalibrationInfo::ADDED_RECORD_SIG
                );

                sig->async_emit();
            }
            else
            {
                calInfo->addRecord(m_images[i], std::make_shared<data::PointList>());
            }
        }
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updateChessboardSize()
{
    try
    {
        ui::Preferences preferences;
        m_width  = preferences.get(m_widthKey, m_width);
        m_height = preferences.get(m_heightKey, m_height);
        m_scale  = preferences.get(m_scaleKey, m_scale);

        if(m_scale > 1.F)
        {
            m_scale = 1.F;
            SIGHT_ERROR("It is pointless to upscale the image for chessboard detection.");
        }
    }
    catch(const ui::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::doDetection(std::size_t _imageIndex)
{
    const auto img = m_image[_imageIndex].lock();
    SIGHT_ASSERT("Missing 'image' input.", img);

    const bool isValid = data::helper::MedicalImage::checkImageValidity(img.get_shared());

    if(isValid)
    {
        const cv::Mat cvImg = io::opencv::Image::moveToCv(img.get_shared());

        m_pointLists[_imageIndex] =
            sight::geometry::vision::helper::detectChessboard(cvImg, m_width, m_height, m_scale);

        if(m_pointLists[_imageIndex] != nullptr)
        {
            m_images[_imageIndex] = std::make_shared<data::Image>();
            m_images[_imageIndex]->deep_copy(img.get_shared());
        }
        else
        {
            m_images[_imageIndex] = nullptr;
        }

        const bool outputDetection = (m_detection.size() == m_image.size());
        if(outputDetection)
        {
            auto outPl = m_detection[_imageIndex].lock();

            if(m_pointLists[_imageIndex] != nullptr)
            {
                outPl->deep_copy(m_pointLists[_imageIndex]);
            }
            else
            {
                outPl->getPoints().clear();
            }

            auto sig = outPl->signal<data::PointList::ModifiedSignalType>(data::PointList::MODIFIED_SIG);
            sig->async_emit();
        }
    }
}

// ----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
