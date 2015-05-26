/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_MARKERTL_HPP__
#define __ARDATA_MARKERTL_HPP__

#include "arData/config.hpp"

#include "extData/GenericTL.hpp"
#include "extData/GenericTL.hxx"
#include "extData/timeline/GenericObject.hpp"
#include "extData/timeline/GenericObject.hxx"

namespace arData
{

/**
 * @class   MarkerTL
 * @brief   This class defines a timeline that stores groups of markers.
 */
class ARDATA_CLASS_API MarkerTL : public ::extData::GenericTL< float[8] >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MarkerTL)(::extData::TimeLine),(()), ::fwData::factory::New< MarkerTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API MarkerTL( ::fwData::Object::Key key ) : ::extData::GenericTL< float[8] >(key) {}
};

} // namespace arData


#endif // __ARDATA_MARKERTL_HPP__
