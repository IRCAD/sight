/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwXML/reader/FwXMLObjectReader.hpp>
#include <fwWX/ProgressTowx.hpp>

#include "ioXML/FwXMLImageReaderService.hpp"


namespace ioXML
{

//REGISTER_SERVICE( ::io::IReader , FwXMLImageReaderService , ::fwData::Image ) ;
REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLImageReaderService , ::fwData::Image ) ;
//REGISTER_EXECUTABLE( ::ioXML::FwXMLImageReaderService, "ioXML::FwXMLImageReaderService" );

//------------------------------------------------------------------------------

FwXMLImageReaderService::FwXMLImageReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

FwXMLImageReaderService::~FwXMLImageReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    // Test if in the service configuration the tag filename is defined. If it is defined, the image path is initialized and we tag the service as configured.
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsImagePath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsImagePath);
        OSLM_TRACE("Filename found in service configuration : img path = " << filename ) ;
    }
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose a xml file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("fwXML|*.xml"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_FILE_MUST_EXIST,
#else
            wxFILE_MUST_EXIST,
#endif
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsImagePath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        m_bServiceIsConfigured = true;
        _sDefaultPath = wxConvertMB2WX( m_fsImagePath.branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageReaderService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageReaderService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLImageReaderService::info" ;
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwData::Image > FwXMLImageReaderService::createImage( const ::boost::filesystem::path inrFileDir )
{
    SLM_TRACE("FwXMLImageReaderService::createImage");
    ::fwXML::reader::FwXMLObjectReader myLoader;

    myLoader.setFile(inrFileDir);

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Loading Image ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
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

    ::boost::shared_ptr< ::fwData::Image > pImage = ::boost::dynamic_pointer_cast< ::fwData::Image > ( myLoader.getObject() );

    return pImage;
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageReaderService::updating()");

    if( m_bServiceIsConfigured )
    {
        ::boost::shared_ptr< ::fwData::Image > image = createImage( m_fsImagePath );

        if ( image != NULL )
        {

            // Retrieve dataStruct associated with this service
            ::boost::shared_ptr< ::fwTools::Object > associatedObject = this->getObject();
            ::boost::shared_ptr< ::fwData::Image > associatedImage = ::boost::dynamic_pointer_cast< ::fwData::Image >( associatedObject ) ;
            assert( associatedImage ) ;

            ( *( associatedImage ) ) = ( *( image.get() ) ) ;

            wxBeginBusyCursor();
            notificationOfDBUpdate();
            wxEndBusyCursor();

        }
        else
        {
            std::stringstream xmlFile;
            xmlFile << "Sorry, the xml file \""
            << m_fsImagePath.string()
            << "\" does not content a Image. This xml file has not been loaded.";
            wxString mes ( wxConvertMB2WX( xmlFile.str().c_str() ));
            wxMessageBox (  mes,
                    _("FwXML Image Reader"),
                    wxOK|wxICON_WARNING,
                    wxTheApp->GetTopWindow() );
        }
    }
}

//------------------------------------------------------------------------------

void FwXMLImageReaderService::notificationOfDBUpdate()
{
    SLM_TRACE("FwXMLImageReaderService::notificationOfDBUpdate");
    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    assert( pImage );

//   ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >( pImage ) ;
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ;
    msg->addEvent( ::fwComEd::ImageMsg::BUFFER ) ;
    msg->addEvent( ::fwComEd::ImageMsg::REGION ) ;
    msg->addEvent( ::fwComEd::ImageMsg::SPACING ) ;
    msg->addEvent( ::fwComEd::ImageMsg::PIXELTYPE ) ;

//  editor->notify( msg );
    ::fwServices::IEditionService::notify(this->getSptr(),  pImage, msg);
}

//------------------------------------------------------------------------------

} // namespace ioXML
