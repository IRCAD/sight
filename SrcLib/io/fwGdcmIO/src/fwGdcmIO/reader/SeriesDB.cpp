/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/SeriesDB.hpp"

#include "fwGdcmIO/helper/DicomDir.hpp"
#include "fwGdcmIO/helper/DicomSearch.hpp"
#include "fwGdcmIO/helper/DicomSeries.hpp"
#include "fwGdcmIO/helper/SOPClass.hpp"
#include "fwGdcmIO/reader/Series.hpp"

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmAttribute.h>
#include <gdcmDirectory.h>
#include <gdcmMediaStorage.h>
#include <gdcmUIDs.h>

fwDataIOReaderRegisterMacro( ::fwGdcmIO::reader::SeriesDB );

namespace fwGdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_isDicomdirActivated(false),
    m_dicomFilterType(""),
    m_logger(::fwLog::Logger::New()),
    m_job(::fwJobs::Aggregator::New("DICOM reader")),
    m_enableBufferRotation(true),
    m_dicomdirFileLookupJob(::fwJobs::Observer::New("Extracting information from DICOMDIR")),
    m_regularFileLookupJob(::fwJobs::Observer::New("Looking for DICOM files")),
    m_readerJob(::fwJobs::Observer::New("Reading DICOM files")),
    m_completeDicomSeriesJob(::fwJobs::Observer::New("Completing series")),
    m_converterJob(::fwJobs::Observer::New("DICOM data convertion"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
    SLM_TRACE_FUNC();
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
    catch (const std::exception& e)
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
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(m_dicomFilterType);
        ::fwDicomIOFilter::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
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
    catch (const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
    }
    catch( ... )
    {
        m_logger->critical("An unkown error has occurred during the reading process.");
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
    catch (const std::exception& e)
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

    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);

    // Push Dicom Series
    if(!m_job->cancelRequested())
    {
        for(::fwMedData::DicomSeries::sptr series : m_dicomSeriesContainer)
        {
            seriesDBHelper.add(series);
        }
    }

    try
    {
        // .get() throws exception that might have occurred
        m_job->run().get();
    }
    catch (const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
    }
    catch( ... )
    {
        m_logger->critical("An unkown error has occurred during the reading process.");
    }

    if(m_dicomSeriesContainer.empty())
    {
        m_logger->critical("Unable to retrieve series from the selected folder.");
    }

}

//------------------------------------------------------------------------------

void SeriesDB::readDicom()
{
    SLM_ASSERT("This reader only work on folder selection.",
               (::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this)));

    // DICOMDIR
    auto dicomdir = ::fwGdcmIO::helper::DicomDir::findDicomDir(this->getFolder());
    if(m_isDicomdirActivated && ::boost::filesystem::exists(dicomdir))
    {
        // Create Dicom Series
        ::fwGdcmIO::helper::DicomDir::retrieveDicomSeries(dicomdir,
                                                          m_dicomSeriesContainer,
                                                          m_logger,
                                                          m_readerJob->progressCallback(),
                                                          m_readerJob->cancelRequestedCallback()
                                                          );
        // Fill Dicom Series
        ::fwGdcmIO::helper::DicomSeries helper;
        helper.complete(m_dicomSeriesContainer, m_completeDicomSeriesJob);

    }

    // Finish DICOMDIR lookup
    m_dicomdirFileLookupJob->done();
    m_dicomdirFileLookupJob->finish();

    // Regular read
    if(!m_isDicomdirActivated || !::boost::filesystem::exists(dicomdir) || m_dicomSeriesContainer.empty())
    {
        m_readerJob->doneWork(0);

        // Recursively search for dicom files
        std::vector< ::boost::filesystem::path > filenames;
        ::fwGdcmIO::helper::DicomSearch::searchRecursively(
            this->getFolder(), filenames, true, m_regularFileLookupJob);

        // Read Dicom Series
        ::fwGdcmIO::helper::DicomSeries helper;
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

void SeriesDB::readFromDicomSeriesDB(const ::fwMedData::SeriesDB::csptr& dicomSeriesDB,
                                     const ::fwServices::IService::sptr& notifier)
{
    // Clear DicomSeries container
    m_dicomSeriesContainer.clear();

    m_job->add(m_converterJob);

    // Read series
    for(::fwMedData::Series::sptr series : dicomSeriesDB->getContainer())
    {
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
        SLM_ASSERT("Trying to read a series which is not a DicomSeries.", dicomSeries);
        m_dicomSeriesContainer.push_back(dicomSeries);
    }

    // Apply Default filters
    if(!m_dicomFilterType.empty())
    {
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(m_dicomFilterType);
        ::fwDicomIOFilter::helper::Filter::applyFilter(m_dicomSeriesContainer, filter, true, m_logger);
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
    catch (const std::exception& e)
    {
        m_logger->critical("An error has occurred during the reading process : " + std::string(e.what()));
    }
    catch( ... )
    {
        m_logger->critical("An unkown error has occurred during the reading process.");
    }

}

//------------------------------------------------------------------------------

bool SeriesDB::isDicomDirAvailable()
{
    auto dicomdir = ::fwGdcmIO::helper::DicomDir::findDicomDir(this->getFolder());
    return ::boost::filesystem::exists(dicomdir);
}

//------------------------------------------------------------------------------

void SeriesDB::convertDicomSeries(const ::fwServices::IService::sptr& notifier)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    // Sort DicomSeries
    std::sort(m_dicomSeriesContainer.begin(), m_dicomSeriesContainer.end(), SeriesDB::dicomSeriesComparator);

    // Create reader
    SPTR(::fwGdcmIO::reader::Series) seriesReader = std::make_shared< ::fwGdcmIO::reader::Series >();
    seriesReader->setBufferRotationEnabled(m_enableBufferRotation);
    seriesReader->setLogger(m_logger);

    m_converterJob->setTotalWorkUnits(m_dicomSeriesContainer.size());

    // Compute total work units
    // We do not use an Aggregator here as the jobs
    // are created after updating the main aggregator.
    std::uint64_t totalWorkUnits = 0;
    for(const ::fwMedData::DicomSeries::sptr& dicomSeries : m_dicomSeriesContainer)
    {
        totalWorkUnits += dicomSeries->getLocalDicomPaths().size();
    }
    m_converterJob->setTotalWorkUnits(totalWorkUnits);

    std::uint64_t completedProgress = 0;
    auto progressCallback = [&](std::uint64_t progress)
                            {
                                m_converterJob->doneWork(completedProgress + progress);
                            };

    // Read series
    for(const ::fwMedData::DicomSeries::csptr& dicomSeries : m_dicomSeriesContainer)
    {
        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = dicomSeries->getSOPClassUIDs();
        FW_RAISE_IF("The series contains several SOPClassUIDs. Try to apply a filter in order to split the series.",
                    sopClassUIDContainer.size() != 1);
        std::string sopClassUID = sopClassUIDContainer.begin()->c_str();

        const SupportedSOPClassContainerType::iterator bIt = m_supportedSOPClassContainer.begin();
        const SupportedSOPClassContainerType::iterator eIt = m_supportedSOPClassContainer.end();

        if(m_supportedSOPClassContainer.empty() || std::find(bIt, eIt, sopClassUID) != eIt)
        {
            seriesReader->setProgressCallback(progressCallback);
            seriesReader->setCancelRequestedCallback(m_converterJob->cancelRequestedCallback());
            try
            {

                ::fwMedData::Series::sptr series = seriesReader->read(dicomSeries);

                if(series)
                {
                    // Add the series to the DB
                    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);
                    seriesDBHelper.add(series);

                    if(notifier)
                    {
                        seriesDBHelper.notify();
                    }
                }
            }
            catch (::fwGdcmIO::exception::Failed& e)
            {
                m_logger->critical("Unable to read series : " + dicomSeries->getInstanceUID());
            }
        }
        else
        {
            const std::string sopClassName = ::fwGdcmIO::helper::SOPClass::getSOPClassName(sopClassUID);
            m_logger->critical("DICOM SOP Class \"" + sopClassName +"\" is not supported by the selected reader.");
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

bool SeriesDB::dicomSeriesComparator(const SPTR(::fwMedData::DicomSeries)& a,
                                     const SPTR(::fwMedData::DicomSeries)& b)
{
    const ::fwMedData::DicomSeries::SOPClassUIDContainerType aSOPClassUIDContainer = a->getSOPClassUIDs();
    const std::string aSOPClassUID                                                 = *(aSOPClassUIDContainer.begin());
    const ::fwMedData::DicomSeries::SOPClassUIDContainerType bSOPClassUIDContainer = b->getSOPClassUIDs();
    const std::string bSOPClassUID                                                 = *(bSOPClassUIDContainer.begin());

    // a > b if a contains a SR and not b
    const bool aIsAnImage =
        (::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
         ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
         aSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" || // FIXME Replace hard coded string by
                                                            // "::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) ==
                                                            // ::gdcm::MediaStorage::Comprehensive3DSR"
         ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage ||
         ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage);

    const bool bIsAnImage =
        (::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::EnhancedSR ||
         ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::ComprehensiveSR ||
         bSOPClassUID == "1.2.840.10008.5.1.4.1.1.88.34" || // FIXME Replace hard coded string by
                                                            // "::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) ==
                                                            // ::gdcm::MediaStorage::Comprehensive3DSR"
         ::gdcm::MediaStorage::GetMSType(bSOPClassUID.c_str()) == ::gdcm::MediaStorage::SpacialFiducialsStorage ||
         ::gdcm::MediaStorage::GetMSType(aSOPClassUID.c_str()) == ::gdcm::MediaStorage::SurfaceSegmentationStorage);

    return bIsAnImage && !aIsAnImage;
}

//------------------------------------------------------------------------------

SeriesDB::DicomSeriesContainerType& SeriesDB::getDicomSeries()
{
    return m_dicomSeriesContainer;
}

//------------------------------------------------------------------------------

SPTR(::fwJobs::IJob) SeriesDB::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

}  // namespace reader

}  // namespace fwGdcmIO
