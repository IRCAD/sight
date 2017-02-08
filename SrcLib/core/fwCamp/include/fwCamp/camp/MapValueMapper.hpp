/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__
#define __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__

#include "fwCamp/camp/customtype.hpp"
#include "fwCamp/camp/traits.hpp"

#include <camp/valuemapper.hpp>

namespace camp_ext
{

template <typename T>
struct ValueMapper<T, typename boost::enable_if_c<camp::isMapping<T>::value>::type>
{
    static const int type = camp::mappingType;
};

}  // namespace camp_ext

#endif /* __FWCAMP_CAMP_MAPVALUEMAPPER_HPP__ */
