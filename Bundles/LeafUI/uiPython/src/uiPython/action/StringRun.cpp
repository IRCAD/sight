/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwData/String.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwPython/Interpreter.hpp>

#include "uiPython/action/StringRun.hpp"

namespace uiPython
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPython::action::StringRun , ::fwData::String ) ;

//-----------------------------------------------------------------------------

StringRun::StringRun() throw()
{}

//-----------------------------------------------------------------------------

StringRun::~StringRun() throw()
{}

//-----------------------------------------------------------------------------

void StringRun::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();
}

//-----------------------------------------------------------------------------

void StringRun::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void StringRun::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void StringRun::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void StringRun::info(std::ostream &_sstream )
{
    _sstream << "StringRun Action" << std::endl;
}

//-----------------------------------------------------------------------------

void StringRun::updating() throw( ::fwTools::Failed )
{
    ::fwData::String::sptr str = this->getObject< ::fwData::String >();
    ::fwPython::Interpreter python;
    python.execute(str->value().c_str());
}

//-----------------------------------------------------------------------------

}
}
