/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/helper/DicomSeriesWriter.hpp"

#include "io/dicom/helper/DicomAnonymizer.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <data/DicomSeries.hpp>

#include <io/base/writer/registry/macros.hpp>
#include <io/zip/IWriteArchive.hpp>

#include <boost/foreach.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::helper::DicomSeriesWriter);

namespace sight::io::dicom
{

namespace helper
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter(io::base::writer::IObjectWriter::Key) :
    m_writeCount(0),
    m_job(core::jobs::Observer::New("Writing DICOM files"))
{
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer)
{
    m_anonymizer = anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setOutputArchive(const io::zip::IWriteArchive::sptr& archive, const std::string& subPath)
{
    m_archive = archive;
    m_subPath = subPath;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::write()
{
    if(m_archive)
    {
        this->processWriteArchive();
    }
    else
    {
        this->processWrite();
    }
}

//------------------------------------------------------------------------------

std::string DicomSeriesWriter::getFilename(const size_t& instanceIndex)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(7);
    if(m_anonymizer)
    {
        ss << m_anonymizer->getNextIndex();
    }
    else
    {
        ss << instanceIndex;
    }

    return ss.str();
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processStream(std::istream& inputStream, std::ostream& outputStream)
{
    if(m_anonymizer)
    {
        m_anonymizer->anonymize(inputStream, outputStream);
    }
    else
    {
        outputStream << inputStream.rdbuf();
    }
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWrite()
{
    data::DicomSeries::csptr dicomSeries = this->getConcreteObject();

    // Create folder
    std::filesystem::path folder = this->getFolder();

    if(m_writeCount)
    {
        std::stringstream ss;
        ss << "fwGdcmIO" << std::setfill('0') << std::setw(3) << m_writeCount;
        folder /= ss.str();
    }

    if(!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }

    std::size_t nbInstances = dicomSeries->getNumberOfInstances();

    m_job->setTotalWorkUnits(nbInstances);
    unsigned int count = 0;

    // Write binary files
    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancelRequested())
        {
            return;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::BufferObject::sptr sourceBuffer = value.second;
        core::memory::BufferObject::Lock sourceLocker(sourceBuffer);
        const core::memory::BufferManager::StreamInfo& streamInfo = sourceBuffer->getStreamInfo();
        SPTR(std::istream) stream = streamInfo.stream;

        const std::filesystem::path& dest_dir = m_anonymizer ? folder / m_subPath : folder;

        if(!std::filesystem::exists(dest_dir))
        {
            std::filesystem::create_directories(dest_dir);
        }

        const std::filesystem::path& dest_file = dest_dir / filename;

        std::ofstream fs(dest_file, std::ios::binary | std::ios::trunc);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs.good());

        this->processStream(*(stream.get()), fs);

        m_job->doneWork(++count);
    }

    m_job->finish();
    m_writeCount++;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWriteArchive()
{
    SIGHT_ASSERT("Output archive shall be set", m_archive);

    data::DicomSeries::csptr dicomSeries = this->getConcreteObject();

    const size_t nbInstances = dicomSeries->getNumberOfInstances();
    unsigned int count       = 0;

    m_job->setTotalWorkUnits(nbInstances);

    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancelRequested())
        {
            break;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::BufferObject::sptr sourceBuffer = value.second;
        core::memory::BufferObject::Lock sourceLocker(sourceBuffer);
        const core::memory::BufferManager::StreamInfo& streamInfo = sourceBuffer->getStreamInfo();
        SPTR(std::istream) stream = streamInfo.stream;

        const std::filesystem::path& dest_dir =
            m_anonymizer ? m_subPath : "";

        const std::filesystem::path& dest_file = dest_dir / filename;
        SPTR(std::ostream) fs = m_archive->createFile(dest_file);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs->good());

        this->processStream(*(stream.get()), *fs);

        m_job->doneWork(++count);
    }

    m_job->done();
    m_job->finish();
    ++m_writeCount;
}

//------------------------------------------------------------------------------

std::string DicomSeriesWriter::extension() const
{
    return "";
}

//------------------------------------------------------------------------------

SPTR(core::jobs::IJob) DicomSeriesWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::io::dicom
