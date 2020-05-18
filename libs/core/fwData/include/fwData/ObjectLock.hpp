/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWDATA_OBJECTLOCK_HPP__
#define __FWDATA_OBJECTLOCK_HPP__


#include "fwData/Image.hpp"
#include "fwData/Array.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Reconstruction.hpp"
#include "fwData/config.hpp"

#include <vector>

namespace fwData
{


/**
 * @brief A simple helper to lock specific object, manages : Image, Mesh and Array.
 */
class FWDATA_CLASS_API ObjectLock
{

public:

    ObjectLock()
    {
    }

    FWDATA_API ObjectLock( const ObjectLock & );
    FWDATA_API ObjectLock( ::fwData::Object::sptr obj );

    FWDATA_API ~ObjectLock();

    FWDATA_API ObjectLock & operator=(const ObjectLock & );
private:


    typedef std::vector< ::fwMemory::BufferObject::Lock > LocksType;
    typedef std::vector< ::fwData::Object::sptr > ObjectsType;

    void lock( ::fwData::Array::sptr array, LocksType & locks );
    void lock( ::fwData::Image::sptr image, LocksType & locks );
    void lock( ::fwData::Mesh::sptr mesh, LocksType & locks );
    void lock( ::fwData::Reconstruction::sptr rec, LocksType & locks );

    LocksType m_locks;
    ObjectsType m_objects;
};

} // fwData

#endif // __FWDATA_OBJECTLOCK_HPP__
