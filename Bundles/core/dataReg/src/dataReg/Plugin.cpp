/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "dataReg/Plugin.hpp"

#include <fwData/Float.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedData/Patient.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace dataReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::dataReg::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    //Hack: force link with fwData
    ::fwData::Float::sptr obj = ::fwData::Float::New();
    obj->getClassname();

    //Hack: force link with fwMedData
    ::fwMedData::Patient::sptr pat = ::fwMedData::Patient::New();
    pat->getClassname();

    //Force link with fwDataCamp
    m_fwDataCampVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

    //Force link with fwMedDataCamp
    m_fwMedDataCampVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace dataReg
