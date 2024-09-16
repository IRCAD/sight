/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "frame_writer.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/location/single_folder.hpp>

#include <data/map.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace sight::module::io::video
{

static const core::com::slots::key_t SAVE_FRAME           = "save_frame";
static const core::com::slots::key_t START_RECORD         = "start_record";
static const core::com::slots::key_t STOP_RECORD          = "stop_record";
static const core::com::slots::key_t RECORD               = "record";
static const core::com::slots::key_t TOGGLE_RECORDING     = "toggle_recording";
static const core::com::slots::key_t WRITE                = "write";
static const core::com::slots::key_t SET_FORMAT_PARAMETER = "set_format_parameter";

//------------------------------------------------------------------------------

frame_writer::frame_writer() noexcept :

    m_format(".tiff")
{
    new_slot(SAVE_FRAME, &frame_writer::save_frame, this);
    new_slot(START_RECORD, &frame_writer::start_record, this);
    new_slot(STOP_RECORD, &frame_writer::stop_record, this);
    new_slot(RECORD, &frame_writer::record, this);
    new_slot(TOGGLE_RECORDING, &frame_writer::toggle_recording, this);
    new_slot(WRITE, &frame_writer::write, this);
    new_slot(SET_FORMAT_PARAMETER, &frame_writer::set_format_parameter, this);
}

//------------------------------------------------------------------------------

frame_writer::~frame_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::path_type_t frame_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void frame_writer::configuring()
{
    sight::io::service::writer::configuring();

    service::config_t config = this->get_config();

    m_format = config.get<std::string>("format", ".tiff");
}

//------------------------------------------------------------------------------

void frame_writer::starting()
{
}

//------------------------------------------------------------------------------

void frame_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a folder to save the frames" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::write);
    dialog_file.set_type(ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void frame_writer::stopping()
{
    this->stop_record();
}

//------------------------------------------------------------------------------

void frame_writer::updating()
{
    core::clock::type timestamp = core::clock::get_time_in_milli_sec();
    this->save_frame(timestamp);
}

//------------------------------------------------------------------------------

void frame_writer::save_frame(core::clock::type _timestamp)
{
    this->start_record();
    this->write(_timestamp);
    this->stop_record();
}

//------------------------------------------------------------------------------

void frame_writer::write(core::clock::type _timestamp)
{
    if(m_is_recording)
    {
        // Retrieve dataStruct associated with this service
        const auto locked   = m_data.lock();
        const auto frame_tl = std::dynamic_pointer_cast<const data::frame_tl>(locked.get_shared());

        // The following lock causes the service to drop frames if under heavy load. This prevents desynchronization
        // between frames and timestamps.
        // TODO: experiment with queuing frames and writing them from a worker thread.
        const auto sig = frame_tl->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        core::com::connection::blocker write_blocker(sig->get_connection(m_slots[WRITE]));

        // Get the buffer of the copied timeline
        const auto buffer = frame_tl->get_closest_buffer(_timestamp);

        if(buffer)
        {
            _timestamp = buffer->get_timestamp();
            const int width  = static_cast<int>(frame_tl->get_width());
            const int height = static_cast<int>(frame_tl->get_height());

            const std::uint8_t* image_buffer = &buffer->get_element(0);

            cv::Mat image(cv::Size(width, height), m_image_type, (void*) image_buffer, cv::Mat::AUTO_STEP);

            const auto time = static_cast<std::size_t>(_timestamp);
            const std::string filename("img_" + std::to_string(time) + m_format);
            const std::filesystem::path path = this->get_folder() / filename;

            if(image.type() == CV_8UC3)
            {
                // convert the read image from BGR to RGB
                cv::Mat image_rgb;
                cv::cvtColor(image, image_rgb, cv::COLOR_BGR2RGB);
                cv::imwrite(path.string(), image_rgb);
            }
            else if(image.type() == CV_8UC4)
            {
                // convert the read image from BGRA to RGBA
                cv::Mat image_rgb;
                cv::cvtColor(image, image_rgb, cv::COLOR_BGRA2RGBA);
                cv::imwrite(path.string(), image_rgb);
            }
            else
            {
                cv::imwrite(path.string(), image);
            }
        }
    }
}

//------------------------------------------------------------------------------

void frame_writer::start_record()
{
    if(!this->has_location_defined())
    {
        this->open_location_dialog();
    }

    if(this->has_location_defined())
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

        if(frame_tl->type() == core::type::UINT8 && frame_tl->num_components() == 3)
        {
            m_image_type = CV_8UC3;
        }
        else if(frame_tl->type() == core::type::UINT8 && frame_tl->num_components() == 4)
        {
            m_image_type = CV_8UC4;
        }
        else if(frame_tl->type() == core::type::UINT8 && frame_tl->num_components() == 1)
        {
            m_image_type = CV_8UC1;
        }
        else if(frame_tl->type() == core::type::UINT16 && frame_tl->num_components() == 1)
        {
            m_image_type = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frame_tl->type().name() + " with "
                + std::to_string(frame_tl->num_components()) + " is not supported"
            );
            return;
        }

        std::filesystem::path path = this->get_folder();

        if(!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }

        m_is_recording = true;
    }
}

//------------------------------------------------------------------------------

void frame_writer::stop_record()
{
    m_is_recording = false;
}

//------------------------------------------------------------------------------

void frame_writer::toggle_recording()
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

void frame_writer::record(bool _state)
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

void frame_writer::set_format_parameter(std::string _val, std::string _key)
{
    if(_key == "format")
    {
        if(_val == ".tiff"
           || _val == ".jpeg"
           || _val == ".bmp"
           || _val == ".png"
           || _val == ".jp2")
        {
            m_format = _val;
        }
        else
        {
            SIGHT_ERROR("Value : '" + _val + "' is not supported");
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

service::connections_t frame_writer::auto_connections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::DATA_KEY, data::timeline::signals::PUSHED, WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
