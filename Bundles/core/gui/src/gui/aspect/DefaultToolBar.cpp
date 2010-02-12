/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include "gui/aspect/IToolBar.hpp"
#include "gui/aspect/DefaultToolBar.hpp"

REGISTER_SERVICE( ::gui::aspect::IToolBar , ::gui::aspect::DefaultToolBar , ::fwTools::Object ) ;

namespace gui
{

namespace aspect
{

DefaultToolBar::DefaultToolBar() throw()
{}

//-----------------------------------------------------------------------------

DefaultToolBar::~DefaultToolBar() throw()
{}

//-----------------------------------------------------------------------------

void DefaultToolBar::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->::gui::aspect::IToolBar::starting();
}

//-----------------------------------------------------------------------------

void DefaultToolBar::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->::gui::aspect::IToolBar::stopping();
}

//-----------------------------------------------------------------------------

void DefaultToolBar::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultToolBar::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultToolBar::updating(::fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultToolBar::info( std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

}

}
