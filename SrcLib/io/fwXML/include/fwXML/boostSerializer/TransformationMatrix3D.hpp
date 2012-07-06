/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_TRANSFORMATIONMATRIX3D_HPP_
#define _FWXML_BOOSTSERIALIZER_TRANSFORMATIONMATRIX3D_HPP_

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::TransformationMatrix3D & _transformationMatrix3D, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "Coefficients" , _transformationMatrix3D.getRefCoefficients() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::TransformationMatrix3D & _transformationMatrix3D, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "Coefficients" , _transformationMatrix3D.getCRefCoefficients() );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::TransformationMatrix3D & _transformationMatrix3D, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::TransformationMatrix3D, fwTools::Object>(NULL,NULL);

    split_free(ar,_transformationMatrix3D,version); // call load or save depending of archive type
}



} } // end namespace

#endif // _FWXML_BOOSTSERIALIZER_TRANSFORMATIONMATRIX3D_HPP_

