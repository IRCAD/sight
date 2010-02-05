/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

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

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <fwWX/wxZipFolder.hpp>

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
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose a fxz or a xml file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("fwXML archive (*.fxz)|*.fxz|fwXML (*.xml)|*.xml"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_SAVE,
#else
            wxSAVE,
#endif
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsPatientDBPath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        m_bServiceIsConfigured = true;
        _sDefaultPath = wxConvertMB2WX( m_fsPatientDBPath.branch_path().string().c_str() );
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
        ::fwWX::ProgressTowx progressMeterGUI("Saving Image ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        wxString wxStmp( ss.str().c_str(), wxConvLocal );
        wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        wxString wxStmp( ss.str().c_str(), wxConvLocal );
        wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
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

        wxBeginBusyCursor();
        m_fsPatientDBPath = correctFileFormat( m_fsPatientDBPath );
        if ( isAnFwxmlArchive( m_fsPatientDBPath ) )
        {
            manageZipAndSavePatientDB(m_fsPatientDBPath, patientDB);
        }
        else
        {
            savePatientDB(m_fsPatientDBPath, patientDB);
        }
        wxEndBusyCursor();
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
    wxString destZipFileName ( wxConvertMB2WX( inrFileDir.string().c_str() ) );
    wxString srcFolderName ( wxConvertMB2WX( srcFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::packFolder( srcFolderName, destZipFileName );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
