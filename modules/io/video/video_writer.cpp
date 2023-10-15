/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "video_writer.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::io::video
{

static const core::com::slots::key_t SAVE_FRAME       = "saveFrame";
static const core::com::slots::key_t START_RECORD     = "startRecord";
static const core::com::slots::key_t STOP_RECORD      = "stopRecord";
static const core::com::slots::key_t RECORD           = "record";
static const core::com::slots::key_t TOGGLE_RECORDING = "toggleRecording";

const std::string video_writer::s_MP4_EXTENSION = ".mp4";
const std::string video_writer::s_AVC1_CODEC    = "avc1";

//------------------------------------------------------------------------------

video_writer::video_writer() noexcept
{
    new_slot(SAVE_FRAME, &video_writer::saveFrame, this);
    new_slot(START_RECORD, &video_writer::startRecord, this);
    new_slot(STOP_RECORD, &video_writer::stopRecord, this);
    new_slot(RECORD, &video_writer::record, this);
    new_slot(TOGGLE_RECORDING, &video_writer::toggleRecording, this);
}

//------------------------------------------------------------------------------

video_writer::~video_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType video_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void video_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void video_writer::starting()
{
}

//------------------------------------------------------------------------------

void video_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an file to save the video" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("mp4", "*.mp4");
    dialogFile.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        m_selectedExtension = dialogFile.getSelectedExtensions().front();
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void video_writer::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void video_writer::updating()
{
}

//------------------------------------------------------------------------------

void video_writer::writeBuffer(int width, int height, CSPTR(data::frame_tl::BufferType)buffer)
{
    SIGHT_ASSERT("OpenCV video writer not initialized", m_writer);
    const std::uint8_t* imageBuffer = &buffer->getElement(0);

    const cv::Mat image(
        cv::Size(width, height),
        m_imageType, const_cast<std::uint8_t*>(imageBuffer), // NOLINT(cppcoreguidelines-pro-type-const-cast)
        cv::Mat::AUTO_STEP
    );
    if(m_imageType == CV_16UC1)
    {
        // Convert the image to a RGB image
        cv::Mat img8bit;
        cv::Mat imgColor;
        image.convertTo(img8bit, CV_8UC1, 1 / 100.0);
        cv::cvtColor(img8bit, imgColor, cv::COLOR_GRAY2RGB);

        m_writer->write(imgColor);
    }
    else if(m_imageType == CV_8UC3)
    {
        // convert the image from RGB to BGR
        cv::Mat imageBGR;
        cv::cvtColor(image, imageBGR, cv::COLOR_RGB2BGR);
        m_writer->write(imageBGR);
    }
    else if(m_imageType == CV_8UC4)
    {
        // convert the image from RGBA to BGR
        cv::Mat imageBGR;
        cv::cvtColor(image, imageBGR, cv::COLOR_RGBA2BGR);
        m_writer->write(imageBGR);
    }
    else
    {
        m_writer->write(image);
    }
}

//------------------------------------------------------------------------------

void video_writer::saveFrame(core::hires_clock::type timestamp)
{
    if(m_isRecording)
    {
        // Retrieve dataStruct associated with this service
        const auto locked  = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::frame_tl>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::frame_tl::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            frameTL
        );

        if(m_writer && m_writer->isOpened())
        {
            // Get the buffer of the copied timeline
            CSPTR(data::frame_tl::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
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
                std::filesystem::path path          = this->get_file();
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
                    sight::ui::dialog::message::show(
                        "Video recording",
                        "The extension " + extensionToUse + " is not supported. Unable to write the file: "
                        + path.string()
                    );
                    this->stopRecord();
                    return;
                }

                m_writer =
                    std::make_unique<cv::VideoWriter>(
                        path.string(),
                        cv::VideoWriter::fourcc(
                            codec[0],
                            codec[1],
                            codec[2],
                            codec[3]
                        ),
                        fps,
                        cv::Size(width, height),
                        true
                    );

                if(!m_writer->isOpened())
                {
                    sight::ui::dialog::message::show(
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
                        CSPTR(data::frame_tl::BufferType) buffer = frameTL->getClosestBuffer(oldTimestamp);
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

void video_writer::startRecord()
{
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        const auto data    = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::frame_tl>(data.get_shared());

        if(frameTL->getType() == core::type::UINT8 && frameTL->numComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if(frameTL->getType() == core::type::UINT8 && frameTL->numComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if(frameTL->getType() == core::type::UINT16 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frameTL->getType().name() + " with "
                + std::to_string(frameTL->numComponents()) + " components is not supported"
            );
            return;
        }

        // Make sure the parent path exists
        const std::filesystem::path dirname = this->get_file().parent_path();
        if(!std::filesystem::exists(dirname))
        {
            std::filesystem::create_directories(dirname);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void video_writer::stopRecord()
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

void video_writer::record(bool state)
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

void video_writer::toggleRecording()
{
    if(m_isRecording)
    {
        this->stopRecord();
    }
    else
    {
        this->startRecord();
    }
}

//------------------------------------------------------------------------------

service::connections_t video_writer::auto_connections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::s_DATA_KEY, data::timeline::signals::PUSHED, SAVE_FRAME);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
