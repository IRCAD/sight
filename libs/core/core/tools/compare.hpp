/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <type_traits>

namespace sight::core::tools
{

/**
 * @brief This header defines "is_equal" comparison helper functions, especially usefull for floating point comparison
 *
 * It also defines some template helpers to allow identify a type as a container, as a shared pointer, ...
 *
 * @section Template Template helpers
 *
 * @subsection is_container is_container
 *
 * Allows to identify a type as a container that can be iterated.
 *
 * @code{.cpp}

   template<typename T, typename std::enable_if_t<is_container<T>::value>* = nullptr>
   bool is_equal(const T& a, const T& b) {...}

 * @endcode
 *
 * @subsection is_map is_map
 *
 * Allows to identify a type as a map that can be iterated and value accessed using a key.
 *
 * @code{.cpp}

   template<typename T, typename std::enable_if_t<is_map<T>::value>* = nullptr>
   bool is_equal(const T& a, const T& b) {...}

 * @endcode
 *
 * @subsection is_shared_ptr is_shared_ptr, is_unique_ptr, is_weak_ptr
 *
 * Allows to identify a type as a pointer (std::shared_ptr, std::unique_ptr, std::weak_ptr) that can be dereferenced
 * using '*' operator.
 *
 * @code{.cpp}

   template<typename T, typename std::enable_if_t<is_shared_ptr<T>::value>* = nullptr>
   bool is_equal(const T& a, const T& b) {...}

 * @endcode
 *
 * @section is_equal is_equal
 *
 * @subsection is_equal_real is_equal(real, real)
 *
 * Compares two real values using a scaled epsilon which is similar as ULP method.
 * Near zero, NaN and inf cases are also specifically managed.
 * @code{.cpp}

   double a = 1.0;
   double b = 1.0;

   if(is_equal(a, b)) {...}

 * @endcode
 *
 * @subsection is_equal_pointer is_equal(pointer, pointer)
 *
 * Compares two pointed values by dereferencing pointers. It first checks if the two pointers are the same, then
 * dereferences them and call is_equal() again. Weak_ptr are first converted to shared_ptr before comparison.
 *
 * @code{.cpp}

   auto a = std::make_unique<double>(1.0);
   auto b = std::make_shared<double>(1.0);

   if(is_equal(a, b)) {...}

 * @endcode
 *
 * @subsection is_equal_container is_equal(container, container)
 *
 * Compares contained values by iterating over elements. If the container have the same size, it calls is_equal() on
 * each elements.
 *
 * @code{.cpp}

   std::vector<double> a = {1.0, 2.0, 3.0};
   std::vector<double> b = {1.0, 2.0, 3.0};

   if(is_equal(a, b)) {...}

 * @endcode
 */

/// Allows to check if a type is a container.
template<typename ... Ts>
struct is_helper {};

template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
    T,
    std::conditional_t<
        false,
        is_helper<
            typename T::value_type,
            typename T::size_type,
            typename T::iterator,
            typename T::const_iterator,
            decltype(std::declval<T>().size()),
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend())
        >,
        void
    >
>: public std::true_type {};

/// Allows to check if a type is a map.
template<typename T, typename _ = void>
struct is_map : std::false_type {};

template<typename T>
struct is_map<
    T,
    std::conditional_t<
        false,
        is_helper<
            typename T::value_type,
            typename T::size_type,
            typename T::key_type,
            typename T::mapped_type,
            typename T::iterator,
            typename T::const_iterator,
            decltype(std::declval<T>().size()),
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend()),
            decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])
        >,
        void
    >
>: public std::true_type {};

/// Allows to check if a type is a shared_ptr
template<class T>
struct is_shared_ptr : std::false_type {};

template<class T>
struct is_shared_ptr<std::shared_ptr<T> >: std::true_type {};

/// Allows to check if a type is a unique_ptr
template<class T>
struct is_unique_ptr : std::false_type {};

template<class T>
struct is_unique_ptr<std::unique_ptr<T> >: std::true_type {};

/// Allows to check if a type is a weak_ptr
template<class T>
struct is_weak_ptr : std::false_type {};

template<class T>
struct is_weak_ptr<std::weak_ptr<T> >: std::true_type {};

