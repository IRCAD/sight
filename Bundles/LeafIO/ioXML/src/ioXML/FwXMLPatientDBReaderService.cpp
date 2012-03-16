/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientDBMsg.hpp>

#include <io/IReader.hpp>

#include <fwTools/System.hpp>

#include <fwXML/reader/FwXMLObjectReader.hpp>
#include <fwXML/writer/fwxmlextension.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLPatientDBReaderService.hpp"

//------------------------------------------------------------------------------

namespace ioXML
{

REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLPatientDBReaderService , ::fwData::PatientDB );

//------------------------------------------------------------------------------

FwXMLPatientDBReaderService::FwXMLPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

FwXMLPatientDBReaderService::~FwXMLPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( this->getSelectorDialogTitle() );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*." FWXML_ARCHIVE_EXTENSION);
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
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

::io::IOPathType FwXMLPatientDBReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLPatientDBReaderService::info" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > FwXMLPatientDBReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".xml");
    return extensions ;
}

//------------------------------------------------------------------------------

std::string FwXMLPatientDBReaderService::getSelectorDialogTitle()
{
    return "Choose a " FWXML_ARCHIVE_EXTENSION " or a xml file";
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr FwXMLPatientDBReaderService::createPatientDB( const ::boost::filesystem::path xmlFile )
{
    SLM_TRACE_FUNC();
    ::fwXML::reader::FwXMLObjectReader::NewSptr myLoader;
    ::fwData::PatientDB::sptr pPatientDB;

    myLoader->setFile(xmlFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading PatientDB");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
        pPatientDB = ::fwData::PatientDB::dynamicCast( myLoader->getObject() );
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ss << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return pPatientDB;
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    if( this->hasLocationDefined() )
    {
        ::fwData::PatientDB::sptr patientDB;
        if ( this->isAnFwxmlArchive( this->getFile() ) )
        {
            patientDB = this->manageZipAndCreatePatientDB( this->getFile() );
        }
        else
        {
            patientDB = this->createPatientDB( this->getFile() );
        }

        if (patientDB)
        {
            if( patientDB->getNumberOfPatients() )
            {
                // Retrieve dataStruct associated with this service
                ::fwData::PatientDB::sptr associatedPatientDB = this->getObject< ::fwData::PatientDB >();
                SLM_ASSERT("associatedPatientDB not instanced", associatedPatientDB);

                associatedPatientDB->shallowCopy( patientDB );

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);

                notificationOfDBUpdate();
                cursor.setDefaultCursor();
            }
            else
            {
                ::fwGui::dialog::MessageDialog::showMessageDialog("PatiendDB Reader",
                        "Sorry, no Patient found in the file.",
                        ::fwGui::dialog::IMessageDialog::WARNING);
            }
        }
    }
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDPDB not instanced", pDPDB);

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT, pDPDB );
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_LOADED_PATIENT );

    ::fwServices::IEditionService::notify( this->getSptr(),  pDPDB, msg );
}

//------------------------------------------------------------------------------

bool FwXMLPatientDBReaderService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == "." FWXML_ARCHIVE_EXTENSION );
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr FwXMLPatientDBReaderService::manageZipAndCreatePatientDB( const ::boost::filesystem::path _pArchivePath )
{
    ::fwData::PatientDB::sptr patientDB;
    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";

    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );

    ::fwZip::ZipFolder::NewSptr zip;
    ::fwGui::dialog::ProgressDialog progress("Reading patient");
    zip->addHandler( progress );
    zip->unpackFolder( _pArchivePath, destFolder );

    // Load
    ::boost::filesystem::path xmlFile;
    if(::boost::filesystem::exists(destFolder/"patient.xml"))
    {
        xmlFile = destFolder / "patient.xml";
        patientDB = this->createPatientDB( xmlFile );
    }
    else if(::boost::filesystem::exists(destFolder/"root.xml"))
    {
        xmlFile = destFolder / "root.xml";
        patientDB = this->createPatientDB( xmlFile );
    }
    else
    {
        std::stringstream stream;
        stream << "Sorry, "<<_pArchivePath<< " is not a valid " FWXML_ARCHIVE_EXTENSION " file."
               << this->getObject< ::fwTools::Object >()->getRootedClassname();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                        stream.str(),
                        ::fwGui::dialog::IMessageDialog::WARNING);
    }
    // Remove temp folder
    ::boost::filesystem::remove_all( destFolder );
    return patientDB;
}

//------------------------------------------------------------------------------

} // namespace ioXML
