/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/exception.hpp"
#include "data/generic.hpp"

#include <core/compound_types.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::data
{

/**
 * @brief   This generic class contains an array of scalar values.
 */
template<typename T, std::size_t N>
requires std::is_arithmetic_v<T>
class SIGHT_DATA_CLASS_API vec : public generic<std::array<T,
                                                           N> >
{
public:

    using array_t = std::array<T, N>;

    using generic<array_t>::generic;
    using generic<array_t>::operator=;
    using generic<array_t>::operator[];

    vec()
    {
        std::ranges::fill(this->value(), static_cast<T>(0));
    }

    vec(std::initializer_list<T> _data)
    {
        SIGHT_ASSERT(
            "vec constructor needs " + std::to_string(N) + " elements: " + std::to_string(_data.size()) + " given.",
            _data.size() == N
        );
        std::ranges::copy(_data, this->value().begin());
    }

    //------------------------------------------------------------------------------
    ~vec() noexcept override = default;

    //------------------------------------------------------------------------------

    std::string to_string() const override
    {
        std::vector<std::string> s;
        std::ranges::transform(this->value(), std::back_inserter(s), [](auto& _s){return std::to_string(_s);});
        return boost::join(s, ";");
    }

    //------------------------------------------------------------------------------

    void from_string(const std::string& _value) override
    {
        std::vector<std::string> tokens;
        boost::split(tokens, _value, boost::is_any_of(";"));
        SIGHT_THROW_EXCEPTION_IF(
            data::exception("Unexpected number of arguments in string initializer"),
            tokens.size() != N
        );
        std::ranges::transform(
            tokens,
            this->value().begin(),
            [](const std::string& _value)
            {
                return boost::lexical_cast<T>(boost::trim_copy(_value));
            });
    }
};

#define DECLARE_VEC(_class, _T, _N) \
        class SIGHT_DATA_CLASS_API _class final : public sight::data::vec<_T, \
                                                                          _N> \
        { \
        public: \
            SIGHT_DECLARE_CLASS(_class, data::string_serializable); \
            using sight::data::vec<_T, _N>::vec; \
            using sight::data::vec<_T, _N>::operator=; \
        };

} // namespace sight::data
