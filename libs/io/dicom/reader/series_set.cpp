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

#include "io/dicom/reader/series_set.hpp"

#include "io/dicom/helper/dicom_dir.hpp"
#include "io/dicom/helper/dicom_search.hpp"
#include "io/dicom/helper/dicom_series.hpp"
#include "io/dicom/helper/sop_class.hpp"
#include "io/dicom/reader/series.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/filter.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmMediaStorage.h>
#include <gdcmUIDs.h>

namespace sight::io::dicom::reader
{

//------------------------------------------------------------------------------

series_set::series_set() :
    m_is_dicomdir_activated(false),
    m_logger(std::make_shared<core::log::logger>()),
    m_job(std::make_shared<core::jobs::aggregator>("DICOM reader")),
    m_enable_buffer_rotation(true),
    m_dicomdir_file_lookup_job(std::make_shared<core::jobs::observer>("Extracting information from DICOMDIR")),
    m_regular_file_lookup_job(std::make_shared<core::jobs::observer>("Looking for DICOM files")),
    m_reader_job(std::make_shared<core::jobs::observer>("Reading DICOM files")),
    m_complete_dicom_series_job(std::make_shared<core::jobs::observer>("Completing series")),
    m_converter_job(std::make_shared<core::jobs::observer>("DICOM data conversion"))
{
}

//------------------------------------------------------------------------------

void series_set::read()
{
    // Clear DicomSeries container
    m_dicom_series_container.clear();

    m_job->add(m_dicomdir_file_lookup_job);
    m_job->add(m_regular_file_lookup_job);
    m_job->add(m_reader_job);
    m_job->add(m_complete_dicom_series_job);
    m_job->add(m_converter_job);

    try
    {
        this->read_dicom();
    }
    catch(const std::exception& e)
    {
        m_logger->clear();
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));

        // Finish jobs
        m_dicomdir_file_lookup_job->finish();
        m_regular_file_lookup_job->finish();
        m_reader_job->finish();
        m_complete_dicom_series_job->finish();
        m_converter_job->finish();
        m_job->run().get();
        return;
    }

    // Apply Default filters
    if(!m_dicom_filter_type.empty())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(m_dicom_filter_type);
        sight::filter::dicom::helper::filter::apply_filter(m_dicom_series_container, filter, true, m_logger);
    }

    if(m_dicom_series_container.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
        m_converter_job->done();
        m_converter_job->finish();
    }
    else
    {
        // Read series
        this->convert_dicom_series();
    }

    try
    {
        // .get() throws exception that might have occurred
        m_job->run().get();
    }
    catch(const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));
    }
    catch(...)
    {
        m_logger->critical("An unknown error has occurred during the reading process.");
    }

    if(m_dicom_series_container.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
    }
}

//------------------------------------------------------------------------------

void series_set::read_dicom_series()
{
    // Clear DicomSeries container
    m_dicom_series_container.clear();

    m_job->add(m_dicomdir_file_lookup_job);
    m_job->add(m_regular_file_lookup_job);
    m_job->add(m_reader_job);
    m_job->add(m_complete_dicom_series_job);

    try
    {
        this->read_dicom();
    }
    catch(const std::exception& e)
    {
        m_logger->clear();
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));

        // Finish jobs
        m_dicomdir_file_lookup_job->finish();
        m_regular_file_lookup_job->finish();
        m_reader_job->finish();
        m_complete_dicom_series_job->finish();
        m_job->run().get();
        return;
    }

    const auto& series_set    = get_concrete_object();
    const auto scoped_emitter = series_set->scoped_emit();

    // Push Dicom Series
    if(!m_job->cancel_requested())
    {
        for(const auto& series : m_dicom_series_container)
        {
            series_set->push_back(series);
        }
    }

    try
    {
        // .get() throws exception that might have occurred
        m_job->run().get();
    }
    catch(const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));
    }
    catch(...)
    {
        m_logger->critical("An unknown error has occurred during the reading process.");
    }

    if(m_dicom_series_container.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
    }
}

//------------------------------------------------------------------------------

