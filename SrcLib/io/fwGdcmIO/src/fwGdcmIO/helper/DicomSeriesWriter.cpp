/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSeriesWriter.hpp"

#include "fwGdcmIO/helper/DicomAnonymizer.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwZip/WriteZipArchive.hpp>

#include <boost/foreach.hpp>

fwDataIOWriterRegisterMacro( ::fwGdcmIO::helper::DicomSeriesWriter );

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_writeCount(0),
    m_job(::fwJobs::Observer::New("Writing DICOM files"))
{
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer)
{
    m_anonymizer = anonymizer;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::setOutputArchive(const ::fwZip::IWriteArchive::sptr& archive, const std::string& subPath)
{
    m_archive = archive;
    m_subPath = subPath;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::write()
{
    if(m_archive)
    {
        this->processWriteArchive();
    }
    else
    {
        this->processWrite();
    }
}

//------------------------------------------------------------------------------

std::string DicomSeriesWriter::getFilename(const size_t& instanceIndex)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(7);
    if(m_anonymizer)
    {
        ss << m_anonymizer->getNextIndex();
    }
    else
    {
        ss << instanceIndex;
    }

    return ss.str();
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processStream(std::istream& inputStream, std::ostream& outputStream)
{
    if(m_anonymizer)
    {
        m_anonymizer->anonymize(inputStream, outputStream);
    }
    else
    {
        outputStream << inputStream.rdbuf();
    }
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWrite()
{
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getConcreteObject();

    // Create folder
    ::boost::filesystem::path folder = this->getFolder();

    if(m_writeCount)
    {
        std::stringstream ss;
        ss << "fwGdcmIO" << std::setfill('0') << std::setw(3) << m_writeCount;
        folder /= ss.str();
    }

    if (!::boost::filesystem::exists(folder))
    {
        ::boost::filesystem::create_directories(folder);
    }

    std::size_t nbInstances = dicomSeries->getNumberOfInstances();

    m_job->setTotalWorkUnits(nbInstances);
    unsigned int count = 0;

    // Write binary files
    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancelRequested())
        {
            return;
        }

        const std::string filename = this->getFilename(value.first);

        const ::fwMemory::BufferObject::sptr sourceBuffer = value.second;
        ::fwMemory::BufferObject::Lock sourceLocker(sourceBuffer);

        char* buffer         = static_cast<char*>(sourceBuffer->getBuffer());
        std::streamsize size = std::streamsize(sourceBuffer->getSize());

        std::ifstream stream;
        stream.rdbuf()->pubsetbuf(buffer, size);

        const ::boost::filesystem::path& dest_dir = m_anonymizer ? folder/m_subPath : folder;

        if(!::boost::filesystem::exists(dest_dir))
        {
            ::boost::filesystem::create_directories(dest_dir);
        }

        const ::boost::filesystem::path& dest_file = dest_dir / filename;

        ::boost::filesystem::ofstream fs(dest_file, std::ios::binary|std::ios::trunc);
        FW_RAISE_IF("Can't open '" <<  dest_file.string() << "' for write.", !fs.good());

        this->processStream(stream, fs);
        stream.close();

        m_job->doneWork(++count);
    }

    m_job->finish();
    m_writeCount++;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWriteArchive()
{
    SLM_ASSERT("Output archive shall be set", m_archive);

    ::fwMedData::DicomSeries::csptr dicomSeries = this->getConcreteObject();

    const size_t nbInstances = dicomSeries->getNumberOfInstances();
    unsigned int count       = 0;

    m_job->setTotalWorkUnits(nbInstances);

    for(const auto& value : dicomSeries->getDicomContainer())
    {
        if(m_job->cancelRequested())
        {
            break;
        }

        const std::string filename = this->getFilename(value.first);

        const ::fwMemory::BufferObject::sptr sourceBuffer = value.second;
        ::fwMemory::BufferObject::Lock sourceLocker(sourceBuffer);

        char* buffer         = static_cast<char*>(sourceBuffer->getBuffer());
        std::streamsize size = std::streamsize(sourceBuffer->getSize());

        std::ifstream stream;
        stream.rdbuf()->pubsetbuf(buffer, size);

        const ::boost::filesystem::path& dest_dir =
            m_anonymizer ? m_subPath : "";

        const ::boost::filesystem::path& dest_file = dest_dir / filename;
        SPTR(std::ostream) fs = m_archive->createFile(dest_file);
        FW_RAISE_IF("Can't open '" << dest_file.string() << "' for write.", !fs->good());

        this->processStream(stream, *fs);
        stream.close();

        m_job->doneWork(++count);
    }

    m_job->done();
    m_job->finish();
    ++m_writeCount;
}

//------------------------------------------------------------------------------

std::string DicomSeriesWriter::extension()
{
    return "";
}

//------------------------------------------------------------------------------

SPTR(::fwJobs::IJob) DicomSeriesWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO
