/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arDataReg/Plugin.hpp"

#include <arDataCamp/Version.hpp>
#include <arData/Camera.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwDataCamp/Version.hpp>

namespace arDataReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::arDataReg::Plugin");

Plugin::~Plugin() throw()
{
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    //Hack: force link with arData
    ::arData::Camera::sptr obj = ::arData::Camera::New();
    obj->getClassname();

    //Force link with fwDataCamp
    static const int fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

    //Force link with arDataCamp
    static const int arfVersion = ::arDataCamp::Version::s_CURRENT_VERSION;
}

void Plugin::stop() throw()
{
}

} // namespace arDataReg
