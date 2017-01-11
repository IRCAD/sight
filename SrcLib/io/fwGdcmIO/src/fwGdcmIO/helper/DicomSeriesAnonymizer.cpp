/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSeriesAnonymizer.hpp"

#include <fwCore/base.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Observer.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwTools/System.hpp>

#include <boost/foreach.hpp>

namespace fwGdcmIO
{
namespace helper
{

DicomSeriesAnonymizer::DicomSeriesAnonymizer() :
    m_job(::fwJobs::Aggregator::New("Anonymization process"))
{
}

//------------------------------------------------------------------------------

DicomSeriesAnonymizer::~DicomSeriesAnonymizer()
{
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(::fwMedData::DicomSeries::sptr source)
{
    this->anonymize(source, source);
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(::fwMedData::DicomSeries::sptr source,
                                      ::fwMedData::DicomSeries::sptr destination)
{
    FW_RAISE_IF("Dicom series should be available on the local computer.",
                source->getDicomAvailability() != ::fwMedData::DicomSeries::PATHS);

    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader          = ::fwGdcmIO::reader::SeriesDB::New();

    auto writerObserver     = writer->getJob();
    auto anonymizerObserver = m_anonymizer.getJob();
    auto readerObserver     = reader->getJob();

    // Set up observer cancel callback
    m_job->addSimpleCancelHook([&] {
                writerObserver->cancel();
                anonymizerObserver->cancel();
                readerObserver->cancel();
            });

    m_job->add(writerObserver);
    m_job->add(anonymizerObserver, 10);
    m_job->add(readerObserver);

    m_job->run();

    // Create destination directory
    const ::boost::filesystem::path destPath = ::fwTools::System::getTemporaryFolder("AnonymizedSeries");
    ::boost::filesystem::create_directories( destPath );

    // Write DicomSeries (Copy files)
    writer->setObject(source);
    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(destPath);
    writer->setLocation(loc);
    writer->write();

    if(m_job->cancelRequested())
    {
        return;
    }

    // Anonymize directory
    m_anonymizer.anonymize(destPath);

    if(m_job->cancelRequested())
    {
        return;
    }

    // Read anonymized series
    ::fwGdcmIO::reader::SeriesDB::sptr readerAnonymized = ::fwGdcmIO::reader::SeriesDB::New();
    ::fwMedData::SeriesDB::sptr seriesDB                = ::fwMedData::SeriesDB::New();
    readerAnonymized->setObject(seriesDB);
    readerAnonymized->setFolder(destPath);
    readerAnonymized->readDicomSeries();

    if(m_job->cancelRequested())
    {
        return;
    }

    // Update DicomSeries
    ::fwMedData::DicomSeries::sptr anonymizedSeries =
        ::fwMedData::DicomSeries::dynamicCast(seriesDB->getContainer().front());
    destination->deepCopy(anonymizedSeries);
}

//------------------------------------------------------------------------------

::fwJobs::Aggregator::sptr DicomSeriesAnonymizer::getJob() const
{
    return m_job;
}

} // namespace helper
} // namespace fwGdcmIO


