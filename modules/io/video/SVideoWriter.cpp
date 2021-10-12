/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "SVideoWriter.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::io::video
{

static const core::com::Slots::SlotKeyType s_SAVE_FRAME   = "saveFrame";
static const core::com::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const core::com::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";
static const core::com::Slots::SlotKeyType s_RECORD       = "record";

const std::string SVideoWriter::s_MP4_EXTENSION = ".mp4";
const std::string SVideoWriter::s_AVC1_CODEC    = "avc1";

//------------------------------------------------------------------------------

SVideoWriter::SVideoWriter() noexcept
{
    newSlot(s_SAVE_FRAME, &SVideoWriter::saveFrame, this);
    newSlot(s_START_RECORD, &SVideoWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SVideoWriter::stopRecord, this);
    newSlot(s_RECORD, &SVideoWriter::record, this);
}

//------------------------------------------------------------------------------

SVideoWriter::~SVideoWriter() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SVideoWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SVideoWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SVideoWriter::starting()
{
}

//------------------------------------------------------------------------------

void SVideoWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();
    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an file to save the video" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("mp4", "*.mp4");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        m_selectedExtension = dialogFile.getCurrentSelection();
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->setFile(result->getFile());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SVideoWriter::updating()
{
}

//------------------------------------------------------------------------------

void SVideoWriter::writeBuffer(int width, int height, CSPTR(data::FrameTL::BufferType)buffer)
{
    SIGHT_ASSERT("OpenCV video writer not initialized", m_writer);
    const std::uint8_t* imageBuffer = &buffer->getElement(0);

    const ::cv::Mat image(
        ::cv::Size(width, height),
        m_imageType, const_cast<std::uint8_t*>(imageBuffer),
        ::cv::Mat::AUTO_STEP
    );
    if(m_imageType == CV_16UC1)
    {
        // Convert the image to a RGB image
        ::cv::Mat img8bit;
        ::cv::Mat imgColor;
        image.convertTo(img8bit, CV_8UC1, 1 / 100.0);
        ::cv::cvtColor(img8bit, imgColor, ::cv::COLOR_GRAY2RGB);

        m_writer->write(imgColor);
    }
    else if(m_imageType == CV_8UC3)
    {
        // convert the image from RGB to BGR
        ::cv::Mat imageBGR;
        ::cv::cvtColor(image, imageBGR, ::cv::COLOR_RGB2BGR);
        m_writer->write(imageBGR);
    }
    else if(m_imageType == CV_8UC4)
    {
        // convert the image from RGBA to BGR
        ::cv::Mat imageBGR;
        ::cv::cvtColor(image, imageBGR, ::cv::COLOR_RGBA2BGR);
        m_writer->write(imageBGR);
    }
    else
    {
        m_writer->write(image);
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::saveFrame(core::HiResClock::HiResClockType timestamp)
{
    if(m_isRecording)
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

        if(m_writer && m_writer->isOpened())
        {
            // Get the buffer of the copied timeline
            CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
            if(buffer)
            {
                const int width  = static_cast<int>(frameTL->getWidth());
                const int height = static_cast<int>(frameTL->getHeight());
                this->writeBuffer(width, height, buffer);
            }
        }
        else
        {
            if(m_timestamps.size() >= 5)
            {
                // computes number of fps
                const double fps = 1000 * static_cast<double>(m_timestamps.size())
                                   / (m_timestamps.back() - m_timestamps.front());
                const int width                     = static_cast<int>(frameTL->getWidth());
                const int height                    = static_cast<int>(frameTL->getHeight());
                std::filesystem::path path          = this->getFile();
                const std::string providedExtension = path.extension().string();
                std::string extensionToUse;
                std::string codec;

                // Check if file has an extension.
                if(providedExtension.empty())
                {
                    // No extension provided, add extension of selected filter.
                    extensionToUse = m_selectedExtension;
                    path          += extensionToUse;
                }
                else
                {
                    extensionToUse = providedExtension;
                }

                if(extensionToUse == s_MP4_EXTENSION)
                {
                    codec = s_AVC1_CODEC;
                }
                else
                {
                    sight::ui::base::dialog::MessageDialog::show(
                        "Video recording",
                        "The extension " + extensionToUse + " is not supported. Unable to write the file: "
                        + path.string()
                    );
                    this->stopRecord();
                    return;
                }

                m_writer =
                    std::make_unique< ::cv::VideoWriter>(
                        path.string(),
                        ::cv::VideoWriter::fourcc(
                            codec[0],
                            codec[1],
                            codec[2],
                            codec[3]
                        ),
                        fps,
                        ::cv::Size(width, height),
                        true
                    );

                if(!m_writer->isOpened())
                {
                    sight::ui::base::dialog::MessageDialog::show(
                        "Video recording",
                        "Unable to write the video in the file: " + path.string()
                    );
                    this->stopRecord();
                }
                else
                {
                    for(const auto& oldTimestamp : m_timestamps)
                    {
                        // writes the old frames used to compute the number of fps
                        CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(oldTimestamp);
                        if(buffer)
                        {
                            this->writeBuffer(width, height, buffer);
                        }
                    }
                }
            }
            else
            {
                m_timestamps.push_back(timestamp);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::startRecord()
{
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        const auto data    = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::FrameTL>(data.get_shared());

        if(frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->numComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if(frameTL->getType() == core::tools::Type::s_UINT8 && frameTL->numComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if(frameTL->getType() == core::tools::Type::s_UINT16 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frameTL->getType().string() + " with "
                + std::to_string(frameTL->numComponents()) + " components is not supported"
            );
            return;
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::stopRecord()
{
    m_isRecording = false;
    m_timestamps.clear();
    if(m_writer)
    {
        m_writer->release();
        m_writer.reset();
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::record(bool state)
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

service::IService::KeyConnectionsMap SVideoWriter::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(sight::io::base::service::s_DATA_KEY, data::FrameTL::s_OBJECT_PUSHED_SIG, s_SAVE_FRAME);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
