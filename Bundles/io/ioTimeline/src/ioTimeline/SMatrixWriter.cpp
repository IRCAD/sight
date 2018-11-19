/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

#include <boost/filesystem/operations.hpp>

namespace ioTimeline
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioTimeline::SMatrixWriter, ::arData::MatrixTL);

static const ::fwCom::Slots::SlotKeyType s_SAVE_MATRIX  = "saveMatrix";
static const ::fwCom::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";
static const ::fwCom::Slots::SlotKeyType s_WRITE        = "write";

//------------------------------------------------------------------------------

SMatrixWriter::SMatrixWriter() noexcept :
    m_isRecording(false),
    m_filestream(nullptr)
{
    newSlot(s_SAVE_MATRIX, &SMatrixWriter::saveMatrix, this);
    newSlot(s_START_RECORD, &SMatrixWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SMatrixWriter::stopRecord, this);
    newSlot(s_WRITE, &SMatrixWriter::write, this);
}

//------------------------------------------------------------------------------

SMatrixWriter::~SMatrixWriter() noexcept
{
    if(nullptr != m_filestream)
    {
        m_filestream->close();
        delete m_filestream;
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
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the csv file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(_sDefaultPath);

        if(nullptr != m_filestream)
        {
            m_filestream->close();
        }

        m_filestream = new std::ofstream(this->getFile().string());
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

    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    this->saveMatrix(timestamp);
}

//------------------------------------------------------------------------------

void SMatrixWriter::saveMatrix(::fwCore::HiResClock::HiResClockType _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SMatrixWriter::write(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording)
    {
        ::arData::MatrixTL::csptr matrixTL = this->getInput< ::arData::MatrixTL >(::fwIO::s_DATA_KEY);

        unsigned int numberOfMat = matrixTL->getMaxElementNum();
        // Get the buffer of the copied timeline

        CSPTR(::arData::timeline::Object) object = matrixTL->getClosestObject(timestamp);
        if(object)
        {
            CSPTR(::arData::MatrixTL::BufferType) buffer =
                std::dynamic_pointer_cast< const ::arData::MatrixTL::BufferType >(object);
            if (buffer)
            {
                timestamp = object->getTimestamp();
                size_t time = static_cast<size_t>(timestamp);
                *m_filestream << time <<";";
                for(unsigned int i = 0; i < numberOfMat; ++i)
                {
                    const float* values = buffer->getElement(i);
                    for(unsigned int v = 0; v < 16; ++v)
                    {
                        *m_filestream << values[v] << ";";
                    }
                }
                *m_filestream << std::endl;
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMatrixWriter::startRecord()
{
    if (!this->hasLocationDefined())
    {
        this->configureWithIHM();
    }

    if (this->hasLocationDefined())
    {
        if(nullptr == m_filestream)
        {
            std::string file = this->getFile().string();

            m_filestream = new std::ofstream(file);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SMatrixWriter::stopRecord()
{
    m_isRecording = false;
    if(nullptr != m_filestream)
    {
        m_filestream->flush();
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
