/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwPython/Interpreter.hpp>

#include "uiPython/action/Test.hpp"

namespace uiPython
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPython::action::Test , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

Test::Test() throw()
{}

//-----------------------------------------------------------------------------

Test::~Test() throw()
{}

//-----------------------------------------------------------------------------

void Test::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();
}

//-----------------------------------------------------------------------------

void Test::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Test::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void Test::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void Test::info(std::ostream &_sstream )
{
    _sstream << "Test Action" << std::endl;
}

//-----------------------------------------------------------------------------

void Test::updating() throw( ::fwTools::Failed )
{
    ::fwPython::Interpreter python;
    python.execute("print \"Hello fw4spl world\"");
}

//-----------------------------------------------------------------------------

}
}
