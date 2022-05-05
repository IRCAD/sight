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
 * @subsection is_map is_map, is_multimap, is_unordered_map, is_unordered_multimap
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

// cspell:ignore crbegin crend

/// Template matching utilities
template<typename ... Ts>
struct is_helper {};

/// Allows to check if a type is a container.
/// @see https://en.cppreference.com/w/cpp/container
template<typename T, typename = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
    T,
    std::conditional_t<
        false,
        is_helper<
            typename T::value_type,
            typename T::reference,
            typename T::const_reference,
            typename T::iterator,
            typename T::const_iterator,
            typename T::size_type,
            typename T::difference_type,
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend()),
            decltype(std::declval<T>().size()),
            decltype(std::declval<T>().max_size()),
            decltype(std::declval<T>().empty()),

            // Also found in most containers
            typename T::pointer,
            typename T::const_pointer
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is an ordered container.
template<typename T, typename = void, typename = void>
struct is_container_ordered : std::false_type {};

template<typename T>
struct is_container_ordered<
    T,
    std::enable_if_t<is_container<T>::value>,
    std::conditional_t<
        false,
        is_helper<
            typename T::reverse_iterator,
            typename T::const_reverse_iterator,
            decltype(std::declval<T>().rbegin()),
            decltype(std::declval<T>().rend()),
            decltype(std::declval<T>().crbegin()),
            decltype(std::declval<T>().crend())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a dynamic container (ie not an array).
template<typename T, typename = void, typename = void>
struct is_container_dynamic : std::false_type {};

template<typename T>
struct is_container_dynamic<
    T,
    std::enable_if_t<is_container<T>::value>,
    std::conditional_t<
        false,
        is_helper<
            typename T::allocator_type,
            decltype(std::declval<T>().get_allocator()),
            decltype(std::declval<T>().clear())
        >,
        void
    >
>: std::true_type {};

template<typename T, typename = void, typename = void>
struct is_container_sequenced : std::false_type {};

template<typename T>
struct is_container_sequenced<
    T,
    std::enable_if_t<is_container_ordered<T>::value>,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().front()),
            decltype(std::declval<T>().back())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::array
template<typename T, typename = void, typename = void>
struct is_array : std::false_type {};

template<typename T>
struct is_array<
    T,
    std::enable_if_t<
        is_container_sequenced<T>::value
        && !is_container_dynamic<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().data())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type have push_back() method.
template<typename T, typename = void, typename = void>
struct is_container_back_inserter : std::false_type {};

template<typename T>
struct is_container_back_inserter<
    T,
    std::enable_if_t<
        is_container_sequenced<T>::value
        && is_container_dynamic<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().pop_back())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of boost::multi_index
template<typename T, typename = void, typename = void>
struct is_sequenced_set : std::false_type {};

template<typename T>
struct is_sequenced_set<
    T,
    std::enable_if_t<is_container_back_inserter<T>::value>,
    std::conditional_t<
        false,
        is_helper<
            typename T::ctor_args,
            typename T::tag_list,
            typename T::node_type,
            typename T::insert_return_type,
            decltype(std::declval<T>().capacity()),
            decltype(std::declval<T>().shrink_to_fit()),
            decltype(std::declval<T>().pop_front()),
            decltype(std::declval<T>().pop_back()),
            decltype(std::declval<T>().unique()),
            decltype(std::declval<T>().sort()),
            decltype(std::declval<T>().reverse())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::vector
template<typename T, typename = void, typename = void>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<
    T,
    std::enable_if_t<
        is_container_back_inserter<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().capacity()),
            decltype(std::declval<T>().shrink_to_fit()),
            decltype(std::declval<T>().data())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::deque
template<typename T, typename = void, typename = void>
struct is_deque : std::false_type {};

template<typename T>
struct is_deque<
    T,
    std::enable_if_t<
        is_container_back_inserter<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().shrink_to_fit()),
            decltype(std::declval<T>().pop_front())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::list
template<typename T, typename = void, typename = void>
struct is_list : std::false_type {};

template<typename T>
struct is_list<
    T,
    std::enable_if_t<
        is_container_back_inserter<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            decltype(std::declval<T>().pop_front()),
            decltype(std::declval<T>().unique()),
            decltype(std::declval<T>().sort()),
            decltype(std::declval<T>().reverse())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of ordered associative container
