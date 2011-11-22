/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _REGION_HPP_
#define _REGION_HPP_

#include <fwData/Object.hpp>
#include <fwData/Region.hpp>
#include "fwXML/boostSerializer/Object.hpp"
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"


namespace boost {
namespace serialization {

/**
 * @brief serialize object using  boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, ::fwData::Region &_region, const unsigned int version)
{
    // serialize base class
     // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwData::Region, fwTools::Object>(NULL,NULL);

     ar &  ::boost::serialization::make_nvp( "Origin", _region.getRefOrigin() );
     ar &  ::boost::serialization::make_nvp( "Size"  , _region.getRefSize() );
}

} }

#endif // _REGION_HPP_

