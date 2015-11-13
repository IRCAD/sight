/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <utility>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>


#include <stdio.h>

#include <fwComEd/helper/Array.hpp>

#include <fwCore/base.hpp>

#include <fwDicomData/DicomSeries.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "fwDicomIOExt/gdcm/DicomSeriesWriter.hpp"

fwDataIOWriterRegisterMacro( ::fwDicomIOExt::gdcm::DicomSeriesWriter );

namespace fwDicomIOExt
{
namespace gdcm
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_writeCount(0)
{
}

//------------------------------------------------------------------------------

::boost::filesystem::path longestCommonPrefix( const ::fwDicomData::DicomSeries::DicomPathContainerType &paths )
{
    ::boost::filesystem::path longestPrefix;
    if( !paths.empty() )
    {
        longestPrefix = paths.begin()->second;
    }

    for(const ::fwDicomData::DicomSeries::DicomPathContainerType::value_type &value : paths)
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

::boost::filesystem::path removePathPrefix(const ::boost::filesystem::path &path,
                                           const ::boost::filesystem::path &prefix)
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


void DicomSeriesWriter::write()
{
    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getConcreteObject();

    FW_RAISE_IF("Dicom series should contain binaries.",
                dicomSeries->getDicomAvailability() == ::fwDicomData::DicomSeries::NONE);

    // Create folder
    ::boost::filesystem::path folder = this->getFolder();

    if(m_writeCount)
    {
        std::stringstream ss;
        ss << "fwDicomIOExtWriter" << std::setfill('0') << std::setw(3) << m_writeCount;
        folder /= ss.str();
    }

    if (!::boost::filesystem::exists(folder))
    {
        ::boost::filesystem::create_directories(folder);
    }

    unsigned int nbInstances = dicomSeries->getNumberOfInstances();
    unsigned int count       = 0;
    // Write binary files

    if(dicomSeries->getDicomAvailability() == ::fwDicomData::DicomSeries::BINARIES)
    {
        for(::fwDicomData::DicomSeries::DicomBinaryContainerType::value_type value :
            dicomSeries->getDicomBinaries())
        {
            ::fwData::Array::sptr array = value.second;
            ::fwComEd::helper::Array arrayHelper(array);

            char* buffer = static_cast<char*>(arrayHelper.getBuffer());
            size_t size  = array->getSizeInBytes();

            ::boost::filesystem::ofstream fs(folder / value.first, std::ios::binary|std::ios::trunc);
            FW_RAISE_IF("Can't open '" << (folder / value.first) << "' for write.", !fs.good());

            fs.write(buffer, size);
            fs.close();

            this->notifyProgress(float(++count)/nbInstances, "Saving Dicom files.");

        }
    }
    else
    {
        ::boost::filesystem::path longestPrefix = longestCommonPrefix(dicomSeries->getLocalDicomPaths()).parent_path();
        SLM_TRACE("Longest prefix :" + longestPrefix.string());

        for(const ::fwDicomData::DicomSeries::DicomPathContainerType::value_type &value :
            dicomSeries->getLocalDicomPaths())
        {
            const ::boost::filesystem::path& src       = value.second;
            const ::boost::filesystem::path& dest_dir  = folder / removePathPrefix(src.parent_path(), longestPrefix);
            const ::boost::filesystem::path& dest_file = dest_dir / src.filename();
            ::boost::filesystem::create_directories(dest_dir);

            ::boost::system::error_code ec;
            ::boost::filesystem::copy( src, dest_file, ec );
            FW_RAISE_IF("Copy " << src.string() << " " << dest_file.string() << " error : "
                                << ec.message(), ec.value());

            ::boost::filesystem::permissions(dest_file, ::boost::filesystem::owner_all, ec);
            FW_RAISE_IF("Set permission " << dest_file.string() << " error : " << ec.message(), ec.value());

            this->notifyProgress(float(++count)/nbInstances, "Saving Dicom files.");
        }
    }

    ++m_writeCount;
}

//------------------------------------------------------------------------------

std::string DicomSeriesWriter::extension()
{
    return "";
}

} // namespace gdcm
} // namespace fwDicomIOExt
