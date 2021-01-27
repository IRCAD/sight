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

#include "ioCalibration/SCalibrationInfoReader.hpp"

#include <calibration3d/helper.hpp>

#include <core/com/Slots.hxx>

#include <cvIO/Image.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>
#include <data/location/Folder.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/preferences/helper.hpp>

#include <services/macros.hpp>

#include <opencv2/opencv.hpp>

namespace ioCalibration
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//------------------------------------------------------------------------------

SCalibrationInfoReader::SCalibrationInfoReader() noexcept
{
    newSlot( s_UPDATE_CHESSBOARD_SIZE_SLOT, &SCalibrationInfoReader::updateChessboardSize, this );
}

//------------------------------------------------------------------------------

SCalibrationInfoReader::~SCalibrationInfoReader() noexcept
{

}

//------------------------------------------------------------------------------

::fwIO::IOPathType SCalibrationInfoReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::openLocationDialog()
{
    static std::filesystem::path s_defaultPath;

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Select a folder holding calibration inputs" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(s_defaultPath) );
    dialogFile.setOption(gui::dialog::ILocationDialog::READ);
    dialogFile.setType(gui::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result = data::location::Folder::dynamicCast(dialogFile.show());

    if (result)
    {
        s_defaultPath = result->getFolder().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(s_defaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SCalibrationInfoReader::configuring()
{
    ::fwIO::IReader::configuring();

    const ConfigType config      = this->getConfigTree();
    const ConfigType boardConfig = config.get_child("board");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SLM_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SLM_ASSERT("Missing board height preference key.", !m_heightKey.empty());
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
    if( this->hasLocationDefined() )
    {
        data::CalibrationInfo::sptr calibInfo =
            this->getInOut< data::CalibrationInfo >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("Missing calibration info.", calibInfo);

        data::mt::ObjectWriteLock calibInfoLock(calibInfo);

        gui::Cursor cursor;
        cursor.setCursor(gui::ICursor::BUSY);

        using DetectionPairType = std::pair< data::Image::sptr, data::PointList::sptr >;

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

                data::PointList::sptr chessboardPts = ::calibration3d::helper::detectChessboard(img,
                                                                                                m_width, m_height,
                                                                                                m_scale);

                if(chessboardPts)
                {
                    data::Image::sptr calibImg = data::Image::New();
                    ::cvIO::Image::copyFromCv(calibImg, img);

                    calibImg->setSpacing2({{1., 1., 1.}});
                    calibImg->setOrigin2({{0., 0., 0.}});

                    const auto detectionPair = std::make_pair(calibImg, chessboardPts);
                    const auto filename      = dirEntry.filename().string();
                    filenameDetectionMap[filename] = detectionPair;
                }
                else
                {
                    errorMessage = "Couldn't detect a chessboard in '" +  dirEntry.string() + "'.\n\n"
                                   "Please make sure that the right chessboard parameters are set.";
                }
            }
            else
            {
                errorMessage = "Couldn't read '" +  dirEntry.string() + "'.\n\n"
                               "Make sure it is a valid image format.";
            }

            if(!errorMessage.empty())
            {
                errorMessage += "\n\n Abort reading?";
                gui::dialog::MessageDialog messageBox("Reading calibration inputs failed", errorMessage,
                                                      gui::dialog::MessageDialog::WARNING);

                messageBox.addButton(gui::dialog::IMessageDialog::YES_NO);

                if(messageBox.show() & gui::dialog::IMessageDialog::YES)
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

            auto sig = calibInfo->signal< data::CalibrationInfo::AddedRecordSignalType >
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
    const std::string widthStr = gui::preferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoul(widthStr);
    }

    const std::string heightStr = gui::preferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoul(heightStr);
    }

    const std::string scaleStr = gui::preferences::getPreference(m_scaleKey);
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

//------------------------------------------------------------------------------

} // namespace ioCalibration
