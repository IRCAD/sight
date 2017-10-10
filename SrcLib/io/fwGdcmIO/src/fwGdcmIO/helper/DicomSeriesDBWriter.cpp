/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSeriesDBWriter.hpp"

#include "fwGdcmIO/helper/DicomAnonymizer.hpp"
#include "fwGdcmIO/helper/DicomSeriesWriter.hpp"

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

namespace fwGdcmIO
{

namespace helper
{

//------------------------------------------------------------------------------

DicomSeriesDBWriter::DicomSeriesDBWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
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
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
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

    for(::fwMedData::Series::sptr series: seriesDB->getContainer())
    {
        const ::fwMedData::DicomSeries::sptr& dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);

        ::fwJobs::Job::sptr job = ::fwJobs::Job::New("Write Dicom series",
            [&, dicomSeries](::fwJobs::Job& runningJob){
                if(!runningJob.cancelRequested())
                {
                    m_anonymizer->resetIndex();

                    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer =
                        ::fwGdcmIO::helper::DicomSeriesWriter::New();
                    writer->setObject(dicomSeries);
                    writer->setAnonymizer(m_anonymizer);
                    writer->setOutputArchive(writeArchive, nbSeries > 1 ? getSubPath(processedSeries++) : "");

                    runningJob.addCancelHook([&](::fwJobs::IJob& subJob)
                        {
                            writer->getJob()->cancel();
                        }
                                            );

                    writer->getJob()->addDoneWorkHook(
                        [&](::fwJobs::IJob& subJob, std::uint64_t oldWork)
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
            ::fwServices::registry::ActiveWorkers::getDefaultWorker());

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
