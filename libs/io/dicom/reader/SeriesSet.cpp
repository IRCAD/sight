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

#include "io/dicom/reader/SeriesSet.hpp"

#include "io/dicom/helper/DicomDir.hpp"
#include "io/dicom/helper/DicomSearch.hpp"
#include "io/dicom/helper/DicomSeries.hpp"
#include "io/dicom/helper/SOPClass.hpp"
#include "io/dicom/reader/Series.hpp"

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

SIGHT_REGISTER_IO_READER(sight::io::dicom::reader::SeriesSet);

namespace sight::io::dicom::reader
{

//------------------------------------------------------------------------------

SeriesSet::SeriesSet() :
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

void SeriesSet::read()
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

void SeriesSet::readDicomSeries()
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

void SeriesSet::readDicom()
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

void SeriesSet::readFromDicomSeriesSet(
    const data::SeriesSet::csptr& dicom_series_set,
    const service::base::sptr& notifier
)
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_converterJob);

    // Read series
    for(const auto& series : *dicom_series_set)
    {
        const auto& dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(series);
        SIGHT_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        m_dicomSeriesContainer.push_back(dicomSeries);
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
        this->convertDicomSeries(notifier);
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

bool SeriesSet::isDicomDirAvailable()
{
    auto dicomdir = io::dicom::helper::DicomDir::findDicomDir(this->get_folder());
    return std::filesystem::exists(dicomdir);
}

//------------------------------------------------------------------------------

void SeriesSet::convertDicomSeries(const service::base::sptr& notifier)
{
    auto series_set = this->getConcreteObject();

    // Sort DicomSeries
    std::sort(m_dicomSeriesContainer.begin(), m_dicomSeriesContainer.end(), SeriesSet::dicomSeriesComparator);

    // Create reader
    SPTR(io::dicom::reader::Series) seriesReader = std::make_shared<io::dicom::reader::Series>();
    seriesReader->setBufferRotationEnabled(m_enableBufferRotation);
    seriesReader->setLogger(m_logger);

    m_converterJob->set_total_work_units(m_dicomSeriesContainer.size());

    // Compute total work units
    // We do not use an aggregator here as the jobs
    // are created after updating the main aggregator.
    std::uint64_t totalWorkUnits = 0;
    for(const data::DicomSeries::sptr& dicomSeries : m_dicomSeriesContainer)
    {
        totalWorkUnits += dicomSeries->getDicomContainer().size();
    }

    m_converterJob->set_total_work_units(totalWorkUnits);

    std::uint64_t completedProgress = 0;
    auto progress_callback = [&](std::uint64_t progress)
                             {
                                 m_converterJob->done_work(completedProgress + progress);
                             };

    // Read series
    for(const data::DicomSeries::csptr dicomSeries : m_dicomSeriesContainer)
    {
        data::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
        SIGHT_THROW_IF(
            "The series contains several SOPClassUIDs. Try to apply a filter in order to split the series.",
            sopClassUIDContainer.size() != 1
        );
        const std::string sopClassUID = sopClassUIDContainer.begin()->c_str();

        const auto& bIt = m_supportedSOPClassContainer.begin();
        const auto& eIt = m_supportedSOPClassContainer.end();

        if(m_supportedSOPClassContainer.empty() || std::find(bIt, eIt, sopClassUID) != eIt)
        {
            seriesReader->setProgressCallback(progress_callback);
            seriesReader->setCancelRequestedCallback(m_converterJob->cancel_requested_callback());
            try
            {
                data::Series::sptr series = seriesReader->read(dicomSeries);

                if(series)
                {
                    // Add the series to the DB
                    const auto series_scoped_emitter = notifier ? series_set->scoped_emit() : nullptr;
                    series_set->push_back(series);
                }
            }
            catch(io::dicom::exception::Failed& e)
            {
                m_logger->critical("Unable to read series '" + dicomSeries->getSeriesInstanceUID() + "': " + e.what());
            }
        }
        else
        {
            const std::string sopClassName = io::dicom::helper::SOPClass::getSOPClassName(sopClassUID);
            m_logger->critical("DICOM SOP Class \"" + sopClassName + "\" is not supported by the selected reader.");
        }

        if(m_job->cancel_requested())
        {
            break;
        }

        completedProgress = m_converterJob->get_done_work_units();
    }

    m_converterJob->done();
    m_converterJob->finish();
}

//------------------------------------------------------------------------------

bool SeriesSet::dicomSeriesComparator(
    const SPTR(data::DicomSeries)& a,
    const SPTR(data::DicomSeries)& b
)
{
    const data::DicomSeries::SOPClassUIDContainerType aSOPClassUIDContainer = a->getSOPClassUIDs();
    const std::string aSOPClassUID                                          = *(aSOPClassUIDContainer.begin());
    const data::DicomSeries::SOPClassUIDContainerType bSOPClassUIDContainer = b->getSOPClassUIDs();
    const std::string bSOPClassUID                                          = *(bSOPClassUIDContainer.begin());

    // a > b if a contains a SR and not b
    const bool aIsAnImage =
        (gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == gdcm::MediaStorage::EnhancedSR
         || gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == gdcm::MediaStorage::ComprehensiveSR
         || aSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                 // "gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) ==
                                                            // gdcm::MediaStorage::Comprehensive3DSR"
         gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == gdcm::MediaStorage::SpacialFiducialsStorage
         || gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == gdcm::MediaStorage::SurfaceSegmentationStorage);

    const bool bIsAnImage =
        (gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == gdcm::MediaStorage::EnhancedSR
         || gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == gdcm::MediaStorage::ComprehensiveSR
         || bSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                 // "gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) ==
                                                            // gdcm::MediaStorage::Comprehensive3DSR"
         gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == gdcm::MediaStorage::SpacialFiducialsStorage
         || gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == gdcm::MediaStorage::SurfaceSegmentationStorage);

    return bIsAnImage && !aIsAnImage;
}

//------------------------------------------------------------------------------

SeriesSet::DicomSeriesContainerType& SeriesSet::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) SeriesSet::getJob() const
{
    return m_job;
}

} // namespace sight::io::dicom::reader
