/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/helper/Vector.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Vector.hpp>

namespace sight::data
{
namespace helper
{

//-----------------------------------------------------------------------------

Vector::Vector( data::Vector::wptr _vector ) :
    m_vector( _vector )
{
}

//-----------------------------------------------------------------------------

Vector::~Vector()
{
    if(!m_addedObjects.empty() || !m_removedObjects.empty())
    {
        notify();
    }
}

//-----------------------------------------------------------------------------

void Vector::add( data::Object::sptr _newObject )
{
    data::Vector::sptr vector = m_vector.lock();
    SIGHT_ASSERT( "The object " << _newObject->getID() << " must not exist in vector.",
                  std::find(vector->begin(), vector->end(), _newObject) == vector->end());

    // Modify vector
    vector->getContainer().push_back(_newObject );

    m_addedObjects.push_back(_newObject);
}

//-----------------------------------------------------------------------------

void Vector::remove( data::Object::sptr _oldObject )
{
    data::Vector::sptr vector   = m_vector.lock();
    data::Vector::iterator iter = std::find(vector->begin(), vector->end(), _oldObject);
    SIGHT_ASSERT( "The object " << _oldObject->getID() << " must exist in vector.",
                  iter != vector->end());

    // Modify vector
    vector->getContainer().erase( iter );

    m_removedObjects.push_back(_oldObject);

}

//-----------------------------------------------------------------------------

void Vector::clear()
{
    data::Vector::sptr vector = m_vector.lock();

    while (!vector->empty())
    {
        this->remove(vector->front());
    }
}

//-----------------------------------------------------------------------------

void Vector::notify()
{
    if ( !m_removedObjects.empty() )
    {
        auto sig = m_vector.lock()->signal< data::Vector::RemovedObjectsSignalType >(
            data::Vector::s_REMOVED_OBJECTS_SIG);
        sig->asyncEmit(m_removedObjects);
    }
    if ( !m_addedObjects.empty() )
    {
        auto sig = m_vector.lock()->signal< data::Vector::AddedObjectsSignalType >(
            data::Vector::s_ADDED_OBJECTS_SIG);
        sig->asyncEmit(m_addedObjects);
    }
    SIGHT_INFO_IF("No changes were found on the vector '" + m_vector.lock()->getID() + "', nothing to notify.",
                  m_addedObjects.empty() && m_removedObjects.empty());

    m_removedObjects.clear();
    m_addedObjects.clear();
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace sight::data
