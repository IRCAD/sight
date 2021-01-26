/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "videoOpenCV/SFrameWriter.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/location/Folder.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <services/macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace videoOpenCV
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::videoOpenCV::SFrameWriter, ::sight::data::FrameTL)

static const core::com::Slots::SlotKeyType s_SAVE_FRAME = "saveFrame";
static const core::com::Slots::SlotKeyType s_START_RECORD         = "startRecord";
static const core::com::Slots::SlotKeyType s_STOP_RECORD          = "stopRecord";
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
    newSlot(s_WRITE, &SFrameWriter::write, this);
    newSlot(s_SET_FORMAT_PARAMETER, &SFrameWriter::setFormatParameter, this);
}

//------------------------------------------------------------------------------

SFrameWriter::~SFrameWriter() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SFrameWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SFrameWriter::configuring()
{
    ::fwIO::IWriter::configuring();

    services::IService::ConfigType config = this->getConfigTree();

    m_format = config.get<std::string>("format", ".tiff");

}

//------------------------------------------------------------------------------

void SFrameWriter::starting()
{
}

//------------------------------------------------------------------------------

void SFrameWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SFrameWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the frames" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
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
    if (m_isRecording)
    {
        data::FrameTL::csptr frameTL = this->getInput< data::FrameTL >(::fwIO::s_DATA_KEY);
        // The following lock causes the service to drop frames if under heavy load. This prevents desynchronization
        // between frames and timestamps.
        // TODO: experiment with queuing frames and writing them from a worker thread.
        const auto sig = frameTL->signal< data::FrameTL::ObjectPushedSignalType>(
            data::FrameTL::s_OBJECT_PUSHED_SIG);
        core::com::Connection::Blocker writeBlocker(sig->getConnection(m_slots[s_WRITE]));

        // Get the buffer of the copied timeline
        CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

        if (buffer)
        {
            timestamp = buffer->getTimestamp();
            const int width  = static_cast<int>( frameTL->getWidth() );
            const int height = static_cast<int>( frameTL->getHeight() );

            const std::uint8_t* imageBuffer = &buffer->getElement(0);

            ::cv::Mat image(::cv::Size(width, height), m_imageType, (void*)imageBuffer, ::cv::Mat::AUTO_STEP);

            const size_t time = static_cast<size_t>(timestamp);
            const std::string filename( "img_" + std::to_string(time) + m_format);
            const std::filesystem::path path = this->getFolder() / filename;

            if (image.type() == CV_8UC3)
            {
                // convert the read image from BGR to RGB
                ::cv::Mat imageRgb;
                ::cv::cvtColor(image, imageRgb, ::cv::COLOR_BGR2RGB);
                ::cv::imwrite(path.string(), imageRgb);
            }
            else if (image.type() == CV_8UC4)
            {
                // convert the read image from BGRA to RGBA
                ::cv::Mat imageRgb;
                ::cv::cvtColor(image, imageRgb, ::cv::COLOR_BGRA2RGBA);
                ::cv::imwrite(path.string(), imageRgb);
            }
            else
            {
                ::cv::imwrite(path.string(), image);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::startRecord()
{
    if (!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if (this->hasLocationDefined())
    {
        data::FrameTL::csptr frameTL = this->getInput< data::FrameTL >(::fwIO::s_DATA_KEY);

        if (frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if (frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if (frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 1)
        {
            m_imageType = CV_8UC1;
        }
        else if (frameTL->getType() == core::tools::Type::s_UINT16 && frameTL->getNumberOfComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SLM_ERROR("This type of frame : " + frameTL->getType().string() + " with " +
                      std::to_string(frameTL->getNumberOfComponents()) + " is not supported");
            return;
        }

        std::filesystem::path path = this->getFolder();

        if (!std::filesystem::exists(path))
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

void SFrameWriter::setFormatParameter(std::string val, std::string key)
{
    if(key == "format")
    {
        if(val == ".tiff" ||
           val == ".jpeg" ||
           val == ".bmp"  ||
           val == ".png"  ||
           val == ".jp2")
        {
            m_format = val;
        }
        else
        {
            SLM_ERROR("Value : '"+ val + "' is not supported");
        }
    }
    else
    {
        SLM_ERROR("The slot key : '"+ key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SFrameWriter::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(::fwIO::s_DATA_KEY, data::FrameTL::s_OBJECT_PUSHED_SIG, s_WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace videoOpenCV
