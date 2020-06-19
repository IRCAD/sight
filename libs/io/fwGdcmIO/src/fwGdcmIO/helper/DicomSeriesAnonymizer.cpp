/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGdcmIO/helper/DicomSeriesAnonymizer.hpp"

#include <fwCore/base.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
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
    m_writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    m_reader = ::fwGdcmIO::reader::SeriesDB::New();
}

//------------------------------------------------------------------------------

DicomSeriesAnonymizer::~DicomSeriesAnonymizer()
{
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(const ::fwMedData::DicomSeries::sptr& source)
{
    this->anonymize(source, source);
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(const ::fwMedData::DicomSeries::sptr& source,
                                      const ::fwMedData::DicomSeries::sptr& destination)
{
    auto writerObserver     = m_writer->getJob();
    auto anonymizerObserver = m_anonymizer.getJob();
    auto readerObserver     = m_reader->getJob();

    // Set up observer cancel callback
    m_job->addSimpleCancelHook([&]
            {
                writerObserver->cancel();
                anonymizerObserver->cancel();
                readerObserver->cancel();
            });

    m_job->add(writerObserver);
    m_job->add(anonymizerObserver, 10);
    m_job->add(readerObserver);

    const auto future = m_job->run();

    // Create destination directory
    const std::filesystem::path destPath = ::fwTools::System::getTemporaryFolder("AnonymizedSeries");
    std::filesystem::create_directories( destPath );

    // Write DicomSeries (Copy files)
    m_writer->setObject(source);
    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(destPath);
    m_writer->setLocation(loc);
    m_writer->write();

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
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();
    m_reader->setObject(seriesDB);
    m_reader->setFolder(destPath);
    m_reader->readDicomSeries();

    if(m_job->cancelRequested())
    {
        return;
    }

    // Wait for the aggregator to finish and catch exceptions
    future.get();

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

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO
