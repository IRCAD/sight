/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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
#include "data/object.hpp"

#include <core/com/signal.hpp>
#include <core/compare.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>

namespace sight::data
{

// cspell:ignore crbegin crend

/// sequenced_set alias. It provides sequence like API, while keeping the unicity of the elements.
/// @note Remove me, once there is a STL equivalent.
template<class C>
using sequenced_set = boost::multi_index::multi_index_container<
    C,
    boost::multi_index::indexed_by<
        boost::multi_index::random_access<>,
        boost::multi_index::hashed_unique<
            boost::multi_index::identity<C>
        >
    >
>;

#define USING_CONTAINER(C) \
    using typename C::value_type; \
    using typename C::reference; \
    using typename C::const_reference; \
    using typename C::iterator; \
    using typename C::const_iterator; \
    using typename C::size_type; \
    using typename C::difference_type; \
    using C::operator=; \
    using C::begin; \
    using C::end; \
    using C::cbegin; \
    using C::cend; \
    using C::size; \
    using C::max_size; \
    using C::empty; \
    using C::swap; \
    using typename C::pointer; \
    using typename C::const_pointer

#define USING_ORDERED_CONTAINER(C) \
    using typename C::reverse_iterator; \
    using typename C::const_reverse_iterator; \
    using C::rbegin; \
    using C::rend; \
    using C::crbegin; \
    using C::crend

#define USING_DYNAMIC_CONTAINER(C) \
    using typename C::allocator_type; \
    using C::get_allocator; \
    using C::clear; \
    using C::insert; \
    using C::emplace; \
    using C::erase

#define USING_SEQUENCE_CONTAINER(C) \
    using C::assign; \
    using C::resize; \
    using C::front; \
    using C::back; \
    using C::push_back; \
    using C::pop_back

#define USING_ASSOCIATIVE_CONTAINER(C) \
    using typename C::key_type; \
    using typename C::node_type; \
    using C::emplace_hint; \
    using C::merge; \
    using C::extract; \
    using C::count; \
    using C::find; \
    using C::equal_range

#define USING_ASSOCIATIVE_ORDERED_CONTAINER(C) \
    using typename C::key_compare; \
    using typename C::value_compare; \
    using C::key_comp; \
    using C::value_comp; \
    using C::lower_bound; \
    using C::upper_bound

#define USING_UNORDERED_ASSOCIATIVE_CONTAINER(C) \
    using typename C::hasher; \
    using typename C::key_equal; \
    using typename C::local_iterator; \
    using typename C::const_local_iterator; \
    using C::hash_function; \
    using C::key_eq; \
    using C::bucket_count; \
    using C::max_bucket_count; \
    using C::bucket_size; \
    using C::bucket; \
    using C::load_factor; \
    using C::max_load_factor; \
    using C::rehash; \
    using C::reserve

#define USING_MAP_CONTAINER(C) \
    using typename C::mapped_type; \
    using C::operator[]; \
    using C::at; \
    using C::try_emplace; \
    using C::insert_or_assign; \
    using typename C::insert_return_type

/// Dummy class.
template<class C, typename = void>
class container_wrapper
{
};

/// Array container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_array<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other array named requirements
    /// @{
    using C::fill;
    using C::operator[];
    using C::at;
    using C::data;
    using C::front;
    using C::back;
    /// @}

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Vector container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_vector<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other sequence named requirements
    USING_SEQUENCE_CONTAINER(C);

    /// Other vector named requirements
    /// @{
    using C::capacity;
    using C::reserve;
    using C::shrink_to_fit;
    using C::operator[];
    using C::at;
    using C::data;
    using C::emplace_back;
    /// @}

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;

    /// Utility function to remove all matching elements from the container.
    constexpr auto remove(const typename C::value_type& _value)
    {
        return C::erase(std::remove(C::begin(), C::end(), _value), C::end());
    }

    /// Utility function to remove first matching elements from the container.
    constexpr auto remove_one(const typename C::value_type& _value)
    {
        if(const auto& it = std::find(C::cbegin(), C::cend(), _value); it != C::cend())
        {
            return C::erase(it);
        }

        return C::end();
    }
};

/// Deque container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_deque<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other sequence named requirements
    USING_SEQUENCE_CONTAINER(C);

    /// Other deque named requirements
    /// @{
    using C::shrink_to_fit;
    using C::operator[];
    using C::at;
    using C::emplace_front;
    using C::emplace_back;
    using C::push_front;
    using C::pop_front;
    /// @}

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// List container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_list<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other sequence named requirements
    USING_SEQUENCE_CONTAINER(C);

    /// Other list named requirements
    /// @{
    using C::push_front;
    using C::pop_front;
    using C::splice;
    using C::remove;
    using C::unique;
    using C::merge;
    using C::sort;
    using C::reverse;
    /// @}

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Sequenced set container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_sequenced_set<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other sequence named requirements
    USING_SEQUENCE_CONTAINER(C);

    /// Other sequenced_set named requirements
    /// @{
    using typename C::ctor_args;
    using typename C::tag_list;
    using typename C::node_type;
    using typename C::insert_return_type;
    using C::iterator_to;
    using C::capacity;
    using C::reserve;
    using C::shrink_to_fit;
    using C::operator[];
    using C::at;
    using C::emplace_front;
    using C::push_front;
    using C::pop_front;
    using C::emplace_back;
    using C::extract;
    using C::replace;
    using C::modify;
    using C::splice;
    using C::remove;
    using C::remove_if;
    using C::unique;
    using C::merge;
    using C::sort;
    using C::reverse;
    using C::rearrange;
    using C::get;
    /// @}

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;

    /// Utility function to remove first matching elements from the container.
    constexpr auto remove(const typename C::value_type& _value)
    {
        if(const auto& it = std::find(C::cbegin(), C::cend(), _value); it != C::cend())
        {
            return C::erase(it);
        }
        else
        {
            return C::end();
        }
    }
};

