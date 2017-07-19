/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioData/SAttachmentSeriesReader.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwMedData/AttachmentSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <io/IReader.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>

fwServicesRegisterMacro( ::io::IReader, ::ioData::SAttachmentSeriesReader, ::fwMedData::SeriesDB );

namespace ioData
{

//------------------------------------------------------------------------------

std::string getMediaType(const ::boost::filesystem::path& media)
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

::io::IOPathType SAttachmentSeriesReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SAttachmentSeriesReader::configuring()
{
    ::io::IReader::configuring();
}

//------------------------------------------------------------------------------

void SAttachmentSeriesReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath("");

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
        ::fwMedData::SeriesDB::sptr seriesDB;
        if (this->isVersion2())
        {
            seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::io::s_DATA_KEY);
        }
        else
        {
            seriesDB = this->getObject< ::fwMedData::SeriesDB >();
        }

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
}

//------------------------------------------------------------------------------

}
