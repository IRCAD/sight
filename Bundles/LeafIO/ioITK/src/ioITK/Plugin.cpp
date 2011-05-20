/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

#include <inr2itk/itkInrImageIO.h>

#include "ioITK/Plugin.hpp"



namespace ioITK
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("ioITK::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    // force use of code in LIB itkInrImageIO ...
    typedef ::itk::InrImageIO::Self Dummy;
}

void Plugin::stop() throw()
{}

} // namespace ioITK
