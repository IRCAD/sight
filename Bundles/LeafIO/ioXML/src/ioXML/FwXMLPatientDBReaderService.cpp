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

FwXMLPatientDBReaderService::FwXMLPatientDBReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_fsPatientDBPath("")
{}

//------------------------------------------------------------------------------

FwXMLPatientDBReaderService::~FwXMLPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    // Test if in the service configuration the tag filename is defined. If it is defined, the image path is initialized and we tag the service as configured.
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsPatientDBPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsPatientDBPath);
        if(m_bServiceIsConfigured)
        {
            OSLM_TRACE("Filename found in service configuration : patient path = " << filename ) ;
        }
        else
        {
            OSLM_WARN("filename not exist = " <<  filename ) ;
        }
    }
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( this->getSelectorDialogTitle() );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.fxz");
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_fsPatientDBPath = result->getPath();
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::fixFilename(std::string _filename)
{
    m_fsPatientDBPath = ::boost::filesystem::path( _filename, ::boost::filesystem::native );
    m_bServiceIsConfigured = true;
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
    return "Choose a fxz or a xml file";
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr FwXMLPatientDBReaderService::createPatientDB( const ::boost::filesystem::path inrFileDir )
{
    SLM_TRACE_FUNC();
    ::fwXML::reader::FwXMLObjectReader myLoader;
    ::fwData::PatientDB::sptr pPatientDB;

    myLoader.setFile(inrFileDir);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ss << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
        return pPatientDB;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
        return pPatientDB;
    }

    pPatientDB = ::fwData::PatientDB::dynamicCast( myLoader.getObject() );

    return pPatientDB;
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    OSLM_TRACE("FwXMLPatientDBReaderService::updating()  m_fsPatientDBPath:"<<  m_fsPatientDBPath);

    if( m_bServiceIsConfigured )
    {

        ::fwData::PatientDB::sptr patientDB;
        if ( isAnFwxmlArchive( m_fsPatientDBPath ) )
        {
            patientDB = manageZipAndCreatePatientDB( m_fsPatientDBPath );
        }
        else
        {
            patientDB = createPatientDB( m_fsPatientDBPath );
        }

        if ( patientDB != NULL )
        {
            if( patientDB->getPatientSize() > 0 )
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
                ::fwGui::dialog::MessageDialog::showMessageDialog("Image Reader",
                        "File format unknown. Retry with another file reader.",
                        ::fwGui::dialog::IMessageDialog::WARNING);
            }
        }
        else
        {
            std::stringstream xmlFile;
            xmlFile << "Sorry, the xml file \""
            << m_fsPatientDBPath.string()
            << "\" does not content a PatientDB. This xml file has not been loaded.";
            ::fwGui::dialog::MessageDialog::showMessageDialog("FwXML PatientDB Reader", xmlFile.str(), ::fwGui::dialog::IMessageDialog::WARNING);
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
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr FwXMLPatientDBReaderService::manageZipAndCreatePatientDB( const ::boost::filesystem::path _pArchivePath )
{
    ::fwData::PatientDB::sptr patientDB;
    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";

    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );

    ::fwZip::ZipFolder::unpackFolder( _pArchivePath, destFolder );

    // Load
    ::boost::filesystem::path xmlfile = destFolder / "patient.xml";
    patientDB = createPatientDB( xmlfile );

    // Remove temp folder
    ::boost::filesystem::remove_all( destFolder );

    return patientDB;
}

//------------------------------------------------------------------------------

} // namespace ioXML
