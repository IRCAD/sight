/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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

::boost::filesystem::path longestCommonPrefix( const ::fwMedData::DicomSeries::DicomPathContainerType& paths )
{
    ::boost::filesystem::path longestPrefix;
    if( !paths.empty() )
    {
        longestPrefix = paths.begin()->second;
    }

    for(const ::fwMedData::DicomSeries::DicomPathContainerType::value_type& value : paths)
    {
        std::pair< ::boost::filesystem::path::const_iterator, ::boost::filesystem::path::const_iterator > p
            = std::mismatch(longestPrefix.begin(), longestPrefix.end(), value.second.begin());

        if (p.first != longestPrefix.end())
        {
            ::boost::filesystem::path newPrefix;
            BOOST_FOREACH( const ::boost::filesystem::path &subpath, std::make_pair(longestPrefix.begin(), p.first))
            {
                newPrefix /= subpath;
            }
            longestPrefix = newPrefix;
        }
    }

    return longestPrefix;
}

//------------------------------------------------------------------------------

::boost::filesystem::path removePathPrefix(const ::boost::filesystem::path& path,
                                           const ::boost::filesystem::path& prefix)
{
    std::pair< ::boost::filesystem::path::const_iterator, ::boost::filesystem::path::const_iterator > p
        = std::mismatch(path.begin(), path.end(), prefix.begin());

    ::boost::filesystem::path newPrefix;
    BOOST_FOREACH( const ::boost::filesystem::path &subpath, std::make_pair(p.first,  path.end()))
    {
        newPrefix /= subpath;
    }
    return newPrefix;
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

std::string DicomSeriesWriter::getFilename(const std::string& defaultName)
{
    std::string filename;

    if(m_anonymizer)
    {
        std::stringstream ss;
        ss << "im" << std::setfill('0') << std::setw(5) << m_anonymizer->getNextIndex();
        filename = ss.str();
    }
    else
    {
        filename = defaultName;
    }

    return filename;
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
    ::fwMedData::DicomSeries::sptr dicomSeries = this->getConcreteObject();

    FW_RAISE_IF("Dicom series should contain binaries.",
                dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::NONE);

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
    if(dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::BINARIES)
    {
        for(::fwMedData::DicomSeries::DicomBinaryContainerType::value_type value : dicomSeries->getDicomBinaries())
        {
            if(m_job->cancelRequested())
            {
                return;
            }

            const std::string filename = this->getFilename(value.first);

            ::fwData::Array::sptr array = value.second;
            ::fwDataTools::helper::Array arrayHelper(array);
            char* buffer = static_cast<char*>(arrayHelper.getBuffer());
            size_t size  = array->getSizeInBytes();

            std::ifstream stream;
            stream.rdbuf()->pubsetbuf(buffer, size);

            const ::boost::filesystem::path& dest_dir =
                m_anonymizer ? folder/m_subPath : folder;

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
    }
    else
    {
        ::boost::filesystem::path longestPrefix = longestCommonPrefix(dicomSeries->getLocalDicomPaths()).parent_path();
        SLM_TRACE("Longest prefix :" + longestPrefix.string());

        for(const ::fwMedData::DicomSeries::DicomPathContainerType::value_type& value :
            dicomSeries->getLocalDicomPaths())
        {
            if(m_job->cancelRequested())
            {
                break;
            }

            const ::boost::filesystem::path& src = value.second;
            const std::string filename           = this->getFilename(src.filename().string());

            const ::boost::filesystem::path& dest_dir = folder /
                                                        (m_anonymizer ? m_subPath : removePathPrefix(src.parent_path(),
                                                                                                     longestPrefix));

            if(!::boost::filesystem::exists(dest_dir))
            {
                ::boost::filesystem::create_directories(dest_dir);
            }

            // Read file from filesystem
            ::boost::filesystem::ifstream stream(src, std::ios::binary);

            const ::boost::filesystem::path& dest_file = dest_dir / filename;

            ::boost::filesystem::ofstream fs(dest_file, std::ios::binary|std::ios::trunc);
            FW_RAISE_IF("Can't open '" <<  dest_file.string() << "' for write.", !fs.good());

            this->processStream(stream, fs);
            stream.close();

            m_job->doneWork(++count);
        }
    }

    m_job->finish();

    m_writeCount++;
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::processWriteArchive()
{
    SLM_ASSERT("Output archive shall be set", m_archive);

    ::fwMedData::DicomSeries::sptr dicomSeries = this->getConcreteObject();

    FW_RAISE_IF("Dicom series should contain binaries.",
                dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::NONE);

    const size_t nbInstances = dicomSeries->getNumberOfInstances();
    unsigned int count       = 0;

    m_job->setTotalWorkUnits(nbInstances);
    if(dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::BINARIES)
    {
        for(::fwMedData::DicomSeries::DicomBinaryContainerType::value_type value : dicomSeries->getDicomBinaries())
        {
            if(m_job->cancelRequested())
            {
                break;
            }

            const std::string filename = this->getFilename(value.first);

            ::fwData::Array::sptr array = value.second;
            ::fwDataTools::helper::Array arrayHelper(array);
            char* buffer = static_cast<char*>(arrayHelper.getBuffer());
            size_t size  = array->getSizeInBytes();

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
    }
    else
    {
        ::boost::filesystem::path longestPrefix = longestCommonPrefix(dicomSeries->getLocalDicomPaths()).parent_path();
        SLM_TRACE("Longest prefix :" + longestPrefix.string());

        for(const auto& value : dicomSeries->getLocalDicomPaths())
        {
            if(m_job->cancelRequested())
            {
                break;
            }

            const ::boost::filesystem::path& src = value.second;
            const std::string filename           = this->getFilename(src.filename().string());

            const ::boost::filesystem::path& dest_dir =
                m_anonymizer ? m_subPath : removePathPrefix(src.parent_path(), longestPrefix);

            // Read file from filesystem
            ::boost::filesystem::ifstream stream(src, std::ios::binary);
            FW_RAISE_IF("Can't open '" << src.string() << "' for read.", !stream.good());

            const ::boost::filesystem::path& dest_file = dest_dir / filename;
            SPTR(std::ostream) fs = m_archive->createFile(dest_file);
            FW_RAISE_IF("Can't open '" << dest_file.string() << "' for write.", !fs->good());

            this->processStream(stream, *fs);
            stream.close();

            m_job->doneWork(++count);
        }
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
