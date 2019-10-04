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

#include "ioCalibration/SCalibrationInfoReader.hpp"

#include <arData/CalibrationInfo.hpp>

#include <calibration3d/helper.hpp>

#include <cvIO/Image.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/opencv.hpp>

namespace ioCalibration
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

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
    static std::filesystem::path s_defaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Select a folder holding calibration inputs" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(s_defaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result = ::fwData::location::Folder::dynamicCast(dialogFile.show());

    if (result)
    {
        s_defaultPath = result->getFolder().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(s_defaultPath) );
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
        ::arData::CalibrationInfo::sptr calibInfo =
            this->getInOut< ::arData::CalibrationInfo >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("Missing calibration info.", calibInfo);

        ::fwData::mt::ObjectWriteLock calibInfoLock(calibInfo);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        using DetectionPairType = std::pair< ::fwData::Image::sptr, ::fwData::PointList::sptr >;

        const std::filesystem::path folder = this->getFolder();
        std::vector<DetectionPairType> detectionVector;

        for(const std::filesystem::path& dirEntry : std::filesystem::directory_iterator(folder))
        {
            ::cv::Mat img = ::cv::imread(dirEntry.string());
            std::string errorMessage;

            if(!img.empty())
            {
                ::fwData::PointList::sptr chessboardPts = ::calibration3d::helper::detectChessboard(img,
                                                                                                    m_width, m_height,
                                                                                                    m_scale);

                if(chessboardPts)
                {
                    ::fwData::Image::sptr calibImg = ::fwData::Image::New();
                    ::cvIO::Image::copyFromCv(calibImg, img);

                    calibImg->setSpacing({{1., 1.}});
                    calibImg->setOrigin({{0., 0.}});

                    auto detectionPair = std::make_pair(calibImg, chessboardPts);
                    detectionVector.push_back(detectionPair);
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
                ::fwGui::dialog::MessageDialog messageBox("Reading calibration inputs failed", errorMessage,
                                                          ::fwGui::dialog::MessageDialog::WARNING);

                messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);

                if(messageBox.show() & ::fwGui::dialog::IMessageDialog::YES)
                {
                    detectionVector.clear();
                    m_readFailed = true;
                    break;
                }
                errorMessage.clear();
            }
        }

        cursor.setDefaultCursor();

        if(!detectionVector.empty())
        {
            for(auto& [img, chessboard] : detectionVector)
            {
                calibInfo->addRecord(img, chessboard);
            }

            auto sig = calibInfo->signal< ::arData::CalibrationInfo::AddedRecordSignalType >
                           (::arData::CalibrationInfo::s_MODIFIED_SIG);

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
    const std::string widthStr = ::fwPreferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoul(widthStr);
    }

    const std::string heightStr = ::fwPreferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoul(heightStr);
    }

    const std::string scaleStr = ::fwPreferences::getPreference(m_scaleKey);
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
