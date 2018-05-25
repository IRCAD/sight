/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDir.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.h>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

namespace fwGdcmIO
{
namespace helper
{
// ----------------------------------------------------------------------------

::boost::filesystem::path DicomDir::findDicomDir(const ::boost::filesystem::path& root)
{
    ::boost::filesystem::path current = root;

    while(::boost::filesystem::exists(current))
    {
        ::boost::filesystem::path dicomDirPath = current / "dicomdir";
        if(::boost::filesystem::exists(dicomDirPath) && !::boost::filesystem::is_directory(dicomDirPath))
        {
            return dicomDirPath;
        }

        dicomDirPath = current / "DICOMDIR";
        if(::boost::filesystem::exists(dicomDirPath) && !::boost::filesystem::is_directory(dicomDirPath))
        {
            return dicomDirPath;
        }

        current = current.parent_path();
    }

    return ::boost::filesystem::path();

}

// ----------------------------------------------------------------------------

void processDirInformation(const ::boost::filesystem::path& dicomdir,
                           const ::boost::filesystem::path& rootDicomDirPath,
                           ::fwMedData::DicomSeries::sptr currentSeries,
                           std::map < std::string, ::fwMedData::DicomSeries::sptr >& dicomSeriesMap,
                           const ::fwLog::Logger::sptr& logger,
                           std::function< void(std::uint64_t) >& progress,
                           std::function< bool() >& cancel,
                           double& p,
                           double& ptotal)
{
    SLM_ASSERT("You must specify a valid dicomdir.", ::boost::filesystem::exists(dicomdir)
               && !::boost::filesystem::is_directory(dicomdir));

    // Try to read the file
    ::gdcm::Reader reader;
    reader.SetFileName(dicomdir.string().c_str());
    if( !reader.Read() )
    {
        return;
    }

    const ::gdcm::File& gdcmFile   = reader.GetFile();
    const ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

    // Check if the file is a DICOMDIR
    ::gdcm::MediaStorage mediaStorage;
    mediaStorage.SetFromFile(gdcmFile);
    if(mediaStorage != ::gdcm::MediaStorage::MediaStorageDirectoryStorage )
    {
        SLM_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const ::gdcm::FileMetaInformation& fileMetaInformation = gdcmFile.GetHeader();
    const std::string& mediaStorageSOP                     =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0002, 0x0002 >(fileMetaInformation);

    if (mediaStorageSOP != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SLM_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    typedef std::set<gdcm::DataElement> DataElementSet;
    typedef DataElementSet::const_iterator ConstIterator;

    for(ConstIterator it = dataset.GetDES().begin(); it != dataset.GetDES().end(); ++it)
    {
        // Directory Record Sequence
        if (it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = it->GetValueAsSQ();
            ptotal += static_cast<double>(sequence->GetNumberOfItems());

            for(unsigned int index = 1; index <= sequence->GetNumberOfItems(); ++index)
            {
                // Retrieve item
                ::gdcm::Item& item = sequence->GetItem(index);

                // Directory Record Type
                const std::string recordType =
                    ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0004, 0x1430 >(item.GetNestedDataSet());

                // Check Referenced File ID
                std::string refFileID =
                    ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0004, 0x1500 >(item.GetNestedDataSet());

                if(recordType == "IMAGE")
                {
                    // Read file path
                    std::string file = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0004, 0x1500 >(
                        item.GetNestedDataSet());
                    std::replace( file.begin(), file.end(), '\\', '/');
                    SLM_WARN_IF("Dicom instance doesn't have a referenced file id.", file.empty());

                    const ::boost::filesystem::path path = rootDicomDirPath / file;
                    OSLM_WARN_IF("Unable to find path :" << path, !::boost::filesystem::exists(path));
                    OSLM_WARN_IF("Dicomdir is badly formatted. Skipping path :" << path, !currentSeries);

                    if(!currentSeries || file.empty())
                    {
                        logger->warning("DICOMDIR file is badly formatted. Instances may be missing");
                    }
                    else if(::boost::filesystem::exists(path))
                    {
                        auto instanceNumber = ::boost::lexical_cast<unsigned int>(
                            ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020,
                                                                              0x0013 >(item.GetNestedDataSet()));
                        currentSeries->addDicomPath(instanceNumber, path);
                    }
                    else
                    {
                        logger->warning("Unable to retrieve file :" + path.string());
                    }
                }
                else
                {
                    // If the record is a series, we select the current series
                    if(recordType == "SERIES")
                    {
                        const std::string& seriesUID =
                            ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020, 0x000e >(item.GetNestedDataSet());
                        if(dicomSeriesMap.find(seriesUID) == dicomSeriesMap.end())
                        {
                            ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::New();
                            series->setInstanceUID(seriesUID);
                            dicomSeriesMap[seriesUID] = series;
                        }

                        currentSeries = dicomSeriesMap[seriesUID];
                    }

                    std::replace(refFileID.begin(), refFileID.end(), '\\', '/');
                    auto refFilePath = dicomdir.parent_path() / refFileID;
                    if(refFileID != "" && ::boost::filesystem::exists(refFilePath))
                    {
                        processDirInformation(refFilePath, rootDicomDirPath, currentSeries,
                                              dicomSeriesMap, logger, progress, cancel, p, ptotal);
                    }
                }

                if(progress)
                {
                    progress(++p);
                }

                if( cancel && cancel() )
                {
                    break;
                }

            }
        }
    }
}

// ----------------------------------------------------------------------------

void DicomDir::retrieveDicomSeries(const ::boost::filesystem::path& dicomdir,
                                   std::vector< SPTR(::fwMedData::DicomSeries) >& seriesDB,
                                   const ::fwLog::Logger::sptr& logger,
                                   std::function< void(std::uint64_t) > progress,
                                   std::function< bool() > cancel)
{
    SLM_ASSERT("You must specify a valid dicomdir.", ::boost::filesystem::exists(dicomdir)
               && !::boost::filesystem::is_directory(dicomdir));

    // Try to read the file
    ::gdcm::Reader reader;
    reader.SetFileName(dicomdir.string().c_str());
    if( !reader.Read() )
    {
        return;
    }

    const ::gdcm::File& gdcmFile   = reader.GetFile();
    const ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

    // Check if the file is a DICOMDIR
    ::gdcm::MediaStorage mediaStorage;
    mediaStorage.SetFromFile(gdcmFile);
    if(mediaStorage != ::gdcm::MediaStorage::MediaStorageDirectoryStorage )
    {
        SLM_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const ::gdcm::FileMetaInformation& fileMetaInformation = gdcmFile.GetHeader();
    const std::string& mediaStorageSOP                     =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0002, 0x0002 >(fileMetaInformation);

    if (mediaStorageSOP != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SLM_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    typedef std::set<gdcm::DataElement> DataElementSet;
    typedef DataElementSet::const_iterator ConstIterator;

    double p      = 0.;
    double ptotal = 0.;

    if(progress)
    {
        // Compute total progress
        for(ConstIterator it = dataset.GetDES().begin(); it != dataset.GetDES().end(); ++it)
        {
            // Directory Record Sequence
            if (it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
            {
                ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = it->GetValueAsSQ();

                ptotal += static_cast<double>(sequence->GetNumberOfItems());
            }
        }
    }

    if( 0. == ptotal )
    {
        ptotal = 1.;
    }

    std::map < std::string, ::fwMedData::DicomSeries::sptr > dicomSeriesMap;
    processDirInformation(dicomdir, dicomdir.parent_path(), nullptr, dicomSeriesMap,
                          logger, progress, cancel, p, ptotal);

    if( cancel && cancel() )
    {
        return;
    }

    for(auto entry : dicomSeriesMap)
    {
        auto series       = entry.second;
        const size_t size = series->getDicomContainer().size();
        if(size)
        {
            series->setNumberOfInstances(size);
            seriesDB.push_back(series);
        }
        else
        {
            logger->critical("Unable to retrieve instances for this series : " + series->getInstanceUID());
        }
    }
}

// ----------------------------------------------------------------------------

} //helper
} //fwGdcmIO
