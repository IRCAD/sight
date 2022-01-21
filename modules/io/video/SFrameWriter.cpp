/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "SFrameWriter.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/location/SingleFolder.hpp>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace sight::module::io::video
{

static const core::com::Slots::SlotKeyType s_SAVE_FRAME           = "saveFrame";
static const core::com::Slots::SlotKeyType s_START_RECORD         = "startRecord";
static const core::com::Slots::SlotKeyType s_STOP_RECORD          = "stopRecord";
static const core::com::Slots::SlotKeyType s_RECORD               = "record";
static const core::com::Slots::SlotKeyType s_WRITE                = "write";
static const core::com::Slots::SlotKeyType s_SET_FORMAT_PARAMETER = "setFormatParameter";

//------------------------------------------------------------------------------

SFrameWriter::SFrameWriter() noexcept :
    m_imageType(0),
    m_isRecording(false),
    m_format(".tiff")
{
    newSlot(s_SAVE_FRAME, &SFrameWriter::saveFrame, this);
    newSlot(s_START_RECORD, &SFrameWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SFrameWriter::stopRecord, this);
    newSlot(s_RECORD, &SFrameWriter::record, this);
    newSlot(s_WRITE, &SFrameWriter::write, this);
    newSlot(s_SET_FORMAT_PARAMETER, &SFrameWriter::setFormatParameter, this);
}

//------------------------------------------------------------------------------

SFrameWriter::~SFrameWriter() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SFrameWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SFrameWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();

    service::IService::ConfigType config = this->getConfigTree();

    m_format = config.get<std::string>("format", ".tiff");
}

//------------------------------------------------------------------------------

void SFrameWriter::starting()
{
}

//------------------------------------------------------------------------------

void SFrameWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();
    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the frames" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
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

void SFrameWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::updating()
{
    core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    this->saveFrame(timestamp);
}

//------------------------------------------------------------------------------

void SFrameWriter::saveFrame(core::HiResClock::HiResClockType _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::write(core::HiResClock::HiResClockType timestamp)
{
    if(m_isRecording)
    {
        // Retrieve dataStruct associated with this service
        const auto locked  = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::FrameTL>(locked.get_shared());

        // The following lock causes the service to drop frames if under heavy load. This prevents desynchronization
        // between frames and timestamps.
        // TODO: experiment with queuing frames and writing them from a worker thread.
        const auto sig = frameTL->signal<data::FrameTL::ObjectPushedSignalType>(
            data::FrameTL::s_OBJECT_PUSHED_SIG
        );
        core::com::Connection::Blocker writeBlocker(sig->getConnection(m_slots[s_WRITE]));

        // Get the buffer of the copied timeline
        const auto buffer = frameTL->getClosestBuffer(timestamp);

        if(buffer)
        {
            timestamp = buffer->getTimestamp();
            const int width  = static_cast<int>(frameTL->getWidth());
            const int height = static_cast<int>(frameTL->getHeight());

            const std::uint8_t* imageBuffer = &buffer->getElement(0);

            cv::Mat image(cv::Size(width, height), m_imageType, (void*) imageBuffer, cv::Mat::AUTO_STEP);

            const std::size_t time = static_cast<std::size_t>(timestamp);
            const std::string filename("img_" + std::to_string(time) + m_format);
            const std::filesystem::path path = this->getFolder() / filename;

            if(image.type() == CV_8UC3)
            {
                // convert the read image from BGR to RGB
                cv::Mat imageRgb;
                cv::cvtColor(image, imageRgb, cv::COLOR_BGR2RGB);
                cv::imwrite(path.string(), imageRgb);
            }
            else if(image.type() == CV_8UC4)
            {
                // convert the read image from BGRA to RGBA
                cv::Mat imageRgb;
                cv::cvtColor(image, imageRgb, cv::COLOR_BGRA2RGBA);
                cv::imwrite(path.string(), imageRgb);
            }
            else
            {
                cv::imwrite(path.string(), image);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::startRecord()
{
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked  = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::FrameTL>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::FrameTL::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            frameTL
        );

        if(frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->numComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if(frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->numComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if(frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_8UC1;
        }
        else if(frameTL->getType() == core::tools::Type::s_UINT16 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frameTL->getType().string() + " with "
                + std::to_string(frameTL->numComponents()) + " is not supported"
            );
            return;
        }

        std::filesystem::path path = this->getFolder();

        if(!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::stopRecord()
{
    m_isRecording = false;
}

//------------------------------------------------------------------------------

void SFrameWriter::record(bool state)
{
    if(state)
    {
        this->startRecord();
    }
    else
    {
        this->stopRecord();
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::setFormatParameter(std::string val, std::string key)
{
    if(key == "format")
    {
        if(val == ".tiff"
           || val == ".jpeg"
           || val == ".bmp"
           || val == ".png"
           || val == ".jp2")
        {
            m_format = val;
        }
        else
        {
            SIGHT_ERROR("Value : '" + val + "' is not supported");
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFrameWriter::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(sight::io::base::service::s_DATA_KEY, data::FrameTL::s_OBJECT_PUSHED_SIG, s_WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
