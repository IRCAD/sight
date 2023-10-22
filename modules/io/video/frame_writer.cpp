/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include <data/composite.hpp>

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

static const core::com::slots::key_t SAVE_FRAME           = "saveFrame";
static const core::com::slots::key_t START_RECORD         = "startRecord";
static const core::com::slots::key_t STOP_RECORD          = "stopRecord";
static const core::com::slots::key_t RECORD               = "record";
static const core::com::slots::key_t TOGGLE_RECORDING     = "toggleRecording";
static const core::com::slots::key_t WRITE                = "write";
static const core::com::slots::key_t SET_FORMAT_PARAMETER = "setFormatParameter";

//------------------------------------------------------------------------------

frame_writer::frame_writer() noexcept :

    m_format(".tiff")
{
    new_slot(SAVE_FRAME, &frame_writer::saveFrame, this);
    new_slot(START_RECORD, &frame_writer::startRecord, this);
    new_slot(STOP_RECORD, &frame_writer::stopRecord, this);
    new_slot(RECORD, &frame_writer::record, this);
    new_slot(TOGGLE_RECORDING, &frame_writer::toggleRecording, this);
    new_slot(WRITE, &frame_writer::write, this);
    new_slot(SET_FORMAT_PARAMETER, &frame_writer::setFormatParameter, this);
}

//------------------------------------------------------------------------------

frame_writer::~frame_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType frame_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
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

void frame_writer::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a folder to save the frames" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(ui::dialog::location::WRITE);
    dialog_file.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void frame_writer::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void frame_writer::updating()
{
    core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();
    this->saveFrame(timestamp);
}

//------------------------------------------------------------------------------

void frame_writer::saveFrame(core::hires_clock::type _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void frame_writer::write(core::hires_clock::type _timestamp)
{
    if(m_isRecording)
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
        const auto buffer = frame_tl->getClosestBuffer(_timestamp);

        if(buffer)
        {
            _timestamp = buffer->getTimestamp();
            const int width  = static_cast<int>(frame_tl->getWidth());
            const int height = static_cast<int>(frame_tl->getHeight());

            const std::uint8_t* image_buffer = &buffer->getElement(0);

            cv::Mat image(cv::Size(width, height), m_imageType, (void*) image_buffer, cv::Mat::AUTO_STEP);

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

void frame_writer::startRecord()
{
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked   = m_data.lock();
        const auto frame_tl = std::dynamic_pointer_cast<const data::frame_tl>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::frame_tl::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            frame_tl
        );

        if(frame_tl->getType() == core::type::UINT8 && frame_tl->numComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if(frame_tl->getType() == core::type::UINT8 && frame_tl->numComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if(frame_tl->getType() == core::type::UINT8 && frame_tl->numComponents() == 1)
        {
            m_imageType = CV_8UC1;
        }
        else if(frame_tl->getType() == core::type::UINT16 && frame_tl->numComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frame_tl->getType().name() + " with "
                + std::to_string(frame_tl->numComponents()) + " is not supported"
            );
            return;
        }

        std::filesystem::path path = this->get_folder();

        if(!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void frame_writer::stopRecord()
{
    m_isRecording = false;
}

//------------------------------------------------------------------------------

void frame_writer::toggleRecording()
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

void frame_writer::record(bool _state)
{
    if(_state)
    {
        this->startRecord();
    }
    else
    {
        this->stopRecord();
    }
}

//------------------------------------------------------------------------------

void frame_writer::setFormatParameter(std::string _val, std::string _key)
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
    connections.push(sight::io::service::s_DATA_KEY, data::timeline::signals::PUSHED, WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
