/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _HISTOGRAM_BOOST_SERIALISER_HPP
#define _HISTOGRAM_BOOST_SERIALISER_HPP


#include <fwData/Histogram.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"




namespace boost {
namespace serialization {

/**
 * @brief serialize histogram boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void save(Archive & ar, const ::fwData::Histogram &_histogram, const unsigned int version)
{
    ::boost::serialization::void_cast_register<  ::fwData::Histogram, fwTools::Object>(NULL,NULL);

    ar &  ::boost::serialization::make_nvp( "BinsWidth" , _histogram.getCRefBinsWidth() );
    ar &  ::boost::serialization::make_nvp( "MaxValue" , _histogram.getCRefMaxValue() );
    ar &  ::boost::serialization::make_nvp( "MinValue" , _histogram.getCRefMinValue() );
    ar &  ::boost::serialization::make_nvp( "Values" , _histogram.getCRefValues() );
}

template<class Archive>
void load(Archive & ar, ::fwData::Histogram &_histogram, const unsigned int version)
{
    ::boost::serialization::void_cast_register<  ::fwData::Histogram, fwTools::Object>(NULL,NULL);

    ar &  ::boost::serialization::make_nvp( "BinsWidth" , _histogram.getRefBinsWidth() );
    ar &  ::boost::serialization::make_nvp( "MaxValue" , _histogram.getRefMaxValue() );
    ar &  ::boost::serialization::make_nvp( "MinValue" , _histogram.getRefMinValue() );
    ar &  ::boost::serialization::make_nvp( "Values" , _histogram.getRefValues() );
}





/// serialize histogram via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Histogram & _histogram, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::Histogram, fwTools::Object>(NULL,NULL);

    split_free(ar, _histogram, version); // call load or save depending of archive type
}


}

}

#endif // _HISTOGRAM_BOOST_SERIALISER_HPP



