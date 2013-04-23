/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_OBJECTLOCK_HPP_
#define _FWDATA_OBJECTLOCK_HPP_

#include <vector>

#include "fwData/Image.hpp"
#include "fwData/Array.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Reconstruction.hpp"

#include "fwData/config.hpp"

namespace fwData
{


/**
 * @class  ObjectLock.
 * @brief  A simple helper to lock specific object, manages : Image, Mesh and Array.
 * @author IRCAD (Research and Development Team).
 * @date   2007-2012.
 */
class FWDATA_CLASS_API ObjectLock
{

public :

    ObjectLock(){}

    FWDATA_API ObjectLock( const ObjectLock & );
    FWDATA_API ObjectLock( ::fwData::Object::sptr obj );

    FWDATA_API ~ObjectLock();

    FWDATA_API ObjectLock & operator=(const ObjectLock & );
private :


    typedef std::vector< ::fwTools::BufferObject::Lock > LocksType;
    typedef std::vector< ::fwData::Object::sptr > ObjectsType;

    void lock( ::fwData::Array::sptr array, LocksType & locks );
    void lock( ::fwData::Image::sptr image, LocksType & locks );
    void lock( ::fwData::Mesh::sptr mesh, LocksType & locks );
    void lock( ::fwData::Reconstruction::sptr rec, LocksType & locks );

    LocksType m_locks;
    ObjectsType m_objects;
};

} // fwData

#endif // _FWDATA_OBJECTLOCK_HPP_
