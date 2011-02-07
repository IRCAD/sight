/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_PATIENT_HPP_
#define _FWXML_BOOSTSERIALIZER_PATIENT_HPP_

#include <fwData/Patient.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include <fwTools/fromIsoExtendedString.hpp>

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Patient & _patient, const unsigned int version)
{
    ar &  boost::serialization::make_nvp( "Name" , _patient.getRefName() );

    ar &  boost::serialization::make_nvp( "Firstname" , _patient.getRefFirstname() );

    ar &  boost::serialization::make_nvp( "IDDicom" , _patient.getRefIDDicom() );

    std::string birthdate;
    ar &  boost::serialization::make_nvp( "Birthdate" , birthdate );
    _patient.setCRefBirthdate( fwTools::fromIsoExtendedString( birthdate ) );

    ar &  boost::serialization::make_nvp( "IsMale" , _patient.getRefIsMale() );

    ar &  boost::serialization::make_nvp( "DbID" , _patient.getRefDbID() );

}

template<class Archive>
void save(Archive & ar, const ::fwData::Patient & _patient, const unsigned int version)
{
    std::string newStr;

    ar &  boost::serialization::make_nvp( "Name" , _patient.getCRefName() );

    ar &  boost::serialization::make_nvp( "Firstname" , _patient.getCRefFirstname() );

    ar &  boost::serialization::make_nvp( "IDDicom" , _patient.getCRefIDDicom() );

    std::string birthdate = fwTools::toIsoExtendedString( _patient.getCRefBirthdate() );
    ar &  boost::serialization::make_nvp( "Birthdate" , birthdate );

    ar &  boost::serialization::make_nvp( "IsMale" , _patient.getCRefIsMale() );

    ar &  boost::serialization::make_nvp( "DbID" , _patient.getCRefDbID() );


}



/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Patient & _patient, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register<  ::fwData::Patient, fwTools::Object>(NULL,NULL);

    split_free(ar,_patient,version); // call load or save depending of archive type
}



} } // end namespace


INSTANTIATE_SERIALIZER(::fwData::Patient);



#endif // _FWXML_BOOSTSERIALIZER_PATIENT_HPP_