/// Map container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_map<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other associative ordered requirements
    USING_ASSOCIATIVE_ORDERED_CONTAINER(C);

    /// Other map named requirements
    USING_MAP_CONTAINER(C);

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Multimap container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_multimap<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other associative ordered requirements
    USING_ASSOCIATIVE_ORDERED_CONTAINER(C);

    /// Other map named requirements
    using typename C::mapped_type;

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Set container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_set<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other associative ordered requirements
    USING_ASSOCIATIVE_ORDERED_CONTAINER(C);

    /// Other set named requirements
    using typename C::insert_return_type;

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Set container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_multiset<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other ordered named requirements
    USING_ORDERED_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other associative ordered requirements
    USING_ASSOCIATIVE_ORDERED_CONTAINER(C);

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Unordered map container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C, typename std::enable_if_t<core::is_unordered_map<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other unordered associative named requirements
    USING_UNORDERED_ASSOCIATIVE_CONTAINER(C);

    /// Other map named requirements
    USING_MAP_CONTAINER(C);

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Unordered multimap container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_unordered_multimap<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other unordered associative named requirements
    USING_UNORDERED_ASSOCIATIVE_CONTAINER(C);

    /// Missing map named requirements
    using typename C::mapped_type;

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Unordered set container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C, typename std::enable_if_t<core::is_unordered_set<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other unordered associative named requirements
    USING_UNORDERED_ASSOCIATIVE_CONTAINER(C);

    /// Missing set named requirements
    using typename C::insert_return_type;

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

/// Unordered set container class.
template<class C>
class SIGHT_DATA_CLASS_API container_wrapper<
        C,
        typename std::enable_if_t<core::is_unordered_multiset<C>::value>
    >: private C
{
public:

    /// STL container named requirements
    USING_CONTAINER(C);

    /// Other dynamic named requirements
    USING_DYNAMIC_CONTAINER(C);

    /// Other associative named requirements
    USING_ASSOCIATIVE_CONTAINER(C);

    /// Other unordered associative named requirements
    USING_UNORDERED_ASSOCIATIVE_CONTAINER(C);

    /// Enable all constructors from the base class...
    using C::C;

    /// Default virtual destructor
    /// @note: constexpr destructors are only available with ‘-std=c++20’ or ‘-std=gnu++20’
    inline virtual ~container_wrapper() noexcept = default;
};

#undef USING_CONTAINER
#undef USING_ORDERED_CONTAINER
#undef USING_DYNAMIC_CONTAINER
#undef USING_SEQUENCE_CONTAINER
#undef USING_LIST_CONTAINER
#undef USING_ASSOCIATIVE_CONTAINER
#undef USING_ASSOCIATIVE_ORDERED_CONTAINER
#undef USING_UNORDERED_ASSOCIATIVE_CONTAINER
#undef USING_MAP_CONTAINER

/// Generic Sight data container class.
template<class C>
class SIGHT_DATA_CLASS_API container : public object,
                                       public container_wrapper<C>
{
public:

    SIGHT_DECLARE_CLASS(container<C>, object);

    /// Could be usefull to keep the underlying container
    using container_t = C;

    /// Constructors / Destructor / Assignment operators
    /// @{
    constexpr container();
    inline explicit container(const C& _container);
    inline explicit container(C&& _container);
    inline ~container() noexcept override = default;

    /// To allow assignment from STL containers
    using container_wrapper<C>::container_wrapper;
    using container_wrapper<C>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    constexpr bool operator==(const container& _other) const noexcept;
    constexpr bool operator!=(const container& _other) const noexcept;
    /// @}

    /// Signals
    /// @{
    /// Type of signal when objects are added
    using added_signal_t = core::com::signal<void (container_t)>;
    inline static const core::com::signals::key_t ADDED_OBJECTS_SIG = "added_objects";

    /// Type of signal when objects are changed (newObjects, oldObjects)
    using changed_signal_t = core::com::signal<void (container_t, container_t)>;
    inline static const core::com::signals::key_t CHANGED_OBJECTS_SIG = "changedObjects";

    /// Type of signal when objects are removed
    using removed_signal_t = core::com::signal<void (container_t)>;
    inline static const core::com::signals::key_t REMOVED_OBJECTS_SIG = "removed_objects";
    /// @}

    /// Returns a copy of the underlying container
    constexpr C get_content() const noexcept;

    struct SIGHT_DATA_CLASS_API scoped_emitter
    {
        constexpr scoped_emitter(const container& _container) noexcept;
        inline ~scoped_emitter() noexcept;

        /// Emits the needed signals
        void emit() noexcept;

        /// Resets the internal state
        constexpr void reset() noexcept;

        /// Resets the internal state
        constexpr void block(const core::com::slot_base::sptr& _slot) noexcept;

        const container& m_container;
        container::container_t m_backup;
        std::vector<core::com::slot_base::sptr> m_blocked_slots;
    };

    [[nodiscard]] constexpr auto scoped_emit() const noexcept;

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    inline void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    inline void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;
};

} // namespace sight::data

/// This will also includes the templated definitions, which is required for generated registerServices.cpp or Qt MOC
#include "data/container.hxx"
