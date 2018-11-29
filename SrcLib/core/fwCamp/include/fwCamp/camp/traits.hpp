/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWCAMP_CAMP_TRAITS_HPP__
#define __FWCAMP_CAMP_TRAITS_HPP__

#include <camp/detail/yesnotype.hpp>

namespace camp
{


template <typename T>
struct isMapping
{
    template <typename U> static camp::detail::TypeYes check(typename U::mapped_type*);
    template <typename U> static camp::detail::TypeNo  check(...);

    //fwData hack to avoid composite passing this test
    template <typename U> static camp::detail::TypeNo  checkSptr(typename U::sptr*);
    template <typename U> static camp::detail::TypeYes checkSptr(...);

    enum {value = (
              sizeof(check< T >(0)) == sizeof(camp::detail::TypeYes)
              && sizeof(checkSptr< T >(0)) == sizeof(camp::detail::TypeYes)
              )  };
};


}// namespace camp




#endif /* __FWCAMP_CAMP_TRAITS_HPP__ */
