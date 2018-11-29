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

/**
 * @file fwCore/concept_checks.hpp
 * @brief This file defines some sight concept checks.
 *
 *
 */
#ifndef __FWCORE_CONCEPT_CHECKS_HPP__
#define __FWCORE_CONCEPT_CHECKS_HPP__

#include <boost/concept_check.hpp>

#include <fwCore/macros.hpp>

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
            typename X::sptr::element_type *a = 0;
            X *b = 0;
            FwCoreNotUsedMacro(a);
            FwCoreNotUsedMacro(b);
            same_type(a,b);
        }

    private:

        // Type deduction will fail unless the arguments have the same type.
        template <typename T>
        void same_type(T *, T *);
};


} // namespace concepts

} // namespace fwCore

#endif //__FWCORE_CONCEPT_CHECKS_HPP__
