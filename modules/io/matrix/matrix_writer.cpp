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
static const core::com::slots::key_t START_RECORD = "startRecord";
static const core::com::slots::key_t STOP_RECORD  = "stopRecord";
static const core::com::slots::key_t WRITE        = "write";

//------------------------------------------------------------------------------

matrix_writer::matrix_writer() noexcept
{
    new_slot(SAVE_MATRIX, &matrix_writer::saveMatrix, this);
    new_slot(START_RECORD, &matrix_writer::startRecord, this);
    new_slot(STOP_RECORD, &matrix_writer::stopRecord, this);
    new_slot(WRITE, &matrix_writer::write, this);
    new_slot(SET_BASE_FOLDER, &matrix_writer::setBaseFolder, this);
}

//------------------------------------------------------------------------------

matrix_writer::~matrix_writer() noexcept
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType matrix_writer::getIOPathType() const
{
    return sight::io::service::FILE;
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

void matrix_writer::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a folder to save the csv file" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(ui::dialog::location::WRITE);
    dialog_file.setType(ui::dialog::location::SINGLE_FILE);
    dialog_file.addFilter(".csv file", "*.csv");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void matrix_writer::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void matrix_writer::updating()
{
    const auto& timestamp = core::hires_clock::get_time_in_milli_sec();
    this->saveMatrix(timestamp);
}

//------------------------------------------------------------------------------

void matrix_writer::saveMatrix(core::hires_clock::type _timestamp)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void matrix_writer::write(core::hires_clock::type _timestamp)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    m_writeFailed = true;

    if(!m_isRecording)
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
        + sight::io::service::s_DATA_KEY
        + "' is not correctly set.",
        matrix_tl
    );

    const unsigned int number_of_mat = matrix_tl->getMaxElementNum();

    // Get the buffer of the copied timeline
    if(const auto& object = matrix_tl->getClosestObject(_timestamp); object)
    {
        if(const auto& buffer = std::dynamic_pointer_cast<const data::matrix_tl::buffer_t>(object); buffer)
        {
            _timestamp = object->getTimestamp();
            const auto time = static_cast<std::size_t>(_timestamp);
            m_filestream << time << ";";

            for(unsigned int i = 0 ; i < number_of_mat ; ++i)
            {
                const std::array<float, 16>& values = buffer->getElement(i);

                for(unsigned int v = 0 ; v < 16 ; ++v)
                {
                    m_filestream << values[v] << ";";
                }
            }

            m_filestream << std::endl;
        }
    }

    m_writeFailed = false;
}

//------------------------------------------------------------------------------

void matrix_writer::startRecord()
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // No need to start if already recording
    if(m_isRecording)
    {
        return;
    }

    // Default mode when opening a file is in append mode
    std::ios_base::openmode open_mode = std::ofstream::app;
    if(m_interactive && !this->hasLocationDefined())
    {
        this->openLocationDialog();

        // In trunc mode, any contents that existed in the file before it is open are discarded.
        // This is the needed behavior when opening the file for the first time.
        open_mode = std::ofstream::trunc;
    }

    if(this->hasLocationDefined())
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
            m_isRecording = m_filestream.good();

            SIGHT_ERROR_IF(
                "The file " + this->get_file().string()
                + " can't be opened. Please check if it is already open in another program.",
                !m_isRecording
            );
        }
        catch(const std::exception& e)
        {
            m_isRecording = false;

            SIGHT_ERROR("The file " + this->get_file().string() + " can't be opened. " + e.what());
        }
    }
    else if(!m_interactive)
    {
        // This will allow to start recording once the file and / or base folder has been set...
        m_isRecording = true;
    }
    else
    {
        SIGHT_WARN("The output location has not been defined. The recording will not start.");
    }
}

//------------------------------------------------------------------------------

void matrix_writer::stopRecord()
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // No need to stop recording if already stopped
    if(!m_isRecording)
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

        m_isRecording = false;
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("The file " + this->get_file().string() + " can't be closed. " + e.what());
    }
}

//------------------------------------------------------------------------------

void matrix_writer::setBaseFolder(std::string _path)
{
    // Protect operation
    std::unique_lock lock(m_mutex);

    // If the service is recording, stop it and restart it after changing the base folder
    const bool was_recording = m_isRecording;

    if(m_isRecording)
    {
        this->stopRecord();
    }

    // Set the base folder
    sight::io::service::writer::setBaseFolder(_path);

    if(was_recording)
    {
        this->startRecord();
    }
}

//------------------------------------------------------------------------------

service::connections_t matrix_writer::auto_connections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::s_DATA_KEY, data::timeline::signals::PUSHED, WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::matrix
