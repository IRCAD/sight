/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/** 
 * @file fwCore/concept_checks.hpp
 * @brief This file defines some fw4spl concept checks.
 *
 * @author IRCAD (Research and Development Team).
 */
#ifndef __FWCORE_CONCEPT_CHECK_HPP
#define __FWCORE_CONCEPT_CHECK_HPP

#include <boost/concept_check.hpp>

namespace fwCore
{
namespace concepts
{



template <class X>
struct SharedPtrTypedef
{
 public:

    BOOST_CONCEPT_USAGE(SharedPtrTypedef)
    {
        typename X::sptr::element_type *a;
        X *b;
        same_type(a,b);
    }
    
 private:

    // Type deduction will fail unless the arguments have the same type.
    template <typename T>
    void same_type(T *, T *);
};


} // namespace concepts

} // namespace fwCore

#endif //__FWCORE_CONCEPT_CHECK_HPP
