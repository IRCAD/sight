/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTIONPOINT_HPP_
#define _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTIONPOINT_HPP_

#include <fwData/TransfertFunctionPoint.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"



namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::TransfertFunctionPoint & _transfertFunctionPoint, const unsigned int version)
{
     ar &  boost::serialization::make_nvp( "Value" , _transfertFunctionPoint.getRefValue() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::TransfertFunctionPoint & _transfertFunctionPoint, const unsigned int version)
{
    ar &  boost::serialization::make_nvp( "Value" , _transfertFunctionPoint.getCRefValue() );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::TransfertFunctionPoint & _transfertFunctionPoint, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     boost::serialization::void_cast_register<  ::fwData::TransfertFunctionPoint, fwTools::Object>(NULL,NULL);

    split_free(ar,_transfertFunctionPoint,version); // call load or save depending of archive type
}



} } // end namespace

INSTANTIATE_SERIALIZER(::fwData::TransfertFunctionPoint);

#endif // _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTIONPOINT_HPP_

