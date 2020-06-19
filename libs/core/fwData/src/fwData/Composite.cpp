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

#include "fwData/Composite.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <algorithm>

fwDataRegisterMacro( ::fwData::Composite );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Composite::s_ADDED_OBJECTS_SIG   = "addedObjects";
const ::fwCom::Signals::SignalKeyType Composite::s_CHANGED_OBJECTS_SIG = "changedObjects";
const ::fwCom::Signals::SignalKeyType Composite::s_REMOVED_OBJECTS_SIG = "removedObjects";

//------------------------------------------------------------------------------

Composite::Composite( ::fwData::Object::Key )
{
    newSignal< AddedObjectsSignalType >(s_ADDED_OBJECTS_SIG);
    newSignal< ChangedObjectsSignalType >(s_CHANGED_OBJECTS_SIG);
    newSignal< RemovedObjectsSignalType >(s_REMOVED_OBJECTS_SIG);
}

//------------------------------------------------------------------------------

Composite::~Composite()
{
}

//------------------------------------------------------------------------------

void Composite::shallowCopy(const Object::csptr& _source )
{
    Composite::csptr other = Composite::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_container.clear();

    m_container = other->m_container;
}

//------------------------------------------------------------------------------

void Composite::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Composite::csptr other = Composite::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_container.clear();

    for(const ValueType& elem : *other)
    {
        m_container.insert( ValueType(elem.first, ::fwData::Object::copy(elem.second, cache) ) );
    }
}

//------------------------------------------------------------------------------

}
