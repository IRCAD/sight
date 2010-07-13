/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTools/System.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwGui/ProgressDialog.hpp>
#include <fwGui/LocationDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLPatient2PatientDBWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLPatient2PatientDBWriterService , ::fwData::Patient ) ;

//------------------------------------------------------------------------------

FwXMLPatient2PatientDBWriterService::FwXMLPatient2PatientDBWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsPatientDBPath("")
{}

//------------------------------------------------------------------------------

FwXMLPatient2PatientDBWriterService::~FwXMLPatient2PatientDBWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::LocationDialog dialogFile;
    dialogFile.setTitle( "Choose a fxz or a xml file" );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.addFilter("fwXML archive","*.fxz");
    dialogFile.setOption(::fwGui::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath() ;
        m_fsPatientDBPath = result->getPath() ;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatient2PatientDBWriterService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatient2PatientDBWriterService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLPatient2PatientDBWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLPatient2PatientDBWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLPatient2PatientDBWriterService::getPersistanceId()
{
    return "ioITK::FwXMLPatient2PatientDBWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::savePatientDB( const ::boost::filesystem::path inrFileDir, ::fwData::PatientDB::sptr _pPatient )
{
    SLM_TRACE("FwXMLPatient2PatientDBWriterService::createPatientDB");
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pPatient);
    myWriter.setFile(inrFileDir);

    try
    {
        ::fwGui::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatient2PatientDBWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Patient::sptr associatedPatient = ::fwData::Patient::dynamicCast( this->getObject() ) ;
        assert( associatedPatient ) ;
        ::fwData::PatientDB::NewSptr patientDB;
        patientDB->addPatient(associatedPatient);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        m_fsPatientDBPath = correctFileFormat( m_fsPatientDBPath );
        if ( isAnFwxmlArchive( m_fsPatientDBPath ) )
        {
            manageZipAndSavePatientDB(m_fsPatientDBPath, patientDB);
        }
        else
        {
            savePatientDB(m_fsPatientDBPath, patientDB);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLPatient2PatientDBWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
    }

    return newPath;
}

//------------------------------------------------------------------------------

bool FwXMLPatient2PatientDBWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

void FwXMLPatient2PatientDBWriterService::manageZipAndSavePatientDB( const ::boost::filesystem::path inrFileDir, ::fwData::PatientDB::sptr _pPatient )
{
    // Save in tmp folder
    ::boost::filesystem::path srcFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = srcFolder / "patient.xml";
    ::boost::filesystem::create_directories( srcFolder );
    savePatientDB(xmlfile,_pPatient);

    // Zip
    ::fwZip::ZipFolder::packFolder( srcFolder, inrFileDir );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
