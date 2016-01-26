/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "extDataReg/Plugin.hpp"

#include <extData/FrameTL.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>


namespace extDataReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::extDataReg::Plugin");

Plugin::~Plugin() throw()
{
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    //Hack: force link with arData
    ::extData::FrameTL::sptr obj = ::extData::FrameTL::New();
    obj->getClassname();
}

void Plugin::stop() throw()
{
}

} // namespace extDataReg
