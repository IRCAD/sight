/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDir.hpp"
#include "fwGdcmIO/helper/DicomData.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>
#include <fwMedData/DicomSeries.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>
#include <fwLog/Logger.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.h>

#include <gdcmReader.h>
#include <gdcmMediaStorage.h>

namespace fwGdcmIO
{
namespace helper
{

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

void DicomDir::retrieveDicomSeries(const ::boost::filesystem::path& dicomdir,
                                   std::vector< SPTR(::fwMedData::DicomSeries) >& seriesDB,
                                   const ::fwLog::Logger::sptr logger,
                                   const SPTR(::fwJobs::Observer)& fileLookupObserver)
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
        ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0002, 0x0002 >(fileMetaInformation);

    if (mediaStorageSOP != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SLM_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    typedef std::set<gdcm::DataElement> DataElementSet;
    typedef DataElementSet::const_iterator ConstIterator;

    unsigned int progress = 0.;
    unsigned int ptotal   = 0.;

    if(fileLookupObserver)
    {
        // Compute total progress
        for(ConstIterator it = dataset.GetDES().begin(); it != dataset.GetDES().end(); ++it)
        {
            // Directory Record Sequence
            if (it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
            {
                ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = it->GetValueAsSQ();

                ptotal += static_cast<unsigned int>(sequence->GetNumberOfItems());
            }
        }

        ptotal = (ptotal == 0) ? 1 : ptotal;
        fileLookupObserver->setTotalWorkUnits(ptotal);
    }

    ::boost::filesystem::path root = dicomdir.parent_path();

    std::map < std::string, ::fwMedData::DicomSeries::sptr > dicomSeriesMap;
    ::fwMedData::DicomSeries::sptr currentSeries = nullptr;

    for(ConstIterator it = dataset.GetDES().begin(); it != dataset.GetDES().end(); ++it)
    {
        // Directory Record Sequence
        if (it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = it->GetValueAsSQ();

            for(unsigned int index = 1; index <= sequence->GetNumberOfItems(); ++index)
            {
                // Retrieve item
                ::gdcm::Item& item = sequence->GetItem(index);

                // Directory Record Type
                const std::string& recordType =
                    ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0004, 0x1430 >(item.GetNestedDataSet());

                // If the record is a series, we select the current series
                if(recordType == "SERIES")
                {
                    const std::string& seriesUID =
                        DicomData::getTrimmedTagValue< 0x0020, 0x000e >(item.GetNestedDataSet());
                    if(dicomSeriesMap.find(seriesUID) == dicomSeriesMap.end())
                    {
                        ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::New();
                        series->setInstanceUID(seriesUID);
                        dicomSeriesMap[seriesUID] = series;
                    }

                    currentSeries = dicomSeriesMap[seriesUID];
                }

                // It the record is an image, we check the filename
                else if(recordType == "IMAGE")
                {
                    std::string file = DicomData::getTrimmedTagValue< 0x0004, 0x1500 >(item.GetNestedDataSet());
                    std::replace( file.begin(), file.end(), '\\', '/');
                    SLM_WARN_IF("Dicom instance doesn't have a referenced file id.", file.empty());

                    const ::boost::filesystem::path path = root / file;
                    OSLM_WARN_IF("Unable to find path :" << path, !::boost::filesystem::exists(path));
                    OSLM_WARN_IF("Dicomdir is badly formated. Skipping path :" << path, !currentSeries);

                    if(!currentSeries || file.empty())
                    {
                        logger->warning("DICOMDIR file is badly formated. Instances may be missing");
                    }
                    else if(::boost::filesystem::exists(path))
                    {
                        auto instanceNumber = ::boost::lexical_cast<unsigned int>(
                            DicomData::getTrimmedTagValue< 0x0020, 0x0013 >(
                                item.GetNestedDataSet()));
                        currentSeries->addDicomPath(instanceNumber, path);
                    }
                    else
                    {
                        logger->warning("Unable to retrieve file :" + path.string() );
                    }
                }

                if(fileLookupObserver)
                {
                    if(fileLookupObserver->cancelRequested())
                    {
                        return;
                    }
                    fileLookupObserver->doneWork(++progress);
                }

            }
        }
    }

    if(fileLookupObserver && fileLookupObserver->cancelRequested())
    {
        return;
    }

    for(auto entry: dicomSeriesMap)
    {
        auto series = entry.second;
        auto size   = series->getLocalDicomPaths().size();
        if(size)
        {
            series->setDicomAvailability(::fwMedData::DicomSeries::PATHS);
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
