/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
 * @file core/concept_checks.hpp
 * @brief This file defines some sight concept checks.
 *
 *
 */

#pragma once

#include <core/macros.hpp>

#include <boost/concept_check.hpp>

namespace sight::core
{

namespace concepts
{

template<class X>
struct SharedPtrTypedef
{
    public:

        BOOST_CONCEPT_USAGE(SharedPtrTypedef)
        {
            typename X::sptr::element_type* a = 0;
            X* b                              = 0;
            SIGHT_NOT_USED(a);
            SIGHT_NOT_USED(b);
            same_type(a, b);
        }

    private:

        // Type deduction will fail unless the arguments have the same type.
        template<typename T>
        void same_type(T*, T*);
};

} // namespace concepts

} // namespace sight::core
