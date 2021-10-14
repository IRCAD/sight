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

#include "SCalibrationInfoReader.hpp"

#include <core/com/Slots.hxx>
#include <core/location/SingleFolder.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Image.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <opencv2/opencv.hpp>

namespace sight::module::io::vision
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//------------------------------------------------------------------------------

SCalibrationInfoReader::SCalibrationInfoReader() noexcept
{
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SCalibrationInfoReader::updateChessboardSize, this);
}

//------------------------------------------------------------------------------

SCalibrationInfoReader::~SCalibrationInfoReader() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SCalibrationInfoReader::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Select a folder holding calibration inputs" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::ILocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());

    if(result)
    {
        this->setFolder(result->getFolder());
        defaultDirectory->setFolder(result->getFolder().parent_path());
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
    sight::io::base::service::IReader::configuring();

    const ConfigType config      = this->getConfigTree();
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

        data::mt::ObjectWriteLock calibInfoLock(calibInfo);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        using DetectionPairType = std::pair<data::Image::sptr, data::PointList::sptr>;

        const std::filesystem::path folder = this->getFolder();

        // Use a map to sort input images by their filename.
        std::map<std::string, DetectionPairType> filenameDetectionMap;

        for(const std::filesystem::path& dirEntry : std::filesystem::directory_iterator(folder))
        {
            ::cv::Mat img = ::cv::imread(dirEntry.string(), ::cv::IMREAD_COLOR);
            std::string errorMessage;

            if(!img.empty())
            {
                ::cv::cvtColor(img, img, ::cv::COLOR_BGR2RGB);

                data::PointList::sptr chessboardPts = geometry::vision::helper::detectChessboard(
                    img,
                    m_width,
                    m_height,
                    m_scale
                );

                if(chessboardPts)
                {
                    data::Image::sptr calibImg = data::Image::New();
                    sight::io::opencv::Image::copyFromCv(*calibImg.get(), img);

                    calibImg->setSpacing2({{1., 1., 1.}});
                    calibImg->setOrigin2({{0., 0., 0.}});

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
                sight::ui::base::dialog::MessageDialog messageBox("Reading calibration inputs failed", errorMessage,
                                                                  sight::ui::base::dialog::MessageDialog::WARNING);

                messageBox.addButton(ui::base::dialog::IMessageDialog::YES_NO);

                if(messageBox.show() & sight::ui::base::dialog::IMessageDialog::YES)
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
                           (data::CalibrationInfo::s_MODIFIED_SIG);

            sig->asyncEmit();
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
        ui::base::Preferences preferences;

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

            if(m_scale > 1.f)
            {
                m_scale = 1.f;
                SIGHT_ERROR("It is pointless to upscale the image for chessboard detection.");
            }
        }
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
