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

#include "gui/editor/DummyEditor.hpp"

namespace gui
{
namespace editor
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::gui::editor::DummyEditor , ::fwTools::Object ) ;


DummyEditor::DummyEditor() throw()
{
    m_text="";
}

//-----------------------------------------------------------------------------

DummyEditor::~DummyEditor() throw()
{}

//-----------------------------------------------------------------------------

void DummyEditor::starting() throw(::fwTools::Failed)
{
    ::gui::editor::IEditor::starting();

    wxBoxSizer* bSizer;
    bSizer = new wxBoxSizer( wxVERTICAL );
    std::string text = m_text.empty() ? this->getUUID() : m_text;
    wxStaticText*  staticText = new wxStaticText( m_container, wxID_ANY, ::fwWX::std2wx(text), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer->Add( staticText, 1, wxALL|wxEXPAND, 5 );
    m_container->SetSizer( bSizer );
    m_container->SetBackgroundColour(wxColour(rand()%256, rand()%256, rand()%256));
    m_container->Refresh();
}

//-----------------------------------------------------------------------------

void DummyEditor::stopping() throw(::fwTools::Failed)
{
    m_container->SetBackgroundColour(wxNullColour);
    ::gui::editor::IEditor::stopping();
}

//-----------------------------------------------------------------------------

void DummyEditor::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::gui::editor::IEditor::configuring();

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
}
}