/// This is a floating point comparison helper. It mainly use a scaled epsilon with some fallback.
/// @param a left floating point number to compare
/// @param b right floating point number to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<std::is_floating_point_v<T1>&& std::is_floating_point_v<T2> >* = nullptr
>
constexpr static bool is_equal(T1 a, T2 b)
{
    if constexpr(std::is_same_v<T1, T2>)
    {
        // Special corner cases: NaN, Inf, ...
        if(a == b)
        {
            // If direct compare is true, then it is certainly equal. Should also take care of Inf, ...
            return true;
        }
        else if(std::isinf(a) || std::isinf(b))
        {
            // If one of them is infinite, then they are certainly not equal.
            return false;
        }
        else if(const bool a_nan = std::isnan(a), b_nan = std::isnan(b); a_nan || b_nan)
        {
            // Normally, NaN == NaN returns false. We want the opposite
            return a_nan && b_nan;
        }
        else if(const T1 abs_diff = std::abs(a - b); abs_diff <= std::numeric_limits<T1>::epsilon())
        {
            // This manage the case where we are near zero
            return true;
        }
        else
        {
            // Otherwise, use a scaled epsilon
            return abs_diff <= std::numeric_limits<T1>::epsilon() * std::max(std::abs(a), std::abs(b));
        }
    }
    else if constexpr(std::is_same_v<T1, float>|| std::is_same_v<T2, float>)
    {
        // In case one is float and the other is double or long double
        return is_equal(static_cast<float>(a), static_cast<float>(b));
    }
    else if constexpr(std::is_same_v<T1, double>|| std::is_same_v<T2, double>)
    {
        // In case one is double and the other is long double
        return is_equal(static_cast<double>(a), static_cast<double>(b));
    }
}

/// This is a mixed comparison helper. It cast the integral type to floating point or use == operator as needed
/// @param a left number to compare
/// @param b right number to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<
        !is_container<T1>::value
        && !is_container<T2>::value
        && !is_map<T1>::value
        && !is_map<T2>::value
        && !std::is_pointer_v<T1>
        && !is_shared_ptr<T1>::value
        && !is_unique_ptr<T1>::value
        && !is_weak_ptr<T1>::value
        && !std::is_pointer_v<T2>
        && !is_shared_ptr<T2>::value
        && !is_unique_ptr<T2>::value
        && !is_weak_ptr<T2>::value
        && (!std::is_floating_point_v<T1>|| !std::is_floating_point_v<T2>)
    >* = nullptr
>
constexpr static bool is_equal(const T1& a, const T2& b)
{
    if constexpr(std::is_integral_v<T1>&& std::is_floating_point_v<T2>)
    {
        return is_equal(static_cast<T2>(a), b);
    }
    else if constexpr(std::is_floating_point_v<T1>&& std::is_integral_v<T2>)
    {
        return is_equal(a, static_cast<T1>(b));
    }
    else
    {
        return a == b;
    }
}

/// This is a weak pointer comparison helper. It automatically dereferences the pointer and compare the values
/// @param a left weak pointer to compare
/// @param b right weak pointer to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<is_weak_ptr<T1>::value || is_weak_ptr<T2>::value>* = nullptr
>
constexpr static bool is_equal(T1 a, T2 b)
{
    // Manage weak_ptr cases
    if constexpr(is_weak_ptr<T1>::value)
    {
        return is_equal(a.lock(), b);
    }
    else if constexpr(is_weak_ptr<T2>::value)
    {
        return is_equal(a, b.lock());
    }
    else
    {
        // Should not be reached
        const auto a_ptr = a.lock();
        const auto b_ptr = b.lock();
        return a == b || is_equal(*a_ptr, *b_ptr);
    }
}

/// This is a pointer comparison helper. It automatically dereferences the pointer and compare the values
/// @param a left pointer to compare
/// @param b right pointer to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<(
                                  std::is_pointer_v<T1>
                                  || is_shared_ptr<T1>::value
                                  || is_unique_ptr<T1>::value
                              ) && (
                                  std::is_pointer_v<T2>
                                  || is_shared_ptr<T2>::value
                                  || is_unique_ptr<T2>::value
                              )>* = nullptr
>
constexpr static bool is_equal(T1 a, T2 b)
{
    // c++ forbids to compare pointers to different types
    if constexpr(std::is_same_v<T1, T2>)
    {
        // Simple fast pointer comparison,
        if(a == b)
        {
            return true;
        }
    }
    else if(a == nullptr && b == nullptr)
    {
        return true;
    }

    if(a == nullptr || b == nullptr)
    {
        return false;
    }

    // Dereferenced comparison
    return is_equal(*a, *b);
}

