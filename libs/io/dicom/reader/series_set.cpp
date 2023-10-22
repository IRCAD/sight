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

#include "io/dicom/reader/series_set.hpp"

#include "io/dicom/helper/DicomDir.hpp"
#include "io/dicom/helper/DicomSearch.hpp"
#include "io/dicom/helper/DicomSeries.hpp"
#include "io/dicom/helper/sop_class.hpp"
#include "io/dicom/reader/series.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/Filter.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmMediaStorage.h>
#include <gdcmUIDs.h>

SIGHT_REGISTER_IO_READER(sight::io::dicom::reader::series_set);

namespace sight::io::dicom::reader
{

//------------------------------------------------------------------------------

series_set::series_set() :
    m_isDicomdirActivated(false),
    m_logger(std::make_shared<core::log::logger>()),
    m_job(std::make_shared<core::jobs::aggregator>("DICOM reader")),
    m_enableBufferRotation(true),
    m_dicomdirFileLookupJob(std::make_shared<core::jobs::observer>("Extracting information from DICOMDIR")),
    m_regularFileLookupJob(std::make_shared<core::jobs::observer>("Looking for DICOM files")),
    m_readerJob(std::make_shared<core::jobs::observer>("Reading DICOM files")),
    m_completeDicomSeriesJob(std::make_shared<core::jobs::observer>("Completing series")),
    m_converterJob(std::make_shared<core::jobs::observer>("DICOM data conversion"))
{
}

//------------------------------------------------------------------------------

void series_set::read()
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_dicomdirFileLookupJob);
    m_job->add(m_regularFileLookupJob);
    m_job->add(m_readerJob);
    m_job->add(m_completeDicomSeriesJob);
    m_job->add(m_converterJob);

    try
    {
        this->readDicom();
    }
    catch(const std::exception& e)
    {
        m_logger->clear();
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));

        // Finish jobs
        m_dicomdirFileLookupJob->finish();
        m_regularFileLookupJob->finish();
        m_readerJob->finish();
        m_completeDicomSeriesJob->finish();
        m_converterJob->finish();
        m_job->run().get();
        return;
    }

    // Apply Default filters
    if(!m_dicomFilterType.empty())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(m_dicomFilterType);
        sight::filter::dicom::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
    }

    if(m_dicomSeriesContainer.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
        m_converterJob->done();
        m_converterJob->finish();
    }
    else
    {
        // Read series
        this->convertDicomSeries();
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

    if(m_dicomSeriesContainer.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
    }
}

//------------------------------------------------------------------------------

void series_set::readDicomSeries()
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_dicomdirFileLookupJob);
    m_job->add(m_regularFileLookupJob);
    m_job->add(m_readerJob);
    m_job->add(m_completeDicomSeriesJob);

    try
    {
        this->readDicom();
    }
    catch(const std::exception& e)
    {
        m_logger->clear();
        m_logger->critical("An error has occurred during the reading process: " + std::string(e.what()));

        // Finish jobs
        m_dicomdirFileLookupJob->finish();
        m_regularFileLookupJob->finish();
        m_readerJob->finish();
        m_completeDicomSeriesJob->finish();
        m_job->run().get();
        return;
    }

    const auto& series_set    = getConcreteObject();
    const auto scoped_emitter = series_set->scoped_emit();

    // Push Dicom Series
    if(!m_job->cancel_requested())
    {
        for(const auto& series : m_dicomSeriesContainer)
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

    if(m_dicomSeriesContainer.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
    }
}

//------------------------------------------------------------------------------

void series_set::readDicom()
{
    // DICOMDIR
    auto dicomdir = io::dicom::helper::DicomDir::findDicomDir(this->get_folder());
    if(m_isDicomdirActivated && std::filesystem::exists(dicomdir))
    {
        // Create Dicom Series
        io::dicom::helper::DicomDir::retrieveDicomSeries(
            dicomdir,
            m_dicomSeriesContainer,
            m_logger,
            m_readerJob->progress_callback(),
            m_readerJob->cancel_requested_callback()
        );

        // Fill Dicom Series
        io::dicom::helper::DicomSeries::complete(m_dicomSeriesContainer, m_completeDicomSeriesJob);
    }

    // Finish DICOMDIR lookup
    m_dicomdirFileLookupJob->done();
    m_dicomdirFileLookupJob->finish();

    // Regular read
    if(!m_isDicomdirActivated || !std::filesystem::exists(dicomdir) || m_dicomSeriesContainer.empty())
    {
        m_readerJob->done_work(0);

        // Recursively search for dicom files
        std::vector<std::filesystem::path> filenames;
        io::dicom::helper::DicomSearch::searchRecursively(
            this->get_folder(),
            filenames,
            true,
            m_regularFileLookupJob
        );

        // Read Dicom Series
        m_dicomSeriesContainer = io::dicom::helper::DicomSeries::read(
            filenames,
            m_readerJob,
            m_completeDicomSeriesJob
        );
    }

    // Finish regular lookup
    m_regularFileLookupJob->done();
    m_regularFileLookupJob->finish();

    // Finish reading
    m_readerJob->done();
    m_readerJob->finish();

    // Finish completing series
    m_completeDicomSeriesJob->done();
    m_completeDicomSeriesJob->finish();
}

