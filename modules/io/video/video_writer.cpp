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

const std::string video_writer::P4_EXTENSION = ".mp4";
const std::string video_writer::AV_C1_CODEC  = "avc1";

//------------------------------------------------------------------------------

video_writer::video_writer() noexcept
{
    new_slot(SAVE_FRAME, &video_writer::save_frame, this);
    new_slot(START_RECORD, &video_writer::start_record, this);
    new_slot(STOP_RECORD, &video_writer::stop_record, this);
    new_slot(RECORD, &video_writer::record, this);
    new_slot(TOGGLE_RECORDING, &video_writer::toggle_recording, this);
}

//------------------------------------------------------------------------------

video_writer::~video_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::path_type_t video_writer::get_path_type() const
{
    return sight::io::service::file;
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

void video_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose an file to save the video" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("mp4", "*.mp4");
    dialog_file.set_option(ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        m_selected_extension = dialog_file.get_selected_extensions().front();
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void video_writer::stopping()
{
    this->stop_record();
}

//------------------------------------------------------------------------------

void video_writer::updating()
{
}

//------------------------------------------------------------------------------

void video_writer::write_buffer(int _width, int _height, CSPTR(data::frame_tl::buffer_t)_buffer)
{
    SIGHT_ASSERT("OpenCV video writer not initialized", m_writer);
    const std::uint8_t* image_buffer = &_buffer->get_element(0);

    const cv::Mat image(
        cv::Size(_width, _height),
        m_image_type, const_cast<std::uint8_t*>(image_buffer), // NOLINT(cppcoreguidelines-pro-type-const-cast)
        cv::Mat::AUTO_STEP
    );
    if(m_image_type == CV_16UC1)
    {
        // Convert the image to a RGB image
        cv::Mat img8bit;
        cv::Mat img_color;
        image.convertTo(img8bit, CV_8UC1, 1 / 100.0);
        cv::cvtColor(img8bit, img_color, cv::COLOR_GRAY2RGB);

        m_writer->write(img_color);
    }
    else if(m_image_type == CV_8UC3)
    {
        // convert the image from RGB to BGR
        cv::Mat image_bgr;
        cv::cvtColor(image, image_bgr, cv::COLOR_RGB2BGR);
        m_writer->write(image_bgr);
    }
    else if(m_image_type == CV_8UC4)
    {
        // convert the image from RGBA to BGR
        cv::Mat image_bgr;
        cv::cvtColor(image, image_bgr, cv::COLOR_RGBA2BGR);
        m_writer->write(image_bgr);
    }
    else
    {
        m_writer->write(image);
    }
}

//------------------------------------------------------------------------------

void video_writer::save_frame(core::hires_clock::type _timestamp)
{
    if(m_is_recording)
    {
        // Retrieve dataStruct associated with this service
        const auto locked   = m_data.lock();
        const auto frame_tl = std::dynamic_pointer_cast<const data::frame_tl>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::frame_tl::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            frame_tl
        );

        if(m_writer && m_writer->isOpened())
        {
            // Get the buffer of the copied timeline
            CSPTR(data::frame_tl::buffer_t) buffer = frame_tl->get_closest_buffer(_timestamp);
            if(buffer)
            {
                const int width  = static_cast<int>(frame_tl->get_width());
                const int height = static_cast<int>(frame_tl->get_height());
                this->write_buffer(width, height, buffer);
            }
        }
        else
        {
            if(m_timestamps.size() >= 5)
            {
                // computes number of fps
                const double fps = 1000 * static_cast<double>(m_timestamps.size())
                                   / (m_timestamps.back() - m_timestamps.front());
                const int width                      = static_cast<int>(frame_tl->get_width());
                const int height                     = static_cast<int>(frame_tl->get_height());
                std::filesystem::path path           = this->get_file();
                const std::string provided_extension = path.extension().string();
                std::string extension_to_use;
                std::string codec;

                // Check if file has an extension.
                if(provided_extension.empty())
                {
                    // No extension provided, add extension of selected filter.
                    extension_to_use = m_selected_extension;
                    path            += extension_to_use;
                }
                else
                {
                    extension_to_use = provided_extension;
                }

                if(extension_to_use == P4_EXTENSION)
                {
                    codec = AV_C1_CODEC;
                }
                else
                {
                    sight::ui::dialog::message::show(
                        "Video recording",
                        "The extension " + extension_to_use + " is not supported. Unable to write the file: "
                        + path.string()
                    );
                    this->stop_record();
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
                    this->stop_record();
                }
                else
                {
                    for(const auto& old_timestamp : m_timestamps)
                    {
                        // writes the old frames used to compute the number of fps
                        CSPTR(data::frame_tl::buffer_t) buffer = frame_tl->get_closest_buffer(old_timestamp);
                        if(buffer)
                        {
                            this->write_buffer(width, height, buffer);
                        }
                    }
                }
            }
            else
            {
                m_timestamps.push_back(_timestamp);
            }
        }
    }
}

//------------------------------------------------------------------------------

void video_writer::start_record()
{
    if(!this->has_location_defined())
    {
        this->open_location_dialog();
    }

    if(this->has_location_defined())
    {
        const auto data     = m_data.lock();
        const auto frame_tl = std::dynamic_pointer_cast<const data::frame_tl>(data.get_shared());

        if(frame_tl->type() == core::type::UINT8 && frame_tl->num_components() == 3)
        {
            m_image_type = CV_8UC3;
        }
        else if(frame_tl->type() == core::type::UINT8 && frame_tl->num_components() == 4)
        {
            m_image_type = CV_8UC4;
        }
        else if(frame_tl->type() == core::type::UINT16 && frame_tl->num_components() == 1)
        {
            m_image_type = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frame_tl->type().name() + " with "
                + std::to_string(frame_tl->num_components()) + " components is not supported"
            );
            return;
        }

        // Make sure the parent path exists
        const std::filesystem::path dirname = this->get_file().parent_path();
        if(!std::filesystem::exists(dirname))
        {
            std::filesystem::create_directories(dirname);
        }

        m_is_recording = true;
    }
}

//------------------------------------------------------------------------------

void video_writer::stop_record()
{
    m_is_recording = false;
    m_timestamps.clear();
    if(m_writer)
    {
        m_writer->release();
        m_writer.reset();
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void video_writer::record(bool _state)
{
    if(_state)
    {
        this->start_record();
    }
    else
    {
        this->stop_record();
    }
}

//------------------------------------------------------------------------------

void video_writer::toggle_recording()
{
    if(m_is_recording)
    {
        this->stop_record();
    }
    else
    {
        this->start_record();
    }
}

//------------------------------------------------------------------------------

service::connections_t video_writer::auto_connections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::DATA_KEY, data::timeline::signals::PUSHED, SAVE_FRAME);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