void series_set::read_dicom()
{
    // DICOMDIR
    auto dicomdir = io::dicom::helper::dicom_dir::find_dicom_dir(this->get_folder());
    if(m_is_dicomdir_activated && std::filesystem::exists(dicomdir))
    {
        // Create Dicom Series
        io::dicom::helper::dicom_dir::retrieve_dicom_series(
            dicomdir,
            m_dicom_series_container,
            m_logger,
            m_reader_job->progress_callback(),
            m_reader_job->cancel_requested_callback()
        );

        // Fill Dicom Series
        io::dicom::helper::dicom_series::complete(m_dicom_series_container, m_complete_dicom_series_job);
    }

    // Finish DICOMDIR lookup
    m_dicomdir_file_lookup_job->done();
    m_dicomdir_file_lookup_job->finish();

    // Regular read
    if(!m_is_dicomdir_activated || !std::filesystem::exists(dicomdir) || m_dicom_series_container.empty())
    {
        m_reader_job->done_work(0);

        // Recursively search for dicom files
        std::vector<std::filesystem::path> filenames;
        io::dicom::helper::dicom_search::search_recursively(
            this->get_folder(),
            filenames,
            true,
            m_regular_file_lookup_job
        );

        // Read Dicom Series
        m_dicom_series_container = io::dicom::helper::dicom_series::read(
            filenames,
            m_reader_job,
            m_complete_dicom_series_job
        );
    }

    // Finish regular lookup
    m_regular_file_lookup_job->done();
    m_regular_file_lookup_job->finish();

    // Finish reading
    m_reader_job->done();
    m_reader_job->finish();

    // Finish completing series
    m_complete_dicom_series_job->done();
    m_complete_dicom_series_job->finish();
}

//------------------------------------------------------------------------------

void series_set::read_from_dicom_series_set(
    const data::series_set::csptr& _dicom_series_set,
    const service::base::sptr& _notifier
)
{
    // Clear DicomSeries container
    m_dicom_series_container.clear();

    m_job->add(m_converter_job);

    // Read series
    for(const auto& series : *_dicom_series_set)
    {
        const auto& dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);
        SIGHT_ASSERT("Trying to read a series which is not a DicomSeries.", dicom_series);
        m_dicom_series_container.push_back(dicom_series);
    }

    // Apply Default filters
    if(!m_dicom_filter_type.empty())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(m_dicom_filter_type);
        sight::filter::dicom::helper::filter::apply_filter(m_dicom_series_container, filter, true, m_logger);
    }

    if(m_dicom_series_container.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
        m_converter_job->done();
        m_converter_job->finish();
    }
    else
    {
        // Read series
        this->convert_dicom_series(_notifier);
    }

    try
    {
        // .get() throws exception that might have occurred
        m_job->run().get();
    }
    catch(const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));
    }
    catch(...)
    {
        m_logger->critical("An unknown error has occurred during the reading process.");
    }
}

//------------------------------------------------------------------------------

bool series_set::is_dicom_dir_available()
{
    auto dicomdir = io::dicom::helper::dicom_dir::find_dicom_dir(this->get_folder());
    return std::filesystem::exists(dicomdir);
}

//------------------------------------------------------------------------------

