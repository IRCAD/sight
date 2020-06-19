/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwPacsIO/data/PacsConfiguration.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwPacsIO::data::PacsConfiguration );

namespace fwPacsIO
{
namespace data
{

PacsConfiguration::PacsConfiguration(::fwData::Object::Key key) :
    m_localApplicationTitle(""),
    m_pacsHostName(""),
    m_pacsApplicationTitle(""),
    m_pacsApplicationPort(0),
    m_moveApplicationTitle(""),
    m_moveApplicationPort(0),
    m_retrieveMethod(MOVE_RETRIEVE_METHOD)
{
}

//------------------------------------------------------------------------------

PacsConfiguration::~PacsConfiguration()
{
}

//------------------------------------------------------------------------------

void PacsConfiguration::shallowCopy(const Object::csptr& _source )
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_localApplicationTitle = other->m_localApplicationTitle;
    m_pacsHostName          = other->m_pacsHostName;
    m_pacsApplicationTitle  = other->m_pacsApplicationTitle;
    m_pacsApplicationPort   = other->m_pacsApplicationPort;
    m_moveApplicationTitle  = other->m_moveApplicationTitle;
    m_moveApplicationPort   = other->m_moveApplicationPort;
    m_retrieveMethod        = other->m_retrieveMethod;
}

//------------------------------------------------------------------------------

void PacsConfiguration::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_localApplicationTitle = other->m_localApplicationTitle;
    m_pacsHostName          = other->m_pacsHostName;
    m_pacsApplicationTitle  = other->m_pacsApplicationTitle;
    m_pacsApplicationPort   = other->m_pacsApplicationPort;
    m_moveApplicationTitle  = other->m_moveApplicationTitle;
    m_moveApplicationPort   = other->m_moveApplicationPort;
    m_retrieveMethod        = other->m_retrieveMethod;
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace fwPacsIO
