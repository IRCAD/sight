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

#include "io/dicom/reader/SeriesDB.hpp"

#include "io/dicom/helper/DicomDir.hpp"
#include "io/dicom/helper/DicomSearch.hpp"
#include "io/dicom/helper/DicomSeries.hpp"
#include "io/dicom/helper/SOPClass.hpp"
#include "io/dicom/reader/Series.hpp"

#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/jobs/Observer.hpp>
#include <core/thread/ActiveWorkers.hpp>

#include <data/helper/SeriesDB.hpp>

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmMediaStorage.h>
#include <gdcmUIDs.h>

SIGHT_REGISTER_IO_READER(::sight::io::dicom::reader::SeriesDB);

namespace sight::io::dicom
{

namespace reader
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(io::base::reader::IObjectReader::Key key) :
    m_isDicomdirActivated(false),
    m_dicomFilterType(""),
    m_logger(core::log::Logger::New()),
    m_job(core::jobs::Aggregator::New("DICOM reader")),
    m_enableBufferRotation(true),
    m_dicomdirFileLookupJob(core::jobs::Observer::New("Extracting information from DICOMDIR")),
    m_regularFileLookupJob(core::jobs::Observer::New("Looking for DICOM files")),
    m_readerJob(core::jobs::Observer::New("Reading DICOM files")),
    m_completeDicomSeriesJob(core::jobs::Observer::New("Completing series")),
    m_converterJob(core::jobs::Observer::New("DICOM data conversion"))
{
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//------------------------------------------------------------------------------

void SeriesDB::read()
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
        m_logger->critical("An error has occurred during the reading process : unable to retrieve series.");

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
        filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(m_dicomFilterType);
        filter::dicom::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
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
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
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

void SeriesDB::readDicomSeries()
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
        m_logger->critical("An error has occurred during the reading process : unable to retrieve series.");

        // Finish jobs
        m_dicomdirFileLookupJob->finish();
        m_regularFileLookupJob->finish();
        m_readerJob->finish();
        m_completeDicomSeriesJob->finish();
        m_job->run().get();
        return;
    }

    data::SeriesDB::sptr seriesDB = this->getConcreteObject();
    data::helper::SeriesDB seriesDBHelper(*seriesDB);

    // Push Dicom Series
    if(!m_job->cancelRequested())
    {
        for(data::DicomSeries::sptr series : m_dicomSeriesContainer)
        {
            seriesDBHelper.add(series);
        }
    }

