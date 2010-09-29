/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/colour.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwWX/convert.hpp>
#include <fwTools/UUID.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "guiWx/editor/DummyEditor.hpp"

namespace guiWx
{
namespace editor
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::guiWx::editor::DummyEditor , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

DummyEditor::DummyEditor() throw() : m_text("")
{}

//-----------------------------------------------------------------------------

DummyEditor::~DummyEditor() throw()
{}

//-----------------------------------------------------------------------------

void DummyEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    wxBoxSizer* bSizer;
    bSizer = new wxBoxSizer( wxVERTICAL );
    std::string text = m_text.empty() ? this->getUUID() : m_text;
    wxStaticText*  staticText = new wxStaticText( container, wxID_ANY, ::fwWX::std2wx(text), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer->Add( staticText, 1, wxALL|wxEXPAND, 5 );
    container->SetSizer( bSizer );
    container->SetBackgroundColour(wxColour(rand()%256, rand()%256, rand()%256));
    container->Refresh();
}

//-----------------------------------------------------------------------------

void DummyEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container );
    container->SetBackgroundColour(wxNullColour);
    wxContainer->clean();
}

//-----------------------------------------------------------------------------

void DummyEditor::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();

    ::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("text");
    if(viewsCfgElt)
    {
        m_text = viewsCfgElt->getValue();
    }
}

//-----------------------------------------------------------------------------

void DummyEditor::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DummyEditor::updating(::fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DummyEditor::info( std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace guiWx

