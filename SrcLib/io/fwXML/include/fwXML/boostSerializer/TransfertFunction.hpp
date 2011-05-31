/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTION_HPP_
#define _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTION_HPP_

#include <fwData/TransfertFunction.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include <fwTools/fromIsoExtendedString.hpp>

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::TransfertFunction & _transfertFunction, const unsigned int version)
{
     ar &  ::boost::serialization::make_nvp( "Name"       , _transfertFunction.getRefName() );
     ar &  ::boost::serialization::make_nvp( "Encoding"   , _transfertFunction.getRefEncoding() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::TransfertFunction & _transfertFunction, const unsigned int version)
{
    std::string newStr;
    
//  newStr = ::fwTools::toStringWithoutAccent( _transfertFunction.getCRefName() );
    ar &  ::boost::serialization::make_nvp( "Name"        , _transfertFunction.getCRefName() );
    
//  newStr = ::fwTools::toStringWithoutAccent( _transfertFunction.getCRefEncoding() );
    ar &  ::boost::serialization::make_nvp( "Encoding"    , _transfertFunction.getCRefEncoding() );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::TransfertFunction & _transfertFunction, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::TransfertFunction, fwTools::Object>(NULL,NULL);

    split_free(ar,_transfertFunction,version); // call load or save depending of archive type
}



} } // end namespace

INSTANTIATE_SERIALIZER(::fwData::TransfertFunction);

#endif // _FWXML_BOOSTSERIALIZER_TRANSFERTFUNCTION_HPP_

