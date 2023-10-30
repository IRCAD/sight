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

dicom_series_anonymizer::dicom_series_anonymizer() :
    m_job(std::make_shared<core::jobs::aggregator>("Anonymization process"))
{
    m_writer = std::make_shared<io::dicom::helper::dicom_series_writer>();
    m_reader = std::make_shared<io::dicom::reader::series_set>();
}

//------------------------------------------------------------------------------

dicom_series_anonymizer::~dicom_series_anonymizer()
= default;

//------------------------------------------------------------------------------

void dicom_series_anonymizer::anonymize(const data::dicom_series::sptr& _source)
{
    this->anonymize(_source, _source);
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::anonymize(
    const data::dicom_series::sptr& _source,
    const data::dicom_series::sptr& _destination
)
{
    auto writer_observer     = m_writer->get_job();
    auto anonymizer_observer = m_anonymizer.get_job();

    // Set up observer cancel callback
    m_job->add_simple_cancel_hook(
        [&]
        {
            writer_observer->cancel();
            anonymizer_observer->cancel();
        });

    m_job->add(writer_observer);
    m_job->add(anonymizer_observer, 10);

    const auto future = m_job->run();

    // Create destination directory
    core::os::temp_dir dest;

    // Write DicomSeries (Copy files)
    m_writer->set_object(_source);
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
    auto series_set = std::make_shared<data::series_set>();
    m_reader->set_object(series_set);
    m_reader->set_folder(dest);
    m_reader->read_dicom_series();

    // Update DicomSeries
    auto anonymized_series = std::dynamic_pointer_cast<data::dicom_series>(series_set->front());
    _destination->deep_copy(anonymized_series);
}

//------------------------------------------------------------------------------

core::jobs::aggregator::sptr dicom_series_anonymizer::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
