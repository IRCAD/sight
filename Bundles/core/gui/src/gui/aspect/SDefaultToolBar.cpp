/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/aspect/SDefaultToolBar.hpp"

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwGui::IToolBarSrv, ::gui::aspect::SDefaultToolBar, ::fwData::Object );

namespace gui
{

namespace aspect
{

SDefaultToolBar::SDefaultToolBar() throw()
{
}

//-----------------------------------------------------------------------------

SDefaultToolBar::~SDefaultToolBar() throw()
{
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefaultToolBar::info( std::ostream& _sstream )
{
    SLM_TRACE_FUNC();
}

}

}
