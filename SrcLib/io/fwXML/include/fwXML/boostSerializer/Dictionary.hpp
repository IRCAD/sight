/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_DICTIONARY_HPP_
#define _FWXML_BOOSTSERIALIZER_DICTIONARY_HPP_

#include <fwCore/base.hpp>

#include <fwData/Dictionary.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Dictionary & _dictionary, const unsigned int version)
{
}

template<class Archive>
void save(Archive & ar, const ::fwData::Dictionary & _dictionary, const unsigned int version)
{
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Dictionary & _study, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::Dictionary, fwTools::Object>(NULL,NULL);

    split_free(ar,_study,version); // call load or save depending of archive type
}




} } // end namespace

#endif // _FWXML_BOOSTSERIALIZER_DICTIONARY_HPP_