    try
    {
        // .get() throws exception that might have occurred
        m_job->run().get();
    }
    catch(const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
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

void SeriesDB::readDicom()
{
    // DICOMDIR
    auto dicomdir = io::dicom::helper::DicomDir::findDicomDir(this->getFolder());
    if(m_isDicomdirActivated && std::filesystem::exists(dicomdir))
    {
        // Create Dicom Series
        io::dicom::helper::DicomDir::retrieveDicomSeries(
            dicomdir,
            m_dicomSeriesContainer,
            m_logger,
            m_readerJob->progressCallback(),
            m_readerJob->cancelRequestedCallback()
        );

        // Fill Dicom Series
        io::dicom::helper::DicomSeries helper;
        helper.complete(m_dicomSeriesContainer, m_completeDicomSeriesJob);
    }

    // Finish DICOMDIR lookup
    m_dicomdirFileLookupJob->done();
    m_dicomdirFileLookupJob->finish();

    // Regular read
    if(!m_isDicomdirActivated || !std::filesystem::exists(dicomdir) || m_dicomSeriesContainer.empty())
    {
        m_readerJob->doneWork(0);

        // Recursively search for dicom files
        std::vector<std::filesystem::path> filenames;
        io::dicom::helper::DicomSearch::searchRecursively(
            this->getFolder(),
            filenames,
            true,
            m_regularFileLookupJob
        );

        // Read Dicom Series
        io::dicom::helper::DicomSeries helper;
        m_dicomSeriesContainer = helper.read(filenames, m_readerJob, m_completeDicomSeriesJob);
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

void SeriesDB::readFromDicomSeriesDB(
    const data::SeriesDB::csptr& dicomSeriesDB,
    const service::IService::sptr& notifier
)
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_converterJob);

    // Read series
    for(data::Series::sptr series : dicomSeriesDB->getContainer())
    {
        data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast(series);
        SIGHT_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        m_dicomSeriesContainer.push_back(dicomSeries);
    }

    // Apply Default filters
    if(!m_dicomFilterType.empty())
    {
        filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(m_dicomFilterType);
        filter::dicom::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
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
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
    }
    catch(...)
    {
        m_logger->critical("An unknown error has occurred during the reading process.");
    }
}

//------------------------------------------------------------------------------

bool SeriesDB::isDicomDirAvailable()
{
    auto dicomdir = io::dicom::helper::DicomDir::findDicomDir(this->getFolder());
    return std::filesystem::exists(dicomdir);
}

//------------------------------------------------------------------------------

void SeriesDB::convertDicomSeries(const service::IService::sptr& notifier)
{
    data::SeriesDB::sptr seriesDB = this->getConcreteObject();

    // Sort DicomSeries
    std::sort(m_dicomSeriesContainer.begin(), m_dicomSeriesContainer.end(), SeriesDB::dicomSeriesComparator);

    // Create reader
    SPTR(io::dicom::reader::Series) seriesReader = std::make_shared<io::dicom::reader::Series>();
    seriesReader->setBufferRotationEnabled(m_enableBufferRotation);
    seriesReader->setLogger(m_logger);

    m_converterJob->setTotalWorkUnits(m_dicomSeriesContainer.size());

    // Compute total work units
    // We do not use an Aggregator here as the jobs
    // are created after updating the main aggregator.
    std::uint64_t totalWorkUnits = 0;
    for(const data::DicomSeries::sptr& dicomSeries : m_dicomSeriesContainer)
    {
        totalWorkUnits += dicomSeries->getDicomContainer().size();
    }

    m_converterJob->setTotalWorkUnits(totalWorkUnits);

    std::uint64_t completedProgress = 0;
    auto progressCallback = [&](std::uint64_t progress)
                            {
                                m_converterJob->doneWork(completedProgress + progress);
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

        const SupportedSOPClassContainerType::iterator bIt = m_supportedSOPClassContainer.begin();
        const SupportedSOPClassContainerType::iterator eIt = m_supportedSOPClassContainer.end();

        if(m_supportedSOPClassContainer.empty() || std::find(bIt, eIt, sopClassUID) != eIt)
        {
            seriesReader->setProgressCallback(progressCallback);
            seriesReader->setCancelRequestedCallback(m_converterJob->cancelRequestedCallback());
            try
            {
                data::Series::sptr series = seriesReader->read(dicomSeries);

                if(series)
                {
                    // Add the series to the DB
                    data::helper::SeriesDB seriesDBHelper(*seriesDB);
                    seriesDBHelper.add(series);

                    if(notifier)
                    {
                        seriesDBHelper.notify();
                    }
                }
            }
            catch(io::dicom::exception::Failed& e)
            {
                m_logger->critical("Unable to read series : " + dicomSeries->getInstanceUID());
            }
        }
        else
        {
            const std::string sopClassName = io::dicom::helper::SOPClass::getSOPClassName(sopClassUID);
            m_logger->critical("DICOM SOP Class \"" + sopClassName + "\" is not supported by the selected reader.");
        }

        if(m_job->cancelRequested())
        {
            break;
        }

        completedProgress = m_converterJob->getDoneWorkUnits();
    }

    m_converterJob->done();
    m_converterJob->finish();
}

//------------------------------------------------------------------------------

bool SeriesDB::dicomSeriesComparator(
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
        (::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR
         || ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR
         || aSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                 // "::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) ==
                                                            // ::gdcm::MediaStorage::Comprehensive3DSR"
         ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage
         || ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage);

    const bool bIsAnImage =
        (::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR
         || ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR
         || bSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" // FIXME Replace hard coded string by
         ||                                                 // "::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) ==
                                                            // ::gdcm::MediaStorage::Comprehensive3DSR"
         ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage
         || ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage);

    return bIsAnImage && !aIsAnImage;
}

//------------------------------------------------------------------------------

SeriesDB::DicomSeriesContainerType& SeriesDB::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

//------------------------------------------------------------------------------

SPTR(core::jobs::IJob) SeriesDB::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::dicom
