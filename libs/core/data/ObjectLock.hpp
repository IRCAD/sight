/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "data/Array.hpp"
#include "data/config.hpp"
#include "data/Image.hpp"
#include "data/Mesh.hpp"
#include "data/Reconstruction.hpp"

#include <vector>

namespace sight::data
{

/**
 * @brief A simple helper to lock specific object, manages : Image, Mesh and Array.
 */
class DATA_CLASS_API ObjectLock
{
public:

    ObjectLock()
    {
    }

    DATA_API ObjectLock(const ObjectLock&);
    DATA_API ObjectLock(data::Object::sptr obj);

    DATA_API ~ObjectLock();

    DATA_API ObjectLock& operator=(const ObjectLock&);

private:

    typedef std::vector<core::memory::BufferObject::Lock> LocksType;
    typedef std::vector<data::Object::sptr> ObjectsType;

    void lock(data::Array::sptr array, LocksType& locks);
    void lock(data::Image::sptr image, LocksType& locks);
    void lock(data::Mesh::sptr mesh, LocksType& locks);
    void lock(data::Reconstruction::sptr rec, LocksType& locks);

    LocksType m_locks;
    ObjectsType m_objects;
};

} // fwData
