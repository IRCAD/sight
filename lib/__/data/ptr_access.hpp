/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#pragma once

#include <cstdint>

namespace sight::data
{

//------------------------------------------------------------------------------

enum class access : std::uint8_t
{
    in    = 0,
    out   = 1,
    inout = 2
};

//------------------------------------------------------------------------------

// Traits that provides the const or not-const pointer depending on access
template<class DATATYPE, data::access access>
struct access_type_traits;

template<class DATATYPE>
struct access_type_traits<DATATYPE, data::access::in>
{
    using value  = CSPTR(DATATYPE);
    using object = const DATATYPE;
    static constexpr bool OPTIONAL_DEFAULT = false;
};

template<class DATATYPE>
struct access_type_traits<DATATYPE, data::access::inout>
{
    using value  = SPTR(DATATYPE);
    using object = DATATYPE;
    static constexpr bool OPTIONAL_DEFAULT = false;
};

template<class DATATYPE>
struct access_type_traits<DATATYPE, data::access::out>
{
    using value  = SPTR(DATATYPE);
    using object = DATATYPE;
    static constexpr bool OPTIONAL_DEFAULT = true;
};

template<data::access access>
struct assignable_traits;

template<>
struct assignable_traits<data::access::in>
{
    static constexpr bool VALUE = false;
};

template<>
struct assignable_traits<data::access::inout>
{
    static constexpr bool VALUE = false;
};

template<>
struct assignable_traits<data::access::out>
{
    static constexpr bool VALUE = true;
};

template<data::access access>
using access_traits = access_type_traits<sight::data::object, access>;

} // namespace sight::data
