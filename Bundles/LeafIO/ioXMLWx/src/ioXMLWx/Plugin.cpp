/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/event.h>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientDBMsg.hpp>

#include <fwGui/Cursor.hpp>
#include <fwWX/convert.hpp>

#include <io/IReader.hpp>

#include <ioXML/FwXMLPatientDBReaderService.hpp>

#include "ioXMLWx/Plugin.hpp"

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

namespace ioXMLWx
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("ioXMLWx::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
#ifdef __WXMAC__
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    if (frame != NULL)
        frame->Connect( wxIDEventFwOpen, wxEventFwOpen, wxCommandEventHandler(wxEvtHandlerOpenFile::open) );
    else
        SLM_FATAL ("Window not found !")
#endif

}

void Plugin::stop() throw()
{}

} // namespace ioXMLWx
