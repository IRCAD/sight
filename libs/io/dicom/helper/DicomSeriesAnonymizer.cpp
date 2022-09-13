/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <boost/foreach.hpp>

namespace sight::io::dicom::helper
{

DicomSeriesAnonymizer::DicomSeriesAnonymizer() :
    m_job(core::jobs::Aggregator::New("Anonymization process"))
{
    m_writer = io::dicom::helper::DicomSeriesWriter::New();
    m_reader = io::dicom::reader::SeriesSet::New();
}

//------------------------------------------------------------------------------

DicomSeriesAnonymizer::~DicomSeriesAnonymizer()
= default;

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

    // Set up observer cancel callback
    m_job->addSimpleCancelHook(
        [&]
        {
            writerObserver->cancel();
            anonymizerObserver->cancel();
        });

    m_job->add(writerObserver);
    m_job->add(anonymizerObserver, 10);

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

    // Wait for the aggregator to finish and catch exceptions
    future.get();

    // FIXME: At the beginning, this task was a third job
    // However, this reader also uses an aggregator - we discovered that an aggregator of aggregator exits
    // immediately, thus its state is FINISHED when we try to start the task...
    // Read anonymized series
    auto series_set = data::SeriesSet::New();
    m_reader->setObject(series_set);
    m_reader->setFolder(destPath);
    m_reader->readDicomSeries();

    // Update DicomSeries
    auto anonymizedSeries = data::DicomSeries::dynamicCast(series_set->front());
    destination->deepCopy(anonymizedSeries);
}

//------------------------------------------------------------------------------

core::jobs::Aggregator::sptr DicomSeriesAnonymizer::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
