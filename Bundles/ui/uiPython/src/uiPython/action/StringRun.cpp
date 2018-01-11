/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include <fwPython/Interpreter.hpp>

#include "uiPython/action/StringRun.hpp"

namespace uiPython
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiPython::action::StringRun, ::fwData::String );

//-----------------------------------------------------------------------------

StringRun::StringRun() noexcept
{
}

//-----------------------------------------------------------------------------

StringRun::~StringRun() noexcept
{
}

//-----------------------------------------------------------------------------

void StringRun::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void StringRun::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void StringRun::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void StringRun::info(std::ostream& _sstream )
{
    _sstream << "StringRun Action" << std::endl;
}

//-----------------------------------------------------------------------------

void StringRun::updating()
{
    ::fwData::String::sptr str = this->getObject< ::fwData::String >();
    ::fwPython::Interpreter python;
    python.execute(str->value().c_str());
}

//-----------------------------------------------------------------------------

}
}
