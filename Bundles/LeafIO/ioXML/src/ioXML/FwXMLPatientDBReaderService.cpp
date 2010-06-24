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
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientDBMsg.hpp>

#include <io/IReader.hpp>

#include <fwTools/System.hpp>

#include <fwXML/reader/FwXMLObjectReader.hpp>

#include <fwGui/ProgressDialog.hpp>
#include <fwGui/LocationDialog.hpp>
#include <fwWX/wxZipFolder.hpp>
#include <fwWX/convert.hpp>

#include <fwGui/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLPatientDBReaderService.hpp"

//------------------------------------------------------------------------------

#ifdef __WXMAC__

class wxEvtHandlerOpenFile : public wxEvtHandler
{
    public:
    void open(wxCommandEvent& _event)
    {
        OSLM_TRACE("wxEvtHandlerOpenFile::open :" << _event.GetString().mb_str() );

        ::fwTools::Object::sptr pRootObject = ::fwServices::OSR::getRootObject();
        assert( pRootObject );

        ::fwData::Composite::sptr pComposite = ::fwData::Composite::dynamicCast( pRootObject );
        assert( pComposite );
        assert( pComposite->getRefMap().find("myPatientDB") != pComposite->getRefMap().end() );

        ::fwData::PatientDB::sptr pPatientDB = ::fwData::PatientDB::dynamicCast( pComposite->getRefMap()["myPatientDB"] );
        assert( pPatientDB );

        std::vector< std::string > availableExtensionsId;
        ::fwServices::eraseServices< ::io::IReader >( pPatientDB ) ;
        availableExtensionsId = ::fwServices::getImplementationIds< ::io::IReader >( pPatientDB ) ;
        std::string strIOXML ("::ioXML::FwXMLPatientDBReaderService");
        for(std::vector< std::string >::iterator itExt = availableExtensionsId.begin();
            itExt < availableExtensionsId.end();
            itExt++ )
        {
            std::string serviceId = *itExt;
            if (serviceId == strIOXML )
            {
                ::io::IReader::sptr service= ::fwServices::add< ::io::IReader >( pPatientDB , serviceId) ;
                assert(service);
                ::ioXML::FwXMLPatientDBReaderService::sptr reader = ::ioXML::FwXMLPatientDBReaderService::dynamicCast(service);
                assert(reader);
                reader->fixFilename( _event.GetString() );
                reader->start();

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);

                reader->update();
                cursor.setDefaultCursor();
                reader->stop();
            }
        }
    }

    DECLARE_DYNAMIC_CLASS( wxEvtHandlerOpenFile )
};

#endif

//------------------------------------------------------------------------------
//
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

    ::fwGui::LocationDialog dialogFile;
    dialogFile.setTitle( this->getSelectorDialogTitle() );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.fxz");
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::ILocationDialog::FILE_MUST_EXIST);

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

void FwXMLPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBReaderService::starting()");
#ifdef __WXMAC__
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    if (frame != NULL)
        frame->Connect( wxIDEventFwOpen, wxEventFwOpen, wxCommandEventHandler(wxEvtHandlerOpenFile::open) );
    else
        SLM_FATAL ("Window not found !")
#endif
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLPatientDBReaderService::stopping()");
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
    SLM_TRACE("FwXMLPatientDBReaderService::createPatientDB");
    ::fwXML::reader::FwXMLObjectReader myLoader;
    ::fwData::PatientDB::sptr pPatientDB;

    myLoader.setFile(inrFileDir);

    try
    {
        ::fwGui::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        wxString msg = _("Warning during loading : ");
        ss << wxConvertWX2MB(msg.c_str()) << e.what();
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
        return pPatientDB;
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
                assert( associatedPatientDB ) ;

                associatedPatientDB->shallowCopy( patientDB );

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);

                notificationOfDBUpdate();
                cursor.setDefaultCursor();
            }
            else
            {
                ::fwGui::MessageDialog messageBox;
                messageBox.setTitle("Image Reader");
                messageBox.setMessage( "File format unknown. Retry with another file reader." );
                messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
                messageBox.addButton(::fwGui::IMessageDialog::OK);
                messageBox.show();
            }
        }
        else
        {
            std::stringstream xmlFile;
            xmlFile << "Sorry, the xml file \""
            << m_fsPatientDBPath.string()
            << "\" does not content a PatientDB. This xml file has not been loaded.";

            ::fwGui::MessageDialog messageBox;
            messageBox.setTitle("FwXML PatientDB Reader");
            messageBox.setMessage( xmlFile.str() );
            messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

void FwXMLPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE("FwXMLPatientDBReaderService::notificationOfDBUpdate");
    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();
    assert( pDPDB );

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
    ::boost::filesystem::path xmlfile = destFolder / "patient.xml";

    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );

    wxString srcZipFileName ( wxConvertMB2WX( _pArchivePath.string().c_str() ) );
    wxString destFolderName ( wxConvertMB2WX( destFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::unpackFolder( srcZipFileName, destFolderName );

    // Load
    patientDB = createPatientDB( xmlfile );

    // Remove temp folder
    ::boost::filesystem::remove_all( destFolder );

    return patientDB;
}

//------------------------------------------------------------------------------

} // namespace ioXML
