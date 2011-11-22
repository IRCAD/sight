/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_POINT_HPP_
#define _FWXML_BOOSTSERIALIZER_POINT_HPP_

#include <fwData/Point.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"



namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Point & _point, const unsigned int version)
{
    std::vector< ::fwData::Point::PointCoordType > tmp; // compatibility mode std::vector -> ::boost::array
    ar & ::boost::serialization::make_nvp( "Coord" , tmp );
    std::copy(tmp.begin(), tmp.end(), _point.getRefCoord().begin());
}

template<class Archive>
void save(Archive & ar, const ::fwData::Point & _point, const unsigned int version)
{
    std::vector< ::fwData::Point::PointCoordType > tmp( _point.getCRefCoord().size() ); // compatibility mode std::vector -> ::boost::array
    std::copy( _point.getCRefCoord().begin(), _point.getCRefCoord().end(), tmp.begin());
    ar &  ::boost::serialization::make_nvp( "Coord" , tmp );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Point & _point, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register<  ::fwData::Point, ::fwTools::Object>(NULL,NULL);

    split_free(ar,_point,version); // call load or save depending of archive type
}




} } // end namespace

#endif // _FWXML_BOOSTSERIALIZER_POINT_HPP_

