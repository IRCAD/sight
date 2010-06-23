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

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>
#include <fwGui/ProgressDialog.hpp>

#include "ioXML/FwXMLImageWriterService.hpp"


namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLImageWriterService , ::fwData::Image ) ;
//REGISTER_EXECUTABLE( ::ioXML::FwXMLImageWriterService, "ioXML::FwXMLImageWriterService" );

//------------------------------------------------------------------------------

FwXMLImageWriterService::FwXMLImageWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

FwXMLImageWriterService::~FwXMLImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose a xml file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("fwXML (*.xml)|*.xml"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_SAVE,
#else
            wxSAVE,
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

void FwXMLImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLImageWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLImageWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLImageWriterService::getPersistanceId()
{
    return "ioITK::FwXMLImageWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::saveImage( const ::boost::filesystem::path inrFileDir, ::fwData::Image::sptr _pPatient )
{
    SLM_TRACE("FwXMLImageWriterService::createImage");
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

void FwXMLImageWriterService::updating() throw(fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
        assert( associatedImage ) ;

        wxBeginBusyCursor();
        saveImage(m_fsImagePath,associatedImage);
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioXML
