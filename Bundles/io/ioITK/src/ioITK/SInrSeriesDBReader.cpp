/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioITK/SInrSeriesDBReader.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwItkIO/ImageReader.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioITK::SInrSeriesDBReader, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

SInrSeriesDBReader::SInrSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

SInrSeriesDBReader::~SInrSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SInrSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FILES;
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an Inrimage file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setType(::fwGui::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::MultiFiles::sptr result;
    result = ::fwData::location::MultiFiles::dynamicCast( dialogFile.show() );
    if (result)
    {
        const ::fwData::location::ILocation::VectPathType paths = result->getPaths();
        if(!paths.empty())
        {
            _sDefaultPath = paths[0].parent_path();
            dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        }
        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

bool SInrSeriesDBReader::createImage( const ::boost::filesystem::path inrFile, ::fwData::Image::sptr image )
{
    SLM_TRACE_FUNC();
    ::fwItkIO::ImageReader::sptr myLoader = ::fwItkIO::ImageReader::New();
    bool ok = true;

    myLoader->setObject(image);
    myLoader->setFile(inrFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          ss.str(),
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          "Warning during loading",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", seriesDB);

        ::fwMedData::SeriesDB::sptr localSeriesDB = ::fwMedData::SeriesDB::New();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        const std::string instanceUID = ::fwTools::UUID::generateUUID();

        for(const ::boost::filesystem::path& path :  this->getFiles())
        {
            ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();
            this->initSeries(imgSeries, instanceUID);

            ::fwData::Image::sptr image = ::fwData::Image::New();
            this->createImage( path, image );
            imgSeries->setImage(image);

            localSeriesDB->getContainer().push_back(imgSeries);
        }

        ::fwMedDataTools::helper::SeriesDB sDBhelper(seriesDB);

        ::fwData::mt::ObjectWriteLock lock(seriesDB);
        sDBhelper.merge(localSeriesDB);
        sDBhelper.notify();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::initSeries(::fwMedData::Series::sptr series, const std::string& instanceUID)
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

//------------------------------------------------------------------------------

} // namespace ioITK
