/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "SMatrixWriter.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>

namespace sight::module::io::matrix
{

static const core::com::Slots::SlotKeyType s_SAVE_MATRIX  = "saveMatrix";
static const core::com::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const core::com::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";
static const core::com::Slots::SlotKeyType s_WRITE        = "write";

//------------------------------------------------------------------------------

SMatrixWriter::SMatrixWriter() noexcept
{
    newSlot(s_SAVE_MATRIX, &SMatrixWriter::saveMatrix, this);
    newSlot(s_START_RECORD, &SMatrixWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SMatrixWriter::stopRecord, this);
    newSlot(s_WRITE, &SMatrixWriter::write, this);
}

//------------------------------------------------------------------------------

SMatrixWriter::~SMatrixWriter() noexcept
{
    if(m_filestream.is_open())
    {
        m_filestream.flush();
        m_filestream.close();
    }
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SMatrixWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SMatrixWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SMatrixWriter::starting()
{
}

//------------------------------------------------------------------------------

void SMatrixWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();
    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the csv file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
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

void SMatrixWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SMatrixWriter::updating()
{
    core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    this->saveMatrix(timestamp);
}

//------------------------------------------------------------------------------

void SMatrixWriter::saveMatrix(core::HiResClock::HiResClockType _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SMatrixWriter::write(core::HiResClock::HiResClockType timestamp)
{
    if(m_isRecording)
    {
        const auto locked   = m_data.lock();
        const auto matrixTL = std::dynamic_pointer_cast<const data::MatrixTL>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::MatrixTL::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            matrixTL
        );

        const unsigned int numberOfMat = matrixTL->getMaxElementNum();

        // Get the buffer of the copied timeline
        CSPTR(data::timeline::Object) object = matrixTL->getClosestObject(timestamp);
        if(object)
        {
            CSPTR(data::MatrixTL::BufferType) buffer =
                std::dynamic_pointer_cast<const data::MatrixTL::BufferType>(object);
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

void SMatrixWriter::startRecord()
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
        const std::filesystem::path dirname = this->getFile().parent_path();
        if(!std::filesystem::exists(dirname))
        {
            std::filesystem::create_directories(dirname);
        }

        if(!m_filestream.is_open())
        {
            m_filestream.open(this->getFile().string(), std::ofstream::out | openMode);
            m_filestream.precision(7);
            m_filestream << std::fixed;
            m_isRecording = true;
        }
        else
        {
            SIGHT_WARN(
                "The file " + this->getFile().string()
                + " can't be opened. Please check if it is already open in another program."
            );
        }
    }
}

//------------------------------------------------------------------------------

void SMatrixWriter::stopRecord()
{
    m_isRecording = false;
    if(m_filestream.is_open())
    {
        m_filestream.flush();
        m_filestream.close();
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMatrixWriter::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(sight::io::base::service::s_DATA_KEY, data::MatrixTL::s_OBJECT_PUSHED_SIG, s_WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::matrix
