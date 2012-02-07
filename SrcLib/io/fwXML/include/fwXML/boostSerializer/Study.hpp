/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_STUDY_HPP_
#define _FWXML_BOOSTSERIALIZER_STUDY_HPP_

#include <fwData/Study.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include <fwTools/fromIsoExtendedString.hpp>

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Study & _study, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "Hospital" , _study.getRefHospital() );
    ar &  ::boost::serialization::make_nvp( "Modality" , _study.getRefModality() );
    ar &  ::boost::serialization::make_nvp( "AcquisitionZone" , _study.getRefAcquisitionZone() );
    ar &  ::boost::serialization::make_nvp( "RISId" , _study.getRefRISId() );
    ar &  ::boost::serialization::make_nvp( "UID" , _study.getRefUID() );
    ar &  ::boost::serialization::make_nvp( "DbID" , _study.getRefDbID() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::Study & _study, const unsigned int version)
{
    std::string newStr;

    ar &  ::boost::serialization::make_nvp( "Hospital" , _study.getCRefHospital() );
    ar &  ::boost::serialization::make_nvp( "Modality" , _study.getCRefModality() );
    ar &  ::boost::serialization::make_nvp( "AcquisitionZone" , _study.getCRefAcquisitionZone() );
    ar &  ::boost::serialization::make_nvp( "RISId" ,  _study.getCRefRISId() );
    ar &  ::boost::serialization::make_nvp( "UID" , _study.getCRefUID() );
    ar &  ::boost::serialization::make_nvp( "DbID" , _study.getCRefDbID() );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Study & _study, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::Study, fwTools::Object>(NULL,NULL);

    split_free(ar,_study,version); // call load or save depending of archive type
}




} } // end namespace

#endif // _FWXML_BOOSTSERIALIZER_STUDY_HPP_

