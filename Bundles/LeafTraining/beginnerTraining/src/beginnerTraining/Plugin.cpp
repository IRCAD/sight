/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwCore/spyLog.hpp>

#include "beginnerTraining/Plugin.hpp"

namespace beginnerTraining
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::beginnerTraining::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
	SLM_TRACE_FUNC();

//	std::string str (" with stream");
//	bool condition = true;
//	SLM_TRACE("Trace message");
//	OSLM_TRACE("Trace message" << str );
//	OSLM_TRACE_IF("Trace message" << str << " when condition = " << condition, condition );
//
//	SLM_DEBUG("Debug message");
//	SLM_INFO("Info message");
//	SLM_WARN("Warning message");
//	SLM_ERROR("Error message");
//	SLM_FATAL("Fatal message");
//
//	SLM_ASSERT("Fatal message", condition); // only compile in debug

}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
	SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace beginnerTraining
