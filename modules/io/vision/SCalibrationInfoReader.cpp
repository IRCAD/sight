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

#include "SCalibrationInfoReader.hpp"

#include <core/com/slots.hxx>
#include <core/location/single_folder.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Image.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>

#include <opencv2/opencv.hpp>

namespace sight::module::io::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//------------------------------------------------------------------------------

SCalibrationInfoReader::SCalibrationInfoReader() noexcept
{
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &SCalibrationInfoReader::updateChessboardSize, this);
}

//------------------------------------------------------------------------------

SCalibrationInfoReader::~SCalibrationInfoReader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType SCalibrationInfoReader::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Select a folder holding calibration inputs" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());

    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::configuring()
{
    sight::io::service::reader::configuring();

    const ConfigType config      = this->getConfiguration();
    const ConfigType boardConfig = config.get_child("board");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_scaleKey = boardConfig.get<std::string>("<xmlattr>.scale", "");
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::starting()
{
    this->updateChessboardSize();
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::updating()
{
    if(this->hasLocationDefined())
    {
        const auto data      = m_data.lock();
        const auto calibInfo = std::dynamic_pointer_cast<data::CalibrationInfo>(data.get_shared());
        SIGHT_ASSERT("Missing calibration info.", calibInfo);

        data::mt::locked_ptr calibInfoLock(calibInfo);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        using DetectionPairType = std::pair<data::Image::sptr, data::PointList::sptr>;

        const std::filesystem::path folder = this->get_folder();

        // Use a map to sort input images by their filename.
        std::map<std::string, DetectionPairType> filenameDetectionMap;

        for(const std::filesystem::path& dirEntry : std::filesystem::directory_iterator(folder))
        {
            cv::Mat img = cv::imread(dirEntry.string(), cv::IMREAD_COLOR);
            std::string errorMessage;

            if(!img.empty())
            {
                cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

                data::PointList::sptr chessboardPts = geometry::vision::helper::detectChessboard(
                    img,
                    m_width,
                    m_height,
                    m_scale
                );

                if(chessboardPts)
                {
                    data::Image::sptr calibImg = std::make_shared<data::Image>();
                    sight::io::opencv::Image::copyFromCv(*calibImg.get(), img);

                    calibImg->setSpacing({{1., 1., 1.}});
                    calibImg->setOrigin({{0., 0., 0.}});

                    const auto detectionPair = std::make_pair(calibImg, chessboardPts);
                    const auto filename      = dirEntry.filename().string();
                    filenameDetectionMap[filename] = detectionPair;
                }
                else
                {
                    errorMessage = "Couldn't detect a chessboard in '" + dirEntry.string() + "'.\n\n"
                                                                                             "Please make sure that the right chessboard parameters are set.";
                }
            }
            else
            {
                errorMessage = "Couldn't read '" + dirEntry.string() + "'.\n\n"
                                                                       "Make sure it is a valid image format.";
            }

            if(!errorMessage.empty())
            {
                errorMessage += "\n\n Abort reading?";
                sight::ui::dialog::message messageBox("Reading calibration inputs failed", errorMessage,
                                                      sight::ui::dialog::message::WARNING);

                messageBox.addButton(ui::dialog::message::YES_NO);

                if((messageBox.show() & sight::ui::dialog::message::YES) != 0)
                {
                    filenameDetectionMap.clear();
                    m_readFailed = true;
                    break;
                }

                errorMessage.clear();
            }
        }

        cursor.setDefaultCursor();

        if(!filenameDetectionMap.empty())
        {
            for(auto& [fname, detection] : filenameDetectionMap)
            {
                auto& [img, chessboard] = detection;
                calibInfo->addRecord(img, chessboard);
            }

            auto sig = calibInfo->signal<data::CalibrationInfo::AddedRecordSignalType>
                           (data::CalibrationInfo::MODIFIED_SIG);

            sig->async_emit();
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::stopping()
{
}

// ----------------------------------------------------------------------------

void SCalibrationInfoReader::updateChessboardSize()
{
    try
    {
        ui::Preferences preferences;

        if(const auto& saved = preferences.get_optional<decltype(m_width)>(m_widthKey); saved)
        {
            m_width = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_height)>(m_heightKey); saved)
        {
            m_height = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_scale)>(m_scaleKey); saved)
        {
            m_scale = *saved;

            if(m_scale > 1.F)
            {
                m_scale = 1.F;
                SIGHT_ERROR("It is pointless to upscale the image for chessboard detection.");
            }
        }
    }
    catch(const ui::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
