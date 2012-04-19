/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_OBJECTLOCK_HPP_
#define _FWDATA_OBJECTLOCK_HPP_

#include <vector>

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

    FWDATA_API ObjectLock(){}

    FWDATA_API ObjectLock( ::fwData::Object::sptr obj );

private :

    typedef std::vector< ::fwTools::BufferObject::Lock > LocksType;

    LocksType m_locks;
};

} // fwData

#endif // _FWDATA_OBJECTLOCK_HPP_
