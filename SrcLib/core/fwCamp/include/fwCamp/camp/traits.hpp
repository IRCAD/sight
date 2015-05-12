/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
