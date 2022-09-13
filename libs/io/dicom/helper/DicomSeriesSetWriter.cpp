/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/jobs/Observer.hpp>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/zip/WriteDirArchive.hpp>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

DicomSeriesSetWriter::DicomSeriesSetWriter(io::base::writer::IObjectWriter::Key /*unused*/) :
    m_aggregator(core::jobs::Aggregator::New("Writing Dicom series"))
{
}

//------------------------------------------------------------------------------

std::string DicomSeriesSetWriter::extension() const
{
    return "";
}

//------------------------------------------------------------------------------

core::jobs::Aggregator::sptr DicomSeriesSetWriter::getAggregator()
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

    io::zip::IWriteArchive::sptr writeArchive = io::zip::WriteDirArchive::New(this->getFolder());

    const auto nbSeries = series_set->size();
    int processedSeries = 0;

    for(const auto& series : *series_set)
    {
        const auto& dicomSeries = data::DicomSeries::dynamicCast(series);

        core::jobs::Job::sptr job =
            core::jobs::Job::New(
                "Write Dicom series",
                [&, dicomSeries](core::jobs::Job& runningJob)
            {
                if(!runningJob.cancelRequested())
                {
                    m_anonymizer->resetIndex();

                    io::dicom::helper::DicomSeriesWriter::sptr writer = io::dicom::helper::DicomSeriesWriter::New();
                    writer->setObject(dicomSeries);
                    writer->setAnonymizer(m_anonymizer);
                    writer->setOutputArchive(writeArchive, nbSeries > 1 ? getSubPath(processedSeries++) : "");

                    runningJob.addCancelHook(
                        [&](core::jobs::IJob&)
                    {
                        writer->getJob()->cancel();
                    });

                    writer->getJob()->addDoneWorkHook(
                        [&](core::jobs::IJob& subJob, std::uint64_t)
                    {
                        runningJob.doneWork(subJob.getDoneWorkUnits());
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
                core::thread::getDefaultWorker()
            );

        m_aggregator->addCancelHook(
            [&](core::jobs::IJob&)
            {
                job->cancel();
            });
        m_aggregator->add(job);
    }

    auto futureAG = m_aggregator->run();

    futureAG.wait();
}

} // namespace sight::io::dicom::helper
