/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _PROCESSOBJECT_BOOST_SERIALISER_HPP
#define _PROCESSOBJECT_BOOST_SERIALISER_HPP

#include <fwData/ProcessObject.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Object.hpp"




namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::ProcessObject &_algo, const unsigned int version)
{
     // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwData::ProcessObject , fwTools::Object>(NULL,NULL);
    std::string newStr;
    ar &  ::boost::serialization::make_nvp( "Name" , newStr );
    _algo.setName(newStr);



}

template<class Archive>
void save(Archive & ar, const ::fwData::ProcessObject &_algo, const unsigned int version)
{
     // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwData::ProcessObject, fwTools::Object>(NULL,NULL);
     ar &  ::boost::serialization::make_nvp( "Name" , _algo.getName() );

}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::ProcessObject &_algo, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::ProcessObject, fwTools::Object>(NULL,NULL);

    split_free(ar,_algo,version); // call load or save depending of archive type
}



} } // end namespace

#endif // ALGO_SERIALIZER_HPP

