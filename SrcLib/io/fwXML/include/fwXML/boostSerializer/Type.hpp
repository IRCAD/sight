/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DYNAMICTYPE_SERIALISER_HPP
#define _DYNAMICTYPE_SERIALISER_HPP

#include <fwTools/Type.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"



namespace boost {
namespace serialization {


/**
 * @brief serialize Type boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void save(Archive & ar, const fwTools::Type &_type, const unsigned int version)
{

    ar & make_nvp("Type",_type.string());
}

template<class Archive>
void load(Archive & ar, fwTools::Type &_type, const unsigned int version)
{
    std::string typeLitteralValue;
    ar & make_nvp("Type",typeLitteralValue);

    if ( typeLitteralValue == fwTools::Type().string() )
    {
        _type = fwTools::Type();
    }
    else
    {
        _type = fwTools::Type(typeLitteralValue);
    }
}





/// serialize Type via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, fwTools::Type &type, const unsigned int version)
{
    split_free(ar,type,version); // call load or save depending of archive type
}





} }

#endif // _DYNAMICTYPE_SERIALISER_HPP

