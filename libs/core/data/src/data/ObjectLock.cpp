/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/ObjectLock.hpp"

#include <core/memory/BufferObject.hpp>

namespace sight::data
{

//-----------------------------------------------------------------------------

ObjectLock::~ObjectLock()
{
    m_locks.clear();
    m_objects.clear();
}

//-----------------------------------------------------------------------------

ObjectLock::ObjectLock(const ObjectLock& objectLock)
{
    m_objects = objectLock.m_objects;
    m_locks   = objectLock.m_locks;
}

//-----------------------------------------------------------------------------

ObjectLock& ObjectLock::operator=(const ObjectLock& objectLock)
{
    m_locks.clear();
    m_objects.clear();

    m_objects = objectLock.m_objects;
    m_locks   = objectLock.m_locks;

    return *this;
}

//-----------------------------------------------------------------------------

void ObjectLock::lock( data::Array::sptr array, LocksType& locks )
{
    if ( array )
    {
        locks.push_back( array->getBufferObject()->lock() );
        m_objects.push_back(array);
    }
}

//-----------------------------------------------------------------------------

void ObjectLock::lock( data::Image::sptr image, LocksType& locks )
{
    if ( image )
    {
        locks.push_back(image->lock());
    }
}

//-----------------------------------------------------------------------------

void ObjectLock::lock( data::Mesh::sptr mesh, LocksType& locks )
{
    if ( mesh )
    {
        LocksType meshLocks = mesh->lock();
        std::copy(meshLocks.begin(), meshLocks.end(), std::back_inserter(locks));
    }
}

//-----------------------------------------------------------------------------

void ObjectLock::lock( data::Reconstruction::sptr rec, LocksType& locks )
{
    if ( rec )
    {
        this->lock( rec->getImage(), locks );
        this->lock( rec->getMesh(), locks );
    }
}

//-----------------------------------------------------------------------------

ObjectLock::ObjectLock( data::Object::sptr obj )
{
    data::Image::sptr image = data::Image::dynamicCast( obj );
    data::Mesh::sptr mesh   = data::Mesh::dynamicCast( obj );
    data::Array::sptr array = data::Array::dynamicCast( obj );

    if( image )
    {
        this->lock( image, m_locks );
    }
    else if ( mesh )
    {
        this->lock( mesh, m_locks );
    }
    else if ( array )
    {
        this->lock( array, m_locks );
    }
}

//-----------------------------------------------------------------------------

} // fwData