template<typename T, typename = void, typename = void>
struct is_container_ordered_associative : std::false_type {};

template<typename T>
struct is_container_ordered_associative<
    T,
    std::enable_if_t<
        is_container_ordered<T>::value
        && is_container_dynamic<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::key_type,
            typename T::key_compare,
            typename T::node_type,
            decltype(std::declval<T>().key_comp()),
            decltype(std::declval<T>().value_comp())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::map
template<typename T, typename = void, typename = void>
struct is_map : std::false_type {};

template<typename T>
struct is_map<
    T,
    std::enable_if_t<
        is_container_ordered_associative<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::insert_return_type,
            typename T::mapped_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::multimap
template<typename T, typename = void, typename = void>
struct is_multimap : std::false_type {};

template<typename T>
struct is_multimap<
    T,
    std::enable_if_t<
        is_container_ordered_associative<T>::value
        && !is_map<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::mapped_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::set
template<typename T, typename = void, typename = void>
struct is_set : std::false_type {};

template<typename T>
struct is_set<
    T,
    std::enable_if_t<
        is_container_ordered_associative<T>::value
        && !is_map<T>::value
        && !is_multimap<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::insert_return_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::multiset
template<typename T>
struct is_multiset : std::conditional_t<
                         is_container_ordered_associative<T>::value
                         && !is_set<T>::value
                         && !is_map<T>::value
                         && !is_multimap<T>::value
                         && !is_sequenced_set<T>::value,
                         std::true_type,
                         std::false_type
    > {};

/// Allows to check if a type is a kind of unordered associative container
template<typename T, typename = void, typename = void>
struct is_container_unordered_associative : std::false_type {};

template<typename T>
struct is_container_unordered_associative<
    T,
    std::enable_if_t<
        is_container<T>::value
        && is_container_dynamic<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::key_type,
            typename T::hasher,
            typename T::key_equal,
            typename T::local_iterator,
            typename T::const_local_iterator,
            typename T::node_type,
            decltype(std::declval<T>().hash_function()),
            decltype(std::declval<T>().key_eq()),
            decltype(std::declval<T>().bucket_count()),
            decltype(std::declval<T>().max_bucket_count()),
            decltype(std::declval<T>().load_factor()),
            decltype(std::declval<T>().max_load_factor())
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind od std::unordered_map
template<typename T, typename = void, typename = void>
struct is_unordered_map : std::false_type {};

template<typename T>
struct is_unordered_map<
    T,
    std::enable_if_t<
        is_container_unordered_associative<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::mapped_type,
            typename T::insert_return_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind od std::unordered_multimap
template<typename T, typename = void, typename = void>
struct is_unordered_multimap : std::false_type {};

template<typename T>
struct is_unordered_multimap<
    T,
    std::enable_if_t<
        is_container_unordered_associative<T>::value
        && !is_unordered_map<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::mapped_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind od std::unordered_set
template<typename T, typename = void, typename = void>
struct is_unordered_set : std::false_type {};

template<typename T>
struct is_unordered_set<
    T,
    std::enable_if_t<
        is_container_unordered_associative<T>::value
        && !is_unordered_map<T>::value
        && !is_unordered_multimap<T>::value
        && !is_sequenced_set<T>::value
    >,
    std::conditional_t<
        false,
        is_helper<
            typename T::insert_return_type
        >,
        void
    >
>: std::true_type {};

/// Allows to check if a type is a kind of std::unordered_multiset
template<typename T>
struct is_unordered_multiset : std::conditional_t<
                                   is_container_unordered_associative<T>::value
                                   && !is_unordered_set<T>::value
                                   && !is_unordered_map<T>::value
                                   && !is_unordered_multimap<T>::value
                                   && !is_sequenced_set<T>::value,
                                   std::true_type,
                                   std::false_type
    > {};

/// Helpers to know if a type is a kind of map (value_type is a pair)
template<typename T>
struct is_map_like : std::conditional_t<
                         (
                             is_map<T>::value
                             || is_multimap<T>::value
                             || is_unordered_map<T>::value
                             || is_unordered_multimap<T>::value
                         )
                         && !is_sequenced_set<T>::value,
                         std::true_type,
                         std::false_type
    > {};

