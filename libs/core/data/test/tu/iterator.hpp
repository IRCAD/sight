/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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
#include <core/macros.hpp>

#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>

template<typename ... Ts>
class zip_iterator
{
using indices = std::index_sequence_for<Ts ...>;

public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using iterator_type     = std::tuple<Ts ...>;
    using value_type        = std::tuple<typename std::iterator_traits<Ts>::value_type ...>;
    using pointer           = value_type*;
    using reference         = value_type&;

    constexpr FINLINE zip_iterator(std::tuple<Ts ...>&& begin_iterator_tuple, std::tuple<Ts ...>&& end_iterator_tuple) :
        iterator_tuple_{std::forward<std::tuple<Ts ...> >(begin_iterator_tuple)},
        end_iterator_tuple_{std::forward<std::tuple<Ts ...> >(end_iterator_tuple)},
        value_tuple_{make(indices())}
    {
    }

    constexpr FINLINE zip_iterator(std::tuple<Ts ...>&& end_iterator_tuple) :
        iterator_tuple_{std::forward<std::tuple<Ts ...> >(end_iterator_tuple)}
    {
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE auto operator*() noexcept
    {
        return value_tuple_;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE auto operator->() noexcept
    {
        return &value_tuple_;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE zip_iterator& operator++() noexcept
    {
        next(indices());
        value_tuple_ = make(indices());
        return *this;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE zip_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE bool operator!=(zip_iterator const& rhs) const noexcept
    {
        return iterator_tuple_ != rhs.iterator_tuple_;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE bool operator==(zip_iterator const& rhs) const noexcept
    {
        return iterator_tuple_ == rhs.iterator_tuple_;
    }

private:

    //------------------------------------------------------------------------------

    template<std::size_t ... Is>
    constexpr FINLINE auto make(std::index_sequence<Is ...>) noexcept
    {
        return std::make_tuple(
            (
                std::get<Is>(iterator_tuple_) == std::get<Is>(end_iterator_tuple_)
                ? typename std::iterator_traits<decltype(std::prev(std::get<Is>(iterator_tuple_)))>::value_type {}
                : *std::get<Is>(iterator_tuple_)
            ) ...
        );
    }

    //------------------------------------------------------------------------------

    template<std::size_t ... Is>
    constexpr FINLINE void next(std::index_sequence<Is ...>) noexcept
    {
        ((
             std::get<Is>(iterator_tuple_) == std::get<Is>(end_iterator_tuple_)
             ? std::get<Is>(iterator_tuple_)
             : ++std::get<Is>(iterator_tuple_)
         ), ...);
    }

private:

    iterator_type iterator_tuple_;
    iterator_type end_iterator_tuple_;
    value_type value_tuple_;
};

//------------------------------------------------------------------------------

template<typename ... Ts>
zip_iterator<Ts ...> make_zip_iterator(std::tuple<Ts ...>&& end_tuple)
{
    return zip_iterator<Ts ...> {std::forward<std::tuple<Ts ...> >(end_tuple)};
}

//------------------------------------------------------------------------------

template<typename ... Ts>
zip_iterator<Ts ...> make_zip_iterator(std::tuple<Ts ...>&& begin_tuple, std::tuple<Ts ...>&& end_tuple)
{
    return zip_iterator<Ts ...> {
        std::forward<std::tuple<Ts ...> >(begin_tuple),
        std::forward<std::tuple<Ts ...> >(end_tuple)
    };
}

template<typename ... Range>
class zip_longest_range
{
using indices = std::index_sequence_for<Range ...>;

public:

    explicit constexpr FINLINE zip_longest_range(Range&& ... ranges) :
        ranges_{std::forward<Range>(ranges) ...},
        size_{max_size(indices())}
    {
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE auto begin() const noexcept
    {
        return make_zip_iterator(begin_tuple(indices()), end_tuple(indices()));
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE auto end() const noexcept
    {
        return make_zip_iterator(end_tuple(indices()));
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE std::size_t size() const noexcept
    {
        return size_;
    }

    //------------------------------------------------------------------------------

    constexpr FINLINE bool empty() const noexcept
    {
        return 0 == size();
    }

private:

    //------------------------------------------------------------------------------

    template<std::size_t ... Is>
    FINLINE auto begin_tuple(std::index_sequence<Is ...>) const noexcept
    {
        return std::make_tuple(std::begin(std::get<Is>(ranges_)) ...);
    }

    //------------------------------------------------------------------------------

    template<std::size_t ... Is>
    FINLINE auto end_tuple(std::index_sequence<Is ...>) const noexcept
    {
        return std::make_tuple(std::end(std::get<Is>(ranges_)) ...);
    }

    //------------------------------------------------------------------------------

    template<std::size_t ... Is>
    constexpr FINLINE std::size_t max_size(std::index_sequence<Is ...>) const noexcept
    {
        if constexpr(0 == sizeof...(Range))
        {
            return 0;
        }
        else
        {
            return static_cast<std::size_t>(std::max({size(std::get<Is>(ranges_)) ...}));
        }
    }

    //------------------------------------------------------------------------------

    template<typename T>
    constexpr FINLINE ptrdiff_t size(T&& range) const noexcept
    {
        return std::distance(std::begin(range), std::end(range));
    }

private:

    std::tuple<Range ...> ranges_;
    std::size_t size_;
};

//------------------------------------------------------------------------------

template<typename ... Range>
constexpr FINLINE zip_longest_range<Range ...> zip_longest(Range&& ... ranges)
{
    return zip_longest_range<Range ...>(
        std::forward<Range>(ranges) ...
    );
}
