/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/Vector.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

fwDataRegisterMacro( ::fwData::Vector );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Vector::s_ADDED_OBJECTS_SIG   = "addedObjects";
const ::fwCom::Signals::SignalKeyType Vector::s_REMOVED_OBJECTS_SIG = "removedObjects";

//------------------------------------------------------------------------------

Vector::Vector(::fwData::Object::Key)
{
    newSignal< AddedObjectsSignalType >(s_ADDED_OBJECTS_SIG);
    newSignal< RemovedObjectsSignalType >(s_REMOVED_OBJECTS_SIG);
}

//------------------------------------------------------------------------------

Vector::~Vector()
{
}

//------------------------------------------------------------------------------

void Vector::shallowCopy(const Object::csptr& _source )
{
    Vector::csptr other = Vector::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_container = other->m_container;
}

//------------------------------------------------------------------------------

void Vector::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    Vector::csptr other = Vector::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );
    m_container.clear();
    m_container.reserve(other->m_container.size());
    for(const ContainerType::value_type& obj : other->m_container)
    {
        m_container.push_back( ::fwData::Object::copy(obj, cache) );
    }
}

//------------------------------------------------------------------------------

}
