/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>

#include <boost/lexical_cast.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>


#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>


#include "gui/editor/DummyEditor.hpp"

namespace gui
{
namespace editor
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::gui::editor::DummyEditor , ::fwTools::Object ) ;


DummyEditor::DummyEditor() throw()
{}

//-----------------------------------------------------------------------------

DummyEditor::~DummyEditor() throw()
{}

//-----------------------------------------------------------------------------

void DummyEditor::starting() throw(::fwTools::Failed)
{
    ::gui::editor::IEditor::starting();
}

//-----------------------------------------------------------------------------

void DummyEditor::stopping() throw(::fwTools::Failed)
{
    ::gui::editor::IEditor::stopping();
}

//-----------------------------------------------------------------------------

void DummyEditor::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
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

