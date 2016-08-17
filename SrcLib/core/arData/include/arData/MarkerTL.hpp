/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_MARKERTL_HPP__
#define __ARDATA_MARKERTL_HPP__

#include "arData/config.hpp"

#include "arData/GenericTL.hpp"
#include "arData/GenericTL.hxx"
#include "arData/timeline/GenericObject.hpp"
#include "arData/timeline/GenericObject.hxx"

namespace arData
{

/**
 * @brief   This class defines a timeline that stores groups of markers.
 */
class ARDATA_CLASS_API MarkerTL : public ::arData::GenericTL< float[8] >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MarkerTL)(::arData::TimeLine),(()), ::fwData::factory::New< MarkerTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API MarkerTL( ::fwData::Object::Key key ) : ::arData::GenericTL< float[8] >(key)
    {
    }
};

} // namespace arData


#endif // __ARDATA_MARKERTL_HPP__