//------------------------------------------------------------------------------

void series_set::readFromDicomSeriesSet(
    const data::series_set::csptr& _dicom_series_set,
    const service::base::sptr& _notifier
)
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_converterJob);

    // Read series
    for(const auto& series : *_dicom_series_set)
    {
        const auto& dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);
        SIGHT_ASSERT("Trying to read a series which is not a DicomSeries.", dicom_series);
        m_dicomSeriesContainer.push_back(dicom_series);
    }

    // Apply Default filters
    if(!m_dicomFilterType.empty())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(m_dicomFilterType);
        sight::filter::dicom::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
    }

    if(m_dicomSeriesContainer.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
        m_converterJob->done();
        m_converterJob->finish();
    }
    else
    {
        // Read series
        this->convertDicomSeries(_notifier);
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

bool series_set::isDicomDirAvailable()
{
    auto dicomdir = io::dicom::helper::DicomDir::findDicomDir(this->get_folder());
    return std::filesystem::exists(dicomdir);
}

//------------------------------------------------------------------------------

void series_set::convertDicomSeries(const service::base::sptr& _notifier)
{
    auto series_set = this->getConcreteObject();

    // Sort DicomSeries
    std::sort(m_dicomSeriesContainer.begin(), m_dicomSeriesContainer.end(), series_set::dicomSeriesComparator);

    // Create reader
    auto series_reader = std::make_shared<io::dicom::reader::series>();
    series_reader->setBufferRotationEnabled(m_enableBufferRotation);
    series_reader->setLogger(m_logger);

    m_converterJob->set_total_work_units(m_dicomSeriesContainer.size());

    // Compute total work units
    // We do not use an aggregator here as the jobs
    // are created after updating the main aggregator.
    std::uint64_t total_work_units = 0;
    for(const data::dicom_series::sptr& dicom_series : m_dicomSeriesContainer)
    {
        total_work_units += dicom_series->getDicomContainer().size();
    }

    m_converterJob->set_total_work_units(total_work_units);

    std::uint64_t completed_progress = 0;
    auto progress_callback = [&](std::uint64_t _progress)
                             {
                                 m_converterJob->done_work(completed_progress + _progress);
                             };

    // Read series
    for(const data::dicom_series::csptr dicom_series : m_dicomSeriesContainer)
    {
        data::dicom_series::sop_classUIDContainerType sop_class_uid_container = dicom_series->getSOPClassUIDs();
        SIGHT_THROW_IF(
            "The series contains several sop_classUIDs. Try to apply a filter in order to split the series.",
            sop_class_uid_container.size() != 1
        );
        const std::string sop_class_uid = sop_class_uid_container.begin()->c_str();

        const auto& b_it = m_supportedSOPClassContainer.begin();
        const auto& e_it = m_supportedSOPClassContainer.end();

        if(m_supportedSOPClassContainer.empty() || std::find(b_it, e_it, sop_class_uid) != e_it)
        {
            series_reader->setProgressCallback(progress_callback);
            series_reader->setCancelRequestedCallback(m_converterJob->cancel_requested_callback());
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
            catch(io::dicom::exception::Failed& e)
            {
                m_logger->critical("Unable to read series '" + dicom_series->getSeriesInstanceUID() + "': " + e.what());
            }
        }
        else
        {
            const std::string sop_class_name = io::dicom::helper::sop_class::getSOPClassName(sop_class_uid);
            m_logger->critical("DICOM SOP Class \"" + sop_class_name + "\" is not supported by the selected reader.");
        }

        if(m_job->cancel_requested())
        {
            break;
        }

        completed_progress = m_converterJob->get_done_work_units();
    }

    m_converterJob->done();
    m_converterJob->finish();
}

//------------------------------------------------------------------------------

bool series_set::dicomSeriesComparator(const SPTR(data::dicom_series)& _a, const SPTR(data::dicom_series)& _b)
{
    const data::dicom_series::sop_classUIDContainerType a_sop_class_uid_container = _a->getSOPClassUIDs();
    const std::string a_sop_class_uid                                             =
        *(a_sop_class_uid_container.begin());
    const data::dicom_series::sop_classUIDContainerType b_sop_class_uid_container = _b->getSOPClassUIDs();
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

series_set::dicom_series_container_t& series_set::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) series_set::getJob() const
{
    return m_job;
}

} // namespace sight::io::dicom::reader
