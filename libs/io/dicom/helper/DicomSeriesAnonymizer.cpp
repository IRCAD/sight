/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/helper/DicomSeriesAnonymizer.hpp"

#include <core/base.hpp>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/jobs/Observer.hpp>
#include <core/tools/System.hpp>

#include <data/SeriesDB.hpp>

#include <boost/foreach.hpp>

namespace sight::io::dicom
{

namespace helper
{

DicomSeriesAnonymizer::DicomSeriesAnonymizer() :
    m_job(core::jobs::Aggregator::New("Anonymization process"))
{
    m_writer = io::dicom::helper::DicomSeriesWriter::New();
    m_reader = io::dicom::reader::SeriesDB::New();
}

//------------------------------------------------------------------------------

DicomSeriesAnonymizer::~DicomSeriesAnonymizer()
{
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(const data::DicomSeries::sptr& source)
{
    this->anonymize(source, source);
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(
    const data::DicomSeries::sptr& source,
    const data::DicomSeries::sptr& destination
)
{
    auto writerObserver     = m_writer->getJob();
    auto anonymizerObserver = m_anonymizer.getJob();
    auto readerObserver     = m_reader->getJob();

    // Set up observer cancel callback
    m_job->addSimpleCancelHook(
        [&]
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
    const std::filesystem::path destPath = core::tools::System::getTemporaryFolder("AnonymizedSeries");
    std::filesystem::create_directories(destPath);

    // Write DicomSeries (Copy files)
    m_writer->setObject(source);
    m_writer->setFolder(destPath);
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
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();
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
    data::DicomSeries::sptr anonymizedSeries =
        data::DicomSeries::dynamicCast(seriesDB->getContainer().front());
    destination->deepCopy(anonymizedSeries);
}

//------------------------------------------------------------------------------

core::jobs::Aggregator::sptr DicomSeriesAnonymizer::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::io::dicom
