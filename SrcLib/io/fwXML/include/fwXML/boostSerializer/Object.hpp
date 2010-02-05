#ifndef _OBJECT_BOOST_SERIALISER_HPP
#define _OBJECT_BOOST_SERIALISER_HPP

#include <fwData/Object.hpp>



namespace boost {
namespace serialization {

/**
 * @brief serialize object using boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, fwTools::Object &_object, const unsigned int version)
{
    // intrinsic data to be saved must be here
}



} }

INSTANTIATE_SERIALIZER(fwTools::Object);

#endif // _OBJECT_BOOST_SERIALISER_HPP
