/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/helper/dicom_series_writer.hpp"

#include "io/dicom/helper/dicom_anonymizer.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/dicom_series.hpp>

#include <io/zip/write_archive.hpp>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

dicom_series_writer::dicom_series_writer() :
    m_job(std::make_shared<core::jobs::observer>("Writing DICOM files"))
{
}

//------------------------------------------------------------------------------

void dicom_series_writer::set_anonymizer(const SPTR(helper::dicom_anonymizer)& _anonymizer)
{
    m_anonymizer = _anonymizer;
}

//------------------------------------------------------------------------------

void dicom_series_writer::set_output_archive(const io::zip::write_archive::sptr& _archive, const std::string& _sub_path)
{
    m_archive  = _archive;
    m_sub_path = _sub_path;
}

//------------------------------------------------------------------------------

void dicom_series_writer::write()
{
    if(m_archive)
    {
        this->process_write_archive();
    }
    else
    {
        this->process_write();
    }
}

//------------------------------------------------------------------------------

std::string dicom_series_writer::get_filename(const std::size_t& _instance_index)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(7);
    if(m_anonymizer)
    {
        ss << m_anonymizer->get_next_index();
    }
    else
    {
        ss << _instance_index;
    }

    return ss.str();
}

//------------------------------------------------------------------------------

void dicom_series_writer::process_stream(std::istream& _input_stream, std::ostream& _output_stream)
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

void dicom_series_writer::process_write()
{
    data::dicom_series::csptr dicom_series = this->get_concrete_object();

    // Create folder
    std::filesystem::path folder = this->get_folder();

    if(m_write_count != 0)
    {
        std::stringstream ss;
        ss << "fwGdcmIO" << std::setfill('0') << std::setw(3) << m_write_count;
        folder /= ss.str();
    }

    if(!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }

    std::size_t nb_instances = dicom_series->num_instances();

    m_job->set_total_work_units(nb_instances);
    unsigned int count = 0;

    // Write binary files
    for(const auto& value : dicom_series->get_dicom_container())
    {
        if(m_job->cancel_requested())
        {
            return;
        }

        const std::string filename = this->get_filename(value.first);

        const core::memory::buffer_object::sptr source_buffer = value.second;
        core::memory::buffer_object::lock_t source_locker(source_buffer);
        const core::memory::buffer_manager::stream_info& stream_info = source_buffer->get_stream_info();
        SPTR(std::istream) stream = stream_info.stream;

        const std::filesystem::path& dest_dir = m_anonymizer ? folder / m_sub_path : folder;

        if(!std::filesystem::exists(dest_dir))
        {
            std::filesystem::create_directories(dest_dir);
        }

        const std::filesystem::path& dest_file = dest_dir / filename;

        std::ofstream fs(dest_file, std::ios::binary | std::ios::trunc);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs.good());

        this->process_stream(*(stream), fs);

        m_job->done_work(++count);
    }

    m_job->finish();
    m_write_count++;
}

//------------------------------------------------------------------------------

void dicom_series_writer::process_write_archive()
{
    SIGHT_ASSERT("Output archive shall be set", m_archive);

    data::dicom_series::csptr dicom_series = this->get_concrete_object();

    const std::size_t nb_instances = dicom_series->num_instances();
    unsigned int count             = 0;

    m_job->set_total_work_units(nb_instances);

    for(const auto& value : dicom_series->get_dicom_container())
    {
        if(m_job->cancel_requested())
        {
            break;
        }

        const std::string filename = this->get_filename(value.first);

        const core::memory::buffer_object::sptr source_buffer = value.second;
        core::memory::buffer_object::lock_t source_locker(source_buffer);
        const core::memory::buffer_manager::stream_info& stream_info = source_buffer->get_stream_info();
        SPTR(std::istream) stream = stream_info.stream;

        const std::filesystem::path& dest_dir =
            m_anonymizer ? m_sub_path : "";

        const std::filesystem::path& dest_file = dest_dir / filename;
        SPTR(std::ostream) fs = m_archive->create_file(dest_file);
        SIGHT_THROW_IF("Can't open '" << dest_file.string() << "' for write.", !fs->good());

        this->process_stream(*(stream), *fs);

        m_job->done_work(++count);
    }

    m_job->done();
    m_job->finish();
    ++m_write_count;
}

//------------------------------------------------------------------------------

std::string dicom_series_writer::extension() const
{
    return "";
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) dicom_series_writer::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
