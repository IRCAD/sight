/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/base.hpp>
#include <fwPython/python.hpp>
#include <fwPython/bindings/base.hpp>

#include "binding/Plugin.hpp"

namespace binding
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::binding::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
    SLM_TRACE_FUNC();

    if ( this->getBundle()->hasParameter("pythonhome") )
    {
        ::fwPython::setHome(this->getBundle()->getParameterValue("pythonhome"));
    }

    SLM_WARN_IF( "pythonhome is not set, you may encounter some problems",
            !this->getBundle()->hasParameter("pythonhome"));

    if ( this->getBundle()->hasParameter("pythonpath") )
    {
        ::fwPython::addPath(this->getBundle()->getParameterValue("pythonpath"));
    }

    ::fwPython::initialize();
    ::fwPython::bindings::initialize();
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//-----------------------------------------------------------------------------

} // namespace binding
