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

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <fwWX/wxZipFolder.hpp>

#include "ioXML/FwXMLPatientDBWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLPatientDBWriterService , ::fwData::PatientDB ) ;
//REGISTER_EXECUTABLE( ::ioXML::FwXMLPatientDBWriterService, "ioXML::FwXMLPatientDBWriterService" );

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

void FwXMLPatientDBWriterService::savePatientDB( const ::boost::filesystem::path inrFileDir, ::boost::shared_ptr< ::fwData::PatientDB > _pPatient )
{
    SLM_TRACE("FwXMLPatientDBWriterService::createPatientDB");
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

void FwXMLPatientDBWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::boost::shared_ptr< ::fwTools::Object > associatedObject = this->getObject();
        ::boost::shared_ptr< ::fwData::PatientDB > associatedPatientDB = ::boost::dynamic_pointer_cast< ::fwData::PatientDB >( associatedObject ) ;
        assert( associatedPatientDB ) ;

        wxBeginBusyCursor();
        m_fsPatientDBPath = correctFileFormat( m_fsPatientDBPath );

        if ( isAnFwxmlArchive( m_fsPatientDBPath ) )
        {
            manageZipAndSavePatientDB(m_fsPatientDBPath,associatedPatientDB);
        }
        else
        {
            savePatientDB(m_fsPatientDBPath,associatedPatientDB);
        }
        wxEndBusyCursor();
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
    wxString destZipFileName ( wxConvertMB2WX( inrFileDir.string().c_str() ) );
    wxString srcFolderName ( wxConvertMB2WX( srcFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::packFolder( srcFolderName, destZipFileName );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------
} // namespace ioXML
