#ifndef __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__
#define __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__

#include <camp/valuemapper.hpp>
#include "fwCamp/camp/traits.hpp"

namespace camp_ext
{

template <typename T>
struct ValueMapper<T, typename boost::enable_if_c<camp::isMapping<T>::value>::type>
{
    static const int type = camp::mappingType;

};

}  // namespace camp_ext




#endif /* __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__ */
