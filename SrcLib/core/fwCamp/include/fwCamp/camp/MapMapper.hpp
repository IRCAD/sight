/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_CAMP_MAPMAPPER_HPP__
#define __FWCAMP_CAMP_MAPMAPPER_HPP__

#include <boost/utility/enable_if.hpp>
#include "fwCamp/camp/traits.hpp"

namespace camp_ext
{

template <typename T, typename C = void>
struct MapMapper
{
};

template <typename MAP>
struct MapMapper< MAP , typename boost::enable_if_c<camp::isMapping<MAP>::value>::type >
{
   typedef MAP MapType;
   typedef typename MapType::value_type ValueType;
   typedef typename MapType::key_type KeyType;
   typedef typename MapType::mapped_type MappedType;


   static std::size_t size(const MapType& map)
   {
       return map.size();
   }



   static const ValueType& get(const MapType& map, std::size_t index)
   {
       typename MapType::const_iterator it = map.begin();
       std::advance(it, index);
       return *it;
   }

   static void set(MapType& map, const KeyType & key, const MappedType& value)
   {
       map[key] = value;
   }
};

}  // namespace camp_ext




#endif /* __FWCAMP_CAMP_MAPMAPPER_HPP__ */