void series_set::convert_dicom_series(const service::base::sptr& _notifier)
{
    auto series_set = this->get_concrete_object();

    // Sort DicomSeries
    std::sort(m_dicom_series_container.begin(), m_dicom_series_container.end(), series_set::dicom_series_comparator);

    // Create reader
    auto series_reader = std::make_shared<io::dicom::reader::series>();
    series_reader->set_buffer_rotation_enabled(m_enable_buffer_rotation);
    series_reader->set_logger(m_logger);

    m_converter_job->set_total_work_units(m_dicom_series_container.size());

    // Compute total work units
    // We do not use an aggregator here as the jobs
    // are created after updating the main aggregator.
    std::uint64_t total_work_units = 0;
    for(const data::dicom_series::sptr& dicom_series : m_dicom_series_container)
    {
        total_work_units += dicom_series->get_dicom_container().size();
    }

    m_converter_job->set_total_work_units(total_work_units);

    std::uint64_t completed_progress = 0;
    auto progress_callback = [&](std::uint64_t _progress)
                             {
                                 m_converter_job->done_work(completed_progress + _progress);
                             };

    // Read series
    for(const data::dicom_series::csptr dicom_series : m_dicom_series_container)
    {
        data::dicom_series::sop_class_uid_container_t sop_class_uid_container = dicom_series->get_sop_class_ui_ds();
        SIGHT_THROW_IF(
            "The series contains several sop_classUIDs. Try to apply a filter in order to split the series.",
            sop_class_uid_container.size() != 1
        );
        const std::string sop_class_uid = sop_class_uid_container.begin()->c_str();

        const auto& b_it = m_supported_sop_class_container.begin();
        const auto& e_it = m_supported_sop_class_container.end();

        if(m_supported_sop_class_container.empty() || std::find(b_it, e_it, sop_class_uid) != e_it)
        {
            series_reader->set_progress_callback(progress_callback);
            series_reader->set_cancel_requested_callback(m_converter_job->cancel_requested_callback());
            try
            {
                data::series::sptr series = series_reader->read(dicom_series);

                if(series)
                {
                    // Add the series to the DB
                    const auto series_scoped_emitter = _notifier ? series_set->scoped_emit() : nullptr;
                    series_set->push_back(series);
                }
            }
            catch(io::dicom::exception::failed& e)
            {
                m_logger->critical(
                    "Unable to read series '" + dicom_series->get_series_instance_uid() + "': " + e.what()
                );
            }
        }
        else
        {
            const std::string sop_class_name = io::dicom::helper::sop_class::get_sop_class_name(sop_class_uid);
            m_logger->critical("DICOM SOP Class \"" + sop_class_name + "\" is not supported by the selected reader.");
        }

        if(m_job->cancel_requested())
        {
            break;
        }

        completed_progress = m_converter_job->get_done_work_units();
    }

    m_converter_job->done();
    m_converter_job->finish();
}

//------------------------------------------------------------------------------

bool series_set::dicom_series_comparator(const SPTR(data::dicom_series)& _a, const SPTR(data::dicom_series)& _b)
{
    const data::dicom_series::sop_class_uid_container_t a_sop_class_uid_container = _a->get_sop_class_ui_ds();
    const std::string a_sop_class_uid                                             =
        *(a_sop_class_uid_container.begin());
    const data::dicom_series::sop_class_uid_container_t b_sop_class_uid_container = _b->get_sop_class_ui_ds();
    const std::string b_sop_class_uid                                             =
        *(b_sop_class_uid_container.begin());

    // a > b if a contains a SR and not b
    const bool a_is_an_image =
        (gdcm::MediaStorage::GetMSType(a_sop_class_uid.c_str()) == gdcm::MediaStorage::EnhancedSR
         || gdcm::MediaStorage::GetMSType(a_sop_class_uid.c_str()) == gdcm::MediaStorage::ComprehensiveSR
         || a_sop_class_uid == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                    // "gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str())
                                                               // ==
                                                               // gdcm::MediaStorage::Comprehensive3DSR"
         gdcm::MediaStorage::GetMSType(a_sop_class_uid.c_str()) == gdcm::MediaStorage::SpacialFiducialsStorage
         || gdcm::MediaStorage::GetMSType(a_sop_class_uid.c_str()) == gdcm::MediaStorage::SurfaceSegmentationStorage);

    const bool b_is_an_image =
        (gdcm::MediaStorage::GetMSType(b_sop_class_uid.c_str()) == gdcm::MediaStorage::EnhancedSR
         || gdcm::MediaStorage::GetMSType(b_sop_class_uid.c_str()) == gdcm::MediaStorage::ComprehensiveSR
         || b_sop_class_uid == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                    // "gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str())
                                                               // ==
                                                               // gdcm::MediaStorage::Comprehensive3DSR"
         gdcm::MediaStorage::GetMSType(b_sop_class_uid.c_str()) == gdcm::MediaStorage::SpacialFiducialsStorage
         || gdcm::MediaStorage::GetMSType(a_sop_class_uid.c_str()) == gdcm::MediaStorage::SurfaceSegmentationStorage);

    return b_is_an_image && !a_is_an_image;
}

//------------------------------------------------------------------------------

series_set::dicom_series_container_t& series_set::get_dicom_series()
{
    return m_dicom_series_container;
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) series_set::get_job() const
{
    return m_job;
}

} // namespace sight::io::dicom::reader
