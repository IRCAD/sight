/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwGdcmIO/helper/DicomSeriesDBWriter.hpp"

#include "fwGdcmIO/helper/DicomAnonymizer.hpp"
#include "fwGdcmIO/helper/DicomSeriesWriter.hpp"

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <services/registry/ActiveWorkers.hpp>

namespace fwGdcmIO
{

namespace helper
{

//------------------------------------------------------------------------------

DicomSeriesDBWriter::DicomSeriesDBWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    data::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_aggregator(::fwJobs::Aggregator::New("Writing Dicom series")),
    m_enableZippedArchive(false)

{
}

//------------------------------------------------------------------------------

DicomSeriesDBWriter::~DicomSeriesDBWriter()
{
}

//------------------------------------------------------------------------------

std::string DicomSeriesDBWriter::extension()
{
    return "";
}

//------------------------------------------------------------------------------

::fwJobs::Aggregator::sptr DicomSeriesDBWriter::getAggregator()
{
    return m_aggregator;
}

//------------------------------------------------------------------------------

void DicomSeriesDBWriter::setAnonymizer(const DicomAnonymizer::sptr& anonymizer)
{
    m_anonymizer = anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesDBWriter::setProducer(std::string producer)
{
    m_producer = producer;
}

//------------------------------------------------------------------------------

void DicomSeriesDBWriter::enableZippedArchive(bool enable)
{
    m_enableZippedArchive = enable;
}
//------------------------------------------------------------------------------

std::string getSubPath(int index)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << index;
    return ss.str();
}

//------------------------------------------------------------------------------

void DicomSeriesDBWriter::write()
{
    data::SeriesDB::csptr seriesDB = this->getConcreteObject();
    SLM_ASSERT("Unable to retrieve associated SeriesDB", seriesDB);

    ::fwZip::IWriteArchive::sptr writeArchive;

    if(m_enableZippedArchive)
    {
        ::fwZip::WriteZipArchive::sptr writeZipArchive = ::fwZip::WriteZipArchive::New(this->getFile(), m_producer);

        writeArchive = writeZipArchive;
    }
    else
    {
        writeArchive = ::fwZip::WriteDirArchive::New(this->getFolder());
    }

    const auto nbSeries = seriesDB->getContainer().size();
    int processedSeries = 0;

    for(data::Series::sptr series: seriesDB->getContainer())
    {
        const data::DicomSeries::sptr& dicomSeries = data::DicomSeries::dynamicCast(series);

        ::fwJobs::Job::sptr job = ::fwJobs::Job::New("Write Dicom series", [&, dicomSeries](::fwJobs::Job& runningJob)
                {
                    if(!runningJob.cancelRequested())
                    {
                        m_anonymizer->resetIndex();

                        ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
                        writer->setObject(dicomSeries);
                        writer->setAnonymizer(m_anonymizer);
                        writer->setOutputArchive(writeArchive, nbSeries > 1 ? getSubPath(processedSeries++) : "");

                        runningJob.addCancelHook([&](::fwJobs::IJob& subJob)
                        {
                            writer->getJob()->cancel();
                        }
                                                 );

                        writer->getJob()->addDoneWorkHook([&](::fwJobs::IJob& subJob, std::uint64_t oldWork)
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
                                                     services::registry::ActiveWorkers::getDefaultWorker());

        m_aggregator->addCancelHook([&](::fwJobs::IJob& subJob)
                {
                    job->cancel();
                }
                                    );
        m_aggregator->add(job);
    }

    auto futureAG = m_aggregator->run();

    futureAG.wait();
}

//------------------------------------------------------------------------------
} // namespace helper
} // namespace fwGdcmIO