/// Helpers to know if a type is a kind of set (value_type is a pair)
template<typename T>
struct is_set_like : std::conditional_t<
                         (
                             is_set<T>::value
                             || is_multiset<T>::value
                             || is_unordered_set<T>::value
                             || is_unordered_multiset<T>::value
                         )
                         && !is_sequenced_set<T>::value,
                         std::true_type,
                         std::false_type
    > {};

/// Helpers to know if a type is a kind of associative container
template<typename T>
struct is_container_associative : std::conditional_t<
                                      (
                                          is_container_ordered_associative<T>::value
                                          || is_container_unordered_associative<T>::value
                                      )
                                      && !is_sequenced_set<T>::value,
                                      std::true_type,
                                      std::false_type
    > {};

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

/// Allows to check if a type can be dereferenced.
template<typename T, typename = void>
struct is_dereferenceable : std::false_type {};

template<typename T>
struct is_dereferenceable<
    T,
    std::conditional_t<
        false,
        is_helper<
            decltype(*(std::declval<T&>()))
        >,
        void
    >
>: std::true_type
{
    // Deduce the value type such that const is propagated
    using value_type = std::remove_reference_t<decltype(*std::declval<T&>())>;
};

/// Allows to check if a type is a pair
template<class T, typename = void>
struct is_pair : std::false_type {};

template<class T>
struct is_pair<
    T,
    std::conditional_t<
        false,
        is_helper<
            typename T::first_type,
            typename T::second_type,
            decltype(std::declval<T>().first),
            decltype(std::declval<T>().second)
        >,
        void
    >
>: std::true_type {};

/// This is a floating point comparison helper. It mainly use a scaled epsilon with some fallback.
/// @param a left floating point number to compare
/// @param b right floating point number to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value>* = nullptr
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
        !is_pair<T1>::value
        && !is_pair<T2>::value
        && !is_container<T1>::value
        && !is_container<T2>::value
        && !is_dereferenceable<T1>::value
        && !is_weak_ptr<T1>::value
        && !std::is_pointer_v<T2>
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
    typename std::enable_if_t<is_pair<T1>::value && is_pair<T2>::value>* = nullptr
>
constexpr static bool is_equal(T1 a, T2 b)
{
    return is_equal(a.first, b.first) && is_equal(a.second, b.second);
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
    typename std::enable_if_t<is_dereferenceable<T1>::value && is_dereferenceable<T2>::value>* = nullptr
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
    typename std::enable_if_t<is_container_ordered<T1>::value && is_container_ordered<T2>::value>* = nullptr
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
        if constexpr(is_map_like<T1>::value && is_map_like<T2>::value)
        {
            if(!is_equal(a_it->first, b_it->first) || !is_equal(a_it->second, b_it->second))
            {
                return false;
            }
        }
        else
        {
            if(!is_equal(*a_it, *b_it))
            {
                return false;
            }
        }

        ++a_it;
        ++b_it;
    }

    return true;
}

/// This is a comparison helper for unordered container. It may be a bit inefficient due to the unordered nature.
/// @param a left map to compare
/// @param b right map to compare
template<
    typename T1,
    typename T2,
    typename std::enable_if_t<
        is_container_unordered_associative<T1>::value
        && is_container_unordered_associative<T2>::value
        && ((is_map_like<T1>::value && is_map_like<T2>::value)
            || (is_set_like<T1>::value && is_set_like<T2>::value))
    >* = nullptr
>
constexpr static bool is_equal(const T1& a, const T2& b)
{
    if(a.size() != b.size())
    {
        return false;
    }

    const auto b_end = b.cend();

    // Map case
    if constexpr(is_map_like<T1>::value && is_map_like<T2>::value)
    {
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
    }
    // Set case
    else if constexpr(is_set_like<T1>::value && is_set_like<T2>::value)
    {
        for(const auto& a_value : a)
        {
            if(const auto& b_it = b.find(a_value); b_it == b_end)
            {
                return false;
            }
            else if(!is_equal(a_value, *b_it))
            {
                return false;
            }
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
