/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/dicom_series.hpp>

#include <io/__/writer/registry/macros.hpp>
#include <io/zip/write_archive.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::helper::DicomSeriesWriter);

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter() :
    m_job(std::make_shared<core::jobs::observer>("Writing DICOM files"))
{
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer)
{
    m_anonymizer = anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setOutputArchive(const io::zip::write_archive::sptr& archive, const std::string& subPath)
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

std::string DicomSeriesWriter::getFilename(const std::size_t& instanceIndex)
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
    data::dicom_series::csptr dicomSeries = this->getConcreteObject();

    // Create folder
    std::filesystem::path folder = this->get_folder();

    if(m_writeCount != 0)
    {
        std::stringstream ss;
        ss << "fwGdcmIO" << std::setfill('0') << std::setw(3) << m_writeCount;
        folder /= ss.str();
    }

    if(!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }

    std::size_t nb_instances = dicomSeries->numInstances();

    m_job->set_total_work_units(nb_instances);
    unsigned int count = 0;

    // Write binary files
    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancel_requested())
        {
            return;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::buffer_object::sptr sourceBuffer = value.second;
        core::memory::buffer_object::lock_t sourceLocker(sourceBuffer);
        const core::memory::buffer_manager::stream_info& streamInfo = sourceBuffer->get_stream_info();
        SPTR(std::istream) stream = streamInfo.stream;

        const std::filesystem::path& dest_dir = m_anonymizer ? folder / m_subPath : folder;

        if(!std::filesystem::exists(dest_dir))
        {
            std::filesystem::create_directories(dest_dir);
        }

        const std::filesystem::path& dest_file = dest_dir / filename;

        std::ofstream fs(dest_file, std::ios::binary | std::ios::trunc);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs.good());

        this->processStream(*(stream), fs);

        m_job->done_work(++count);
    }

    m_job->finish();
    m_writeCount++;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWriteArchive()
{
    SIGHT_ASSERT("Output archive shall be set", m_archive);

    data::dicom_series::csptr dicomSeries = this->getConcreteObject();

    const std::size_t nb_instances = dicomSeries->numInstances();
    unsigned int count             = 0;

    m_job->set_total_work_units(nb_instances);

    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancel_requested())
        {
            break;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::buffer_object::sptr sourceBuffer = value.second;
        core::memory::buffer_object::lock_t sourceLocker(sourceBuffer);
        const core::memory::buffer_manager::stream_info& streamInfo = sourceBuffer->get_stream_info();
        SPTR(std::istream) stream = streamInfo.stream;

        const std::filesystem::path& dest_dir =
            m_anonymizer ? m_subPath : "";

        const std::filesystem::path& dest_file = dest_dir / filename;
        SPTR(std::ostream) fs = m_archive->createFile(dest_file);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs->good());

        this->processStream(*(stream), *fs);

        m_job->done_work(++count);
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

SPTR(core::jobs::base) DicomSeriesWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
