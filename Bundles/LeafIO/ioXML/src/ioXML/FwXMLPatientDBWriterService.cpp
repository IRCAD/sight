/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTools/System.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLPatientDBWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLPatientDBWriterService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

FwXMLPatientDBWriterService::FwXMLPatientDBWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsPatientDBPath("")
{}

//------------------------------------------------------------------------------

FwXMLPatientDBWriterService::~FwXMLPatientDBWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( "Choose a fxz or a xml file" );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

void FwXMLPatientDBWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBWriterService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBWriterService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLPatientDBWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLPatientDBWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLPatientDBWriterService::getPersistanceId()
{
    return "ioITK::FwXMLPatientDBWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::savePatientDB( const ::boost::filesystem::path inrFileDir, ::fwData::PatientDB::sptr _pPatient )
{
    SLM_TRACE("FwXMLPatientDBWriterService::createPatientDB");
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pPatient);
    myWriter.setFile(inrFileDir);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";

        ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::PatientDB::sptr associatedPatientDB = this->getObject< ::fwData::PatientDB >();
        assert( associatedPatientDB ) ;

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        m_fsPatientDBPath = correctFileFormat( m_fsPatientDBPath );

        if ( isAnFwxmlArchive( m_fsPatientDBPath ) )
        {
            manageZipAndSavePatientDB(m_fsPatientDBPath,associatedPatientDB);
        }
        else
        {
            savePatientDB(m_fsPatientDBPath,associatedPatientDB);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

bool FwXMLPatientDBWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath ) const
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLPatientDBWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    OSLM_TRACE("Data saved in : " << _filePath.string());
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
        OSLM_TRACE("File renamed in : " << newPath.string());
    }

    return newPath;
}

//------------------------------------------------------------------------------

void FwXMLPatientDBWriterService::manageZipAndSavePatientDB( const ::boost::filesystem::path inrFileDir, ::fwData::PatientDB::sptr _pPatient )
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
