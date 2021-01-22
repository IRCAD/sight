/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "ioTimeline/SMatrixWriter.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>

namespace ioTimeline
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioTimeline::SMatrixWriter, ::arData::MatrixTL)

static const ::fwCom::Slots::SlotKeyType s_SAVE_MATRIX = "saveMatrix";
static const ::fwCom::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";
static const ::fwCom::Slots::SlotKeyType s_WRITE        = "write";

//------------------------------------------------------------------------------

SMatrixWriter::SMatrixWriter() noexcept :
    m_isRecording(false)
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

::fwIO::IOPathType SMatrixWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SMatrixWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SMatrixWriter::starting()
{
}

//------------------------------------------------------------------------------

void SMatrixWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SMatrixWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the csv file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if(result)
    {
        _sDefaultPath = result->getPath();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(_sDefaultPath);
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
        ::arData::MatrixTL::csptr matrixTL = this->getInput< ::arData::MatrixTL >(::fwIO::s_DATA_KEY);

        const unsigned int numberOfMat = matrixTL->getMaxElementNum();

        // Get the buffer of the copied timeline
        CSPTR(::arData::timeline::Object) object = matrixTL->getClosestObject(timestamp);
        if(object)
        {
            CSPTR(::arData::MatrixTL::BufferType) buffer =
                std::dynamic_pointer_cast< const ::arData::MatrixTL::BufferType >(object);
            if(buffer)
            {
                timestamp = object->getTimestamp();
                const size_t time = static_cast<size_t>(timestamp);
                m_filestream << time <<";";
                for(unsigned int i = 0; i < numberOfMat; ++i)
                {
                    const float* values = buffer->getElement(i);

                    for(unsigned int v = 0; v < 16; ++v)
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
        if(!m_filestream.is_open())
        {
            m_filestream.open(this->getFile().string(), std::ofstream::out | openMode);
            m_filestream.precision(7);
            m_filestream << std::fixed;
            m_isRecording = true;
        }
        else
        {
            SLM_WARN(
                "The file " + this->getFile().string() +
                " can't be open. Please check if it is already open in another program.");
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

::fwServices::IService::KeyConnectionsMap SMatrixWriter::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(::fwIO::s_DATA_KEY, ::arData::MatrixTL::s_OBJECT_PUSHED_SIG, s_WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace ioTimeline
