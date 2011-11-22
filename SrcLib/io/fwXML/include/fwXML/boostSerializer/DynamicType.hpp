/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DYNAMICTYPE_SERIALISER_HPP
#define _DYNAMICTYPE_SERIALISER_HPP

#include <fwTools/DynamicType.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"



namespace boost {
namespace serialization {

/**
 * @brief serialize dynamicType boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void save(Archive & ar, const fwTools::DynamicType &_dynamicType, const unsigned int version)
{

    ar & make_nvp("DynamicType",_dynamicType.string());
}

template<class Archive>
void load(Archive & ar, fwTools::DynamicType &_dynamicType, const unsigned int version)
{
    std::string dynamicTypeLitteralValue;
    ar & make_nvp("DynamicType",dynamicTypeLitteralValue);

    if ( dynamicTypeLitteralValue == fwTools::DynamicType().string() )
    {
        _dynamicType = fwTools::DynamicType();
    }
    else
    {
        // dynamique cannot be constructed with UNSPECIFIED PIXELTYPE STRING
        _dynamicType = fwTools::makeDynamicType(dynamicTypeLitteralValue);
    }
}





/// serialize dynamicType via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, fwTools::DynamicType &dynamicType, const unsigned int version)
{
    split_free(ar,dynamicType,version); // call load or save depending of archive type
}



} }

#endif // _DYNAMICTYPE_SERIALISER_HPP
