/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/helper.hpp>
#include <fwServices/helper.hpp>
#include "gui/sample/ITestService.hpp"

namespace gui
{
namespace sample
{


ITestService::ITestService() throw()
{}

ITestService::~ITestService() throw() 
{}

void ITestService::info(std::ostream &_sstream )
{
	_sstream << "ITestService" ;
}
}
}

