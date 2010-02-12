/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include "ioTuto/CompositeWriterService.hpp"

namespace ioTuto
{

REGISTER_SERVICE( ::io::IWriter , ioTuto::CompositeWriterService , ::fwData::Composite ) ;

//------------------------------------------------------------------------------
CompositeWriterService::CompositeWriterService( )throw()
: m_fsExternalDataPath("")
{
}

//------------------------------------------------------------------------------
CompositeWriterService::~CompositeWriterService() throw()
{}

//------------------------------------------------------------------------------
void CompositeWriterService::info(std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------
void CompositeWriterService::configuring() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------
void CompositeWriterService::starting() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------
void CompositeWriterService::stopping() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------
void CompositeWriterService::updating( ) throw(::fwTools::Failed)
{
    configureWithIHM();
    ::fwData::Composite::sptr pComposite = this->getObject< ::fwData::Composite >();
    assert( pComposite );

    ::fwXML::writer::FwXMLObjectWriter myWriter;

    if (m_fsExternalDataPath != "")
    {
        myWriter.setObject(pComposite);
        myWriter.setFile(m_fsExternalDataPath);

        try
        {
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
}

//------------------------------------------------------------------------------
void CompositeWriterService::updating(::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg) throw(::fwTools::Failed)
{
}
//-----------------------------------------------------------------------------
void CompositeWriterService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Xml file to save");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("xml file (*.xml)|*.xml"),
            wxFD_SAVE,
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsExternalDataPath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        _sDefaultPath = wxConvertMB2WX( m_fsExternalDataPath.branch_path().string().c_str() );
}
}
//------------------------------------------------------------------------------
} // namespace ioTuto
