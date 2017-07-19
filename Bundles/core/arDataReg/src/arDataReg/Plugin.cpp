/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arDataReg/Plugin.hpp"

#include <arData/Camera.hpp>

#include <arDataCamp/Version.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace arDataReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::arDataReg::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    //Hack: force link with arData
    ::arData::Camera::sptr obj = ::arData::Camera::New();
    obj->getClassname();

    //Force link with fwDataCamp
    m_fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

    //Force link with fwMedDataCamp
    m_fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;

    //Force link with arDataCamp
    m_arDataVersion = ::arDataCamp::Version::s_CURRENT_VERSION;
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace arDataReg
