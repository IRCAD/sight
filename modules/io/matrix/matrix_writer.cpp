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
}

//------------------------------------------------------------------------------

matrix_writer::~matrix_writer() noexcept
{
    if(m_filestream.is_open())
    {
        m_filestream.flush();
        m_filestream.close();
    }
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
}

//------------------------------------------------------------------------------

void matrix_writer::starting()
{
}

//------------------------------------------------------------------------------

void matrix_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the csv file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
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

void matrix_writer::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void matrix_writer::updating()
{
    core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();
    this->saveMatrix(timestamp);
}

//------------------------------------------------------------------------------

void matrix_writer::saveMatrix(core::hires_clock::type _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void matrix_writer::write(core::hires_clock::type timestamp)
{
    if(m_isRecording)
    {
        const auto locked   = m_data.lock();
        const auto matrixTL = std::dynamic_pointer_cast<const data::matrix_tl>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::matrix_tl::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            matrixTL
        );

        const unsigned int numberOfMat = matrixTL->getMaxElementNum();

        // Get the buffer of the copied timeline
        CSPTR(data::timeline::object) object = matrixTL->getClosestObject(timestamp);
        if(object)
        {
            CSPTR(data::matrix_tl::BufferType) buffer =
                std::dynamic_pointer_cast<const data::matrix_tl::BufferType>(object);
            if(buffer)
            {
                timestamp = object->getTimestamp();
                const auto time = static_cast<std::size_t>(timestamp);
                m_filestream << time << ";";
                for(unsigned int i = 0 ; i < numberOfMat ; ++i)
                {
                    const std::array<float, 16> values = buffer->getElement(i);

                    for(unsigned int v = 0 ; v < 16 ; ++v)
                    {
                        m_filestream << values[v] << ";";
                    }
                }

                m_filestream << std::endl;
            }
        }
    }
}

//------------------------------------------------------------------------------

void matrix_writer::startRecord()
{
    // Default mode when opening a file is in append mode
    std::ios_base::openmode openMode = std::ofstream::app;
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();

        // In trunc mode, any contents that existed in the file before it is open are discarded.
        // This is the needed behavior when opening the file for the first time.
        openMode = std::ofstream::trunc;
    }

    if(this->hasLocationDefined())
    {
        // Make sure the parent path exists
        const std::filesystem::path dirname = this->get_file().parent_path();
        if(!std::filesystem::exists(dirname))
        {
            std::filesystem::create_directories(dirname);
        }

        if(!m_filestream.is_open())
        {
            m_filestream.open(this->get_file().string(), std::ofstream::out | openMode);
            m_filestream.precision(7);
            m_filestream << std::fixed;
            m_isRecording = true;
        }
        else
        {
            SIGHT_WARN(
                "The file " + this->get_file().string()
                + " can't be opened. Please check if it is already open in another program."
            );
        }
    }
}

//------------------------------------------------------------------------------

void matrix_writer::stopRecord()
{
    m_isRecording = false;
    if(m_filestream.is_open())
    {
        m_filestream.flush();
        m_filestream.close();
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
