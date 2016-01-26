/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
