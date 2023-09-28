/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>
#include <core/os/temp_path.hpp>

namespace sight::io::dicom::helper
{

DicomSeriesAnonymizer::DicomSeriesAnonymizer() :
    m_job(std::make_shared<core::jobs::aggregator>("Anonymization process"))
{
    m_writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
    m_reader = std::make_shared<io::dicom::reader::SeriesSet>();
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
    m_job->add_simple_cancel_hook(
        [&]
        {
            writerObserver->cancel();
            anonymizerObserver->cancel();
        });

    m_job->add(writerObserver);
    m_job->add(anonymizerObserver, 10);

    const auto future = m_job->run();

    // Create destination directory
    core::os::temp_dir dest;

    // Write DicomSeries (Copy files)
    m_writer->setObject(source);
    m_writer->set_folder(dest);
    m_writer->write();

    if(m_job->cancel_requested())
    {
        return;
    }

    // Anonymize directory
    m_anonymizer.anonymize(dest);

    if(m_job->cancel_requested())
    {
        return;
    }

    // Wait for the aggregator to finish and catch exceptions
    future.get();

    // FIXME: At the beginning, this task was a third job
    // However, this reader also uses an aggregator - we discovered that an aggregator of aggregator exits
    // immediately, thus its state is FINISHED when we try to start the task...
    // Read anonymized series
    auto series_set = std::make_shared<data::SeriesSet>();
    m_reader->setObject(series_set);
    m_reader->set_folder(dest);
    m_reader->readDicomSeries();

    // Update DicomSeries
    auto anonymizedSeries = std::dynamic_pointer_cast<data::DicomSeries>(series_set->front());
    destination->deep_copy(anonymizedSeries);
}

//------------------------------------------------------------------------------

core::jobs::aggregator::sptr DicomSeriesAnonymizer::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
