/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "matrix_writer.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>

namespace sight::module::io::matrix
{

static const core::com::slots::key_t SAVE_MATRIX  = "saveMatrix";
static const core::com::slots::key_t START_RECORD = "start_record";
static const core::com::slots::key_t STOP_RECORD  = "stop_record";
static const core::com::slots::key_t WRITE        = "write";

//------------------------------------------------------------------------------

matrix_writer::matrix_writer() noexcept
{
    new_slot(SAVE_MATRIX, &matrix_writer::save_matrix, this);
    new_slot(START_RECORD, &matrix_writer::start_record, this);
    new_slot(STOP_RECORD, &matrix_writer::stop_record, this);
    new_slot(WRITE, &matrix_writer::write, this);
    new_slot(SET_BASE_FOLDER, &matrix_writer::set_base_folder, this);
}

//------------------------------------------------------------------------------

matrix_writer::~matrix_writer() noexcept
{
    this->stop_record();
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t matrix_writer::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

void matrix_writer::configuring()
{
    sight::io::service::writer::configuring();

    const auto& config = this->get_config();

    if(const auto& config_child = config.get_child_optional("config"); config_child)
    {
        m_interactive = config_child->get<bool>("<xmlattr>.interactive", true);
    }
}

//------------------------------------------------------------------------------

void matrix_writer::starting()
{
}

//------------------------------------------------------------------------------

void matrix_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a folder to save the csv file" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::write);
    dialog_file.set_type(ui::dialog::location::single_file);
    dialog_file.add_filter(".csv file", "*.csv");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
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

void matrix_writer::stopping()
{
    this->stop_record();
}

//------------------------------------------------------------------------------

void matrix_writer::updating()
{
    const auto& timestamp = core::hires_clock::get_time_in_milli_sec();
    this->save_matrix(timestamp);
}

//------------------------------------------------------------------------------

void matrix_writer::save_matrix(core::hires_clock::type _timestamp)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    this->start_record();
    this->write(_timestamp);
    this->stop_record();
}

//------------------------------------------------------------------------------

void matrix_writer::write(core::hires_clock::type _timestamp)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    m_write_failed = true;

    if(!m_is_recording)
    {
        // No need to write if not recording
        return;
    }

    const auto& locked   = m_data.lock();
    const auto matrix_tl = std::dynamic_pointer_cast<const data::matrix_tl>(locked.get_shared());

    SIGHT_ASSERT(
        "The object is not a '"
        + data::matrix_tl::classname()
        + "' or '"
        + sight::io::service::DATA_KEY
        + "' is not correctly set.",
        matrix_tl
    );

    const unsigned int number_of_mat = matrix_tl->get_max_element_num();

    // Get the buffer of the copied timeline
    if(const auto& object = matrix_tl->get_closest_object(_timestamp); object)
    {
        if(const auto& buffer = std::dynamic_pointer_cast<const data::matrix_tl::buffer_t>(object); buffer)
        {
            _timestamp = object->get_timestamp();
            const auto time = static_cast<std::size_t>(_timestamp);
            m_filestream << time << ";";

            for(unsigned int i = 0 ; i < number_of_mat ; ++i)
            {
                const std::array<float, 16>& values = buffer->get_element(i);

                for(unsigned int v = 0 ; v < 16 ; ++v)
                {
                    m_filestream << values[v] << ";";
                }
            }

            m_filestream << std::endl;
        }
    }

    m_write_failed = false;
}

//------------------------------------------------------------------------------

void matrix_writer::start_record()
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // No need to start if already recording
    if(m_is_recording)
    {
        return;
    }

    // Default mode when opening a file is in append mode
    std::ios_base::openmode open_mode = std::ofstream::app;
    if(m_interactive && !this->has_location_defined())
    {
        this->open_location_dialog();

        // In trunc mode, any contents that existed in the file before it is open are discarded.
        // This is the needed behavior when opening the file for the first time.
        open_mode = std::ofstream::trunc;
    }

    if(this->has_location_defined())
    {
        // Make sure the parent path exists
        const std::filesystem::path dirname = this->get_file().parent_path();
        if(!std::filesystem::exists(dirname))
        {
            std::filesystem::create_directories(dirname);
        }

        try
        {
            if(!m_filestream.is_open())
            {
                m_filestream.open(this->get_file().string(), std::ofstream::out | open_mode);
                m_filestream.precision(7);
                m_filestream << std::fixed;
            }

            // Check if the file is open and in good state
            m_is_recording = m_filestream.good();

            SIGHT_ERROR_IF(
                "The file " + this->get_file().string()
                + " can't be opened. Please check if it is already open in another program.",
                !m_is_recording
            );
        }
        catch(const std::exception& e)
        {
            m_is_recording = false;

            SIGHT_ERROR("The file " + this->get_file().string() + " can't be opened. " + e.what());
        }
    }
    else if(!m_interactive)
    {
        // This will allow to start recording once the file and / or base folder has been set...
        m_is_recording = true;
    }
    else
    {
        SIGHT_WARN("The output location has not been defined. The recording will not start.");
    }
}

//------------------------------------------------------------------------------

void matrix_writer::stop_record()
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // No need to stop recording if already stopped
    if(!m_is_recording)
    {
        return;
    }

    try
    {
        if(m_filestream.is_open())
        {
            m_filestream.flush();
            m_filestream.close();
        }

        m_is_recording = false;
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("The file " + this->get_file().string() + " can't be closed. " + e.what());
    }
}

//------------------------------------------------------------------------------

void matrix_writer::set_base_folder(std::string _path)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // If the service is recording, stop it and restart it after changing the base folder
    const bool was_recording = m_is_recording;

    if(m_is_recording)
    {
        this->stop_record();
    }

    // Set the base folder
    sight::io::service::writer::set_base_folder(_path);

    if(was_recording)
    {
        this->start_record();
    }
}

//------------------------------------------------------------------------------

service::connections_t matrix_writer::auto_connections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::DATA_KEY, data::timeline::signals::PUSHED, WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::matrix
