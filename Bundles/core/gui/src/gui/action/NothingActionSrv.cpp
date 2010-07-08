/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>


#include "gui/action/NothingActionSrv.hpp"

namespace gui
{
namespace action
{
REGISTER_SERVICE( ::fwGui::IActionSrv , ::gui::action::NothingActionSrv , ::fwTools::Object ) ;


NothingActionSrv::NothingActionSrv() throw()
{
}

//-----------------------------------------------------------------------------

NothingActionSrv::~NothingActionSrv() throw()
{
}

//-----------------------------------------------------------------------------

void NothingActionSrv::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void NothingActionSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void NothingActionSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void NothingActionSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void NothingActionSrv::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void NothingActionSrv::info(std::ostream &_sstream )
{
    _sstream << "Nothing Action" << std::endl;
}

}
}

