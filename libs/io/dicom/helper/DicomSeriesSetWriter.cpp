/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/helper/DicomSeriesSetWriter.hpp"

#include "io/dicom/helper/DicomAnonymizer.hpp"
#include "io/dicom/helper/DicomSeriesWriter.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/zip/WriteDirArchive.hpp>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

DicomSeriesSetWriter::DicomSeriesSetWriter() :
    m_aggregator(std::make_shared<core::jobs::aggregator>("Writing Dicom series"))
{
}

//------------------------------------------------------------------------------

std::string DicomSeriesSetWriter::extension() const
{
    return "";
}

//------------------------------------------------------------------------------

core::jobs::aggregator::sptr DicomSeriesSetWriter::getAggregator()
{
    return m_aggregator;
}

//------------------------------------------------------------------------------

void DicomSeriesSetWriter::setAnonymizer(const DicomAnonymizer::sptr& anonymizer)
{
    m_anonymizer = anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesSetWriter::setProducer(std::string producer)
{
    m_producer = producer;
}

//------------------------------------------------------------------------------

std::string getSubPath(int index)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << index;
    return ss.str();
}

//------------------------------------------------------------------------------

void DicomSeriesSetWriter::write()
{
    auto series_set = getConcreteObject();
    SIGHT_ASSERT("Unable to retrieve associated SeriesSet", series_set);

    io::zip::write_archive::sptr writeArchive = io::zip::WriteDirArchive::make(this->get_folder());

    const auto nbSeries = series_set->size();
    int processedSeries = 0;

    for(const auto& series : *series_set)
    {
        const auto& dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(series);

        core::jobs::job::sptr job =
            std::make_shared<core::jobs::job>(
                "Write Dicom series",
                [&, dicomSeries](core::jobs::job& runningJob)
            {
                if(!runningJob.cancel_requested())
                {
                    m_anonymizer->resetIndex();

                    io::dicom::helper::DicomSeriesWriter::sptr writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
                    writer->setObject(dicomSeries);
                    writer->setAnonymizer(m_anonymizer);
                    writer->setOutputArchive(writeArchive, nbSeries > 1 ? getSubPath(processedSeries++) : "");

                    runningJob.add_cancel_hook(
                        [&](core::jobs::base&)
                    {
                        writer->getJob()->cancel();
                    });

                    writer->getJob()->add_done_work_hook(
                        [&](core::jobs::base& subJob, std::uint64_t)
                    {
                        runningJob.done_work(subJob.get_done_work_units());
                    });

                    try
                    {
                        writer->write();
                    }
                    catch(std::exception& e)
                    {
                        runningJob.log(std::string("Failed to write series :\n") + e.what());
                    }
                    catch(...)
                    {
                        runningJob.log("An unexpected error occurred while writing series");
                    }
                }
            },
                core::thread::get_default_worker()
            );

        m_aggregator->add_cancel_hook(
            [&](core::jobs::base&)
            {
                job->cancel();
            });
        m_aggregator->add(job);
    }

    auto futureAG = m_aggregator->run();

    futureAG.wait();
}

} // namespace sight::io::dicom::helper
