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

void DicomSeriesWriter::setAnonymizer(const SPTR(helper::DicomAnonymizer)& _anonymizer)
{
    m_anonymizer = _anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setOutputArchive(const io::zip::write_archive::sptr& _archive, const std::string& _sub_path)
{
    m_archive = _archive;
    m_subPath = _sub_path;
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

std::string DicomSeriesWriter::getFilename(const std::size_t& _instance_index)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(7);
    if(m_anonymizer)
    {
        ss << m_anonymizer->getNextIndex();
    }
    else
    {
        ss << _instance_index;
    }

    return ss.str();
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processStream(std::istream& _input_stream, std::ostream& _output_stream)
{
    if(m_anonymizer)
    {
        m_anonymizer->anonymize(_input_stream, _output_stream);
    }
    else
    {
        _output_stream << _input_stream.rdbuf();
    }
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWrite()
{
    data::dicom_series::csptr dicom_series = this->getConcreteObject();

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

    std::size_t nb_instances = dicom_series->numInstances();

    m_job->set_total_work_units(nb_instances);
    unsigned int count = 0;

    // Write binary files
    for(const auto& value : dicom_series->getDicomContainer())
    {
        if(m_job->cancel_requested())
        {
            return;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::buffer_object::sptr source_buffer = value.second;
        core::memory::buffer_object::lock_t source_locker(source_buffer);
        const core::memory::buffer_manager::stream_info& stream_info = source_buffer->get_stream_info();
        SPTR(std::istream) stream = stream_info.stream;

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

    data::dicom_series::csptr dicom_series = this->getConcreteObject();

    const std::size_t nb_instances = dicom_series->numInstances();
    unsigned int count             = 0;

    m_job->set_total_work_units(nb_instances);

    for(const auto& value : dicom_series->getDicomContainer())
    {
        if(m_job->cancel_requested())
        {
            break;
        }

        const std::string filename = this->getFilename(value.first);

        const core::memory::buffer_object::sptr source_buffer = value.second;
        core::memory::buffer_object::lock_t source_locker(source_buffer);
        const core::memory::buffer_manager::stream_info& stream_info = source_buffer->get_stream_info();
        SPTR(std::istream) stream = stream_info.stream;

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
