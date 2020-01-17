/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioData/SAttachmentSeriesReader.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwMedData/AttachmentSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

fwServicesRegisterMacro( ::fwIO::IReader, ::ioData::SAttachmentSeriesReader, ::fwMedData::SeriesDB );

namespace ioData
{

//------------------------------------------------------------------------------

std::string getMediaType(const std::filesystem::path& media)
{
    std::string mediaType;
    std::map<std::string, std::string> mimeMap =
    {
        {".txt", "text/plain"},
        {".rtf", "text/rtf"},
        {".pdf", "application/pdf"},
        {".zip", "application/zip"},
        {".mp3", "audio/mpeg"},
        {".wav", "audio/x-wav"},
        {".bmp", "image/bmp"},
        {".jpeg", "image/jpeg"},
        {".mpeg", "video/mpeg"},
        {".mp4", "video/mpeg"},
        {".avi", "video/x-msvideo"}
    };
    const std::string extension = media.extension().string();
    const auto iter             = mimeMap.find(extension);
    if(iter != mimeMap.end())
    {
        mediaType = iter->second;
    }
    return mediaType;
}

//------------------------------------------------------------------------------

void  initSeries(::fwMedData::Series::sptr series, const std::string& instanceUID)
{
    const std::string unknown = "unknown";
    series->setModality("OT");
    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    const std::string date = ::fwTools::getDate(now);
    const std::string time = ::fwTools::getTime(now);
    series->setDate(date);
    series->setTime(time);
    //series->setDescription(??);
    //series->setPerformingPhysiciansName(??);

    series->getEquipment()->setInstitutionName(unknown);

    series->getPatient()->setName(unknown);
    series->getPatient()->setPatientId(unknown);
    series->getPatient()->setBirthdate(unknown);
    series->getPatient()->setSex(unknown);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
    series->getStudy()->setReferringPhysicianName(unknown);
    series->getStudy()->setDescription(unknown);
    series->getStudy()->setPatientAge(unknown);
}

//-----------------------------------------------------------------------------

SAttachmentSeriesReader::SAttachmentSeriesReader()
{
    FW_DEPRECATED_MSG("'SAttachmentSeriesReader' is deprecated.", "20.0");
}

//-----------------------------------------------------------------------------

void SAttachmentSeriesReader::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << " AttachmentSerie reader";
}

//-----------------------------------------------------------------------------

std::vector< std::string > SAttachmentSeriesReader::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".*");
    return extensions;
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SAttachmentSeriesReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SAttachmentSeriesReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SAttachmentSeriesReader::configureWithIHM()
{
    static std::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a attachment file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Attachment file", "*.*");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SAttachmentSeriesReader::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve object
        ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);

        ::fwMedData::AttachmentSeries::sptr series = ::fwMedData::AttachmentSeries::New();
        const std::string instanceUID = ::fwTools::UUID::generateUUID();
        initSeries(series, instanceUID);
        series->setAttachmentPath(this->getFile());
        const std::string mediaType = getMediaType(this->getFile());
        series->setMediaType(mediaType);

        ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);

        ::fwData::mt::ObjectWriteLock lock(seriesDB);
        seriesDBHelper.add(series);
        seriesDBHelper.notify();
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

}
