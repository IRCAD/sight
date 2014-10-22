/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_MATRIXTL_HPP__
#define __EXTDATA_MATRIXTL_HPP__

#include "extData/GenericTL.hpp"
#include "extData/GenericTL.hxx"
#include "extData/timeline/GenericObject.hpp"
#include "extData/timeline/GenericObject.hxx"

namespace extData
{

/**
 * @class   MatrixTL
 * @brief   This class defines a timeline that stores groups of matrices.
 */
class EXTDATA_CLASS_API MatrixTL : public GenericTL< float[16] >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MatrixTL)(::extData::TimeLine),(()), ::fwData::factory::New< MatrixTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MatrixTL( ::fwData::Object::Key key ) : GenericTL< float[16] >(key) {}
};

} // namespace extData


#endif // __EXTDATA_MATRIXTL_HPP__
