/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_MATRIXTL_HPP__
#define __ARDATA_MATRIXTL_HPP__

#include "arData/GenericTL.hpp"
#include "arData/GenericTL.hxx"
#include "arData/timeline/GenericObject.hpp"
#include "arData/timeline/GenericObject.hxx"

namespace arData
{

/**
 * @brief   This class defines a timeline that stores groups of matrices.
 */
class ARDATA_CLASS_API MatrixTL : public GenericTL< float[16] >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MatrixTL)(::arData::TimeLine),(()), ::fwData::factory::New< MatrixTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MatrixTL( ::fwData::Object::Key key ) : GenericTL< float[16] >(key)
    {
    }
};

} // namespace arData


#endif // __ARDATA_MATRIXTL_HPP__