/// This is a comparison helper for container. It uses internally is_equal() for each element.
/// @param a left container to compare
/// @param b right container to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<
        is_container<T1>::value
        && !is_map<T1>::value
        && is_container<T2>::value
        && !is_map<T2>::value
    >* = nullptr
>
constexpr static bool is_equal(const T1& a, const T2& b)
{
    if(a.size() != b.size())
    {
        return false;
    }

    auto a_it = a.cbegin();
    auto b_it = b.cbegin();

    const auto a_end = a.cend();

    while(a_it != a_end)
    {
        if(!is_equal(*a_it, *b_it))
        {
            return false;
        }

        ++a_it;
        ++b_it;
    }

    return true;
}

/// This is a comparison helper for map. It uses internally is_equal() for each element.
/// @param a left map to compare
/// @param b right map to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<is_map<T1>::value && is_map<T2>::value>* = nullptr
>
constexpr static bool is_equal(const T1& a, const T2& b)
{
    if(a.size() != b.size())
    {
        return false;
    }

    const auto b_end = b.cend();

    for(const auto& [a_key, a_value] : a)
    {
        if(const auto& b_it = b.find(a_key); b_it == b_end)
        {
            return false;
        }
        else if(!is_equal(a_value, b_it->second))
        {
            return false;
        }
    }

    return true;
}

/// This is a floating point less than comparison helper. It mainly use a scaled epsilon with some fallback.
/// This is not perfetc, as no perfect solution exists
/// @param a left floating point number to compare
/// @param b right floating point number to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<std::is_floating_point_v<T1>&& std::is_floating_point_v<T2> >* = nullptr
>
constexpr static bool is_less(T1 a, T2 b)
{
    if constexpr(std::is_same_v<T1, T2>)
    {
        // Special corner cases: NaN, Inf, ...
        if(std::isnan(a) || std::isnan(b))
        {
            // If one of them is NaN, assume the comparison is always false.
            return false;
        }
        else if(std::isinf(a) || std::isinf(b))
        {
            // We assume std::isless returns true for -Inf < Inf and false for -Inf < -Inf
            return std::isless(a, b);
        }
        else if(const T1 diff = a - b; diff < std::numeric_limits<T1>::epsilon())
        {
            // This manage the case where we are near zero
            return true;
        }
        else
        {
            // Otherwise, use a scaled epsilon
            return diff < std::numeric_limits<T1>::epsilon() * std::max(std::abs(a), std::abs(b));
        }
    }
    else if constexpr(std::is_same_v<T1, float>|| std::is_same_v<T2, float>)
    {
        // In case one is float and the other is double or long double
        return is_less(static_cast<float>(a), static_cast<float>(b));
    }
    else if constexpr(std::is_same_v<T1, double>|| std::is_same_v<T2, double>)
    {
        // In case one is double and the other is long double
        return is_less(static_cast<double>(a), static_cast<double>(b));
    }
}

/// This is a floating point greater than comparison helper. It mainly use a scaled epsilon with some fallback.
/// This is not perfetc, as no perfect solution exists
/// @param a left floating point number to compare
/// @param b right floating point number to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<std::is_floating_point_v<T1>&& std::is_floating_point_v<T2> >* = nullptr
>
constexpr static bool is_greater(T1 a, T2 b)
{
    if constexpr(std::is_same_v<T1, T2>)
    {
        // Special corner cases: NaN, Inf, ...
        if(std::isnan(a) || std::isnan(b))
        {
            // If one of them is NaN, assume the comparison is always false.
            return false;
        }
        else if(std::isinf(a) || std::isinf(b))
        {
            // We assume std::isgreater returns true for Inf > -Inf and false for Inf > Inf
            return std::isgreater(a, b);
        }
        else if(const T1 diff = a - b; diff > std::numeric_limits<T1>::epsilon())
        {
            // This manage the case where we are near zero
            return true;
        }
        else
        {
            // Otherwise, use a scaled epsilon
            return diff > std::numeric_limits<T1>::epsilon() * std::max(std::abs(a), std::abs(b));
        }
    }
    else if constexpr(std::is_same_v<T1, float>|| std::is_same_v<T2, float>)
    {
        // In case one is float and the other is double or long double
        return is_greater(static_cast<float>(a), static_cast<float>(b));
    }
    else if constexpr(std::is_same_v<T1, double>|| std::is_same_v<T2, double>)
    {
        // In case one is double and the other is long double
        return is_greater(static_cast<double>(a), static_cast<double>(b));
    }
}

} // namespace sight::core::tools
