/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/helper/DicomDir.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <core/exceptionmacros.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/log/Logger.hpp>
#include <core/spyLog.hpp>

#include <data/DicomSeries.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

#include <filesystem>

namespace sight::io::dicom
{

namespace helper
{

// ----------------------------------------------------------------------------

std::filesystem::path DicomDir::findDicomDir(const std::filesystem::path& root)
{
    std::filesystem::path current = root;

    while(std::filesystem::exists(current) && current != current.root_path())
    {
        std::filesystem::path dicomDirPath = current / "dicomdir";
        if(std::filesystem::exists(dicomDirPath) && !std::filesystem::is_directory(dicomDirPath))
        {
            return dicomDirPath;
        }

        dicomDirPath = current / "DICOMDIR";
        if(std::filesystem::exists(dicomDirPath) && !std::filesystem::is_directory(dicomDirPath))
        {
            return dicomDirPath;
        }

        current = current.parent_path();
    }

    return std::filesystem::path();
}

// ----------------------------------------------------------------------------

void processDirInformation(
    const std::filesystem::path& dicomdir,
    const std::filesystem::path& rootDicomDirPath,
    data::DicomSeries::sptr currentSeries,
    std::map<std::string, data::DicomSeries::sptr>& dicomSeriesMap,
    const core::log::Logger::sptr& logger,
    std::function<void(std::uint64_t)>& progress,
    std::function<bool()>& cancel,
    double& p,
    double& ptotal
)
{
    SIGHT_ASSERT(
        "You must specify a valid dicomdir.",
        std::filesystem::exists(dicomdir)
        && !std::filesystem::is_directory(dicomdir)
    );

    // Try to read the file
    ::gdcm::Reader reader;
    reader.SetFileName(dicomdir.string().c_str());
    if(!reader.Read())
    {
        return;
    }

    const ::gdcm::File& gdcmFile   = reader.GetFile();
    const ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

    // Check if the file is a DICOMDIR
    ::gdcm::MediaStorage mediaStorage;
    mediaStorage.SetFromFile(gdcmFile);
    if(mediaStorage != ::gdcm::MediaStorage::MediaStorageDirectoryStorage)
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const ::gdcm::FileMetaInformation& fileMetaInformation = gdcmFile.GetHeader();
    const std::string& mediaStorageSOP                     =
        io::dicom::helper::DicomDataReader::getTagValue<0x0002, 0x0002>(fileMetaInformation);

    if(mediaStorageSOP != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // For each root elements
    typedef std::set<gdcm::DataElement> DataElementSet;
    typedef DataElementSet::const_iterator ConstIterator;

    for(ConstIterator it = dataset.GetDES().begin() ; it != dataset.GetDES().end() ; ++it)
    {
        // Directory Record Sequence
        if(it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence = it->GetValueAsSQ();
            ptotal += static_cast<double>(sequence->GetNumberOfItems());

            for(unsigned int index = 1 ; index <= sequence->GetNumberOfItems() ; ++index)
            {
                // Retrieve item
                ::gdcm::Item& item = sequence->GetItem(index);

                // Directory Record Type
                const std::string recordType =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0004, 0x1430>(item.GetNestedDataSet());

                // Check Referenced File ID
                std::string refFileID =
                    io::dicom::helper::DicomDataReader::getTagValue<0x0004, 0x1500>(item.GetNestedDataSet());

                if(recordType == "IMAGE")
                {
                    // Read file path
                    std::string file = io::dicom::helper::DicomDataReader::getTagValue<0x0004, 0x1500>(
                        item.GetNestedDataSet()
                    );
                    std::replace(file.begin(), file.end(), '\\', '/');
                    SIGHT_WARN_IF("Dicom instance doesn't have a referenced file id.", file.empty());

                    const std::filesystem::path path = rootDicomDirPath / file;
                    SIGHT_WARN_IF("Unable to find path :" << path, !std::filesystem::exists(path));
                    SIGHT_WARN_IF("Dicomdir is badly formatted. Skipping path :" << path, !currentSeries);

                    if(!currentSeries || file.empty())
                    {
                        logger->warning("DICOMDIR file is badly formatted. Instances may be missing");
                    }
                    else if(std::filesystem::exists(path))
                    {
                        auto instanceNumber = ::boost::lexical_cast<unsigned int>(
                            io::dicom::helper::DicomDataReader::getTagValue<0x0020,
                                                                            0x0013>(item.GetNestedDataSet())
                        );
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
                            io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000e>(item.GetNestedDataSet());
                        if(dicomSeriesMap.find(seriesUID) == dicomSeriesMap.end())
                        {
                            data::DicomSeries::sptr series = data::DicomSeries::New();
                            series->setInstanceUID(seriesUID);
                            dicomSeriesMap[seriesUID] = series;
                        }

                        currentSeries = dicomSeriesMap[seriesUID];
                    }

                    std::replace(refFileID.begin(), refFileID.end(), '\\', '/');
                    auto refFilePath = dicomdir.parent_path() / refFileID;
                    if(refFileID != "" && std::filesystem::exists(refFilePath))
                    {
                        processDirInformation(
                            refFilePath,
                            rootDicomDirPath,
                            currentSeries,
                            dicomSeriesMap,
                            logger,
                            progress,
                            cancel,
                            p,
                            ptotal
                        );
                    }
                }

                if(progress)
                {
                    progress(++p);
                }

                if(cancel && cancel())
                {
                    break;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------

void DicomDir::retrieveDicomSeries(
    const std::filesystem::path& dicomdir,
    std::vector<SPTR(data::DicomSeries)>& seriesDB,
    const core::log::Logger::sptr& logger,
    std::function<void(std::uint64_t)> progress,
    std::function<bool()> cancel
)
{
    SIGHT_ASSERT(
        "You must specify a valid dicomdir.",
        std::filesystem::exists(dicomdir)
        && !std::filesystem::is_directory(dicomdir)
    );

    // Try to read the file
    ::gdcm::Reader reader;
    reader.SetFileName(dicomdir.string().c_str());
    if(!reader.Read())
    {
        return;
    }

    const ::gdcm::File& gdcmFile   = reader.GetFile();
    const ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

    // Check if the file is a DICOMDIR
    ::gdcm::MediaStorage mediaStorage;
    mediaStorage.SetFromFile(gdcmFile);
    if(mediaStorage != ::gdcm::MediaStorage::MediaStorageDirectoryStorage)
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
        return;
    }

    // Check the MediaStorageSOPClass
    const ::gdcm::FileMetaInformation& fileMetaInformation = gdcmFile.GetHeader();
    const std::string& mediaStorageSOP                     =
        io::dicom::helper::DicomDataReader::getTagValue<0x0002, 0x0002>(fileMetaInformation);

    if(mediaStorageSOP != ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MediaStorageDirectoryStorage))
    {
        SIGHT_ERROR("This file is not a DICOMDIR");
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
        for(ConstIterator it = dataset.GetDES().begin() ; it != dataset.GetDES().end() ; ++it)
        {
            // Directory Record Sequence
            if(it->GetTag() == ::gdcm::Tag(0x0004, 0x1220))
            {
                ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence = it->GetValueAsSQ();

                ptotal += static_cast<double>(sequence->GetNumberOfItems());
            }
        }
    }

    if(0. == ptotal)
    {
        ptotal = 1.;
    }

    std::map<std::string, data::DicomSeries::sptr> dicomSeriesMap;
    processDirInformation(
        dicomdir,
        dicomdir.parent_path(),
        nullptr,
        dicomSeriesMap,
        logger,
        progress,
        cancel,
        p,
        ptotal
    );

    if(cancel && cancel())
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
