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

#include "data/container.hpp"
#include "data/exception.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

namespace sight::data
{

/// Special templated wrapper to std::inserter, std::back_inserter, iterator that can be used in std::copy()
template<class C>
constexpr static auto inserter(C& _container)
{
    if constexpr(core::is_container_associative<C>::value)
    {
        return std::inserter(_container, _container.begin());
    }
    else if constexpr(core::is_container_back_inserter<C>::value)
    {
        return std::back_inserter(_container);
    }
    else if constexpr(core::is_container<C>::value)
    {
        // std::array case
        return _container.begin();
    }
    else
    {
        static_assert(
            core::is_container<C>::value,
            "sight::data::inserter(): Unsupported container type"
        );

        return _container;
    }
}

template<class C>
constexpr container<C>::container() :
    object(),
    container_wrapper<C>()
{
    new_signal<added_signal_t>(ADDED_OBJECTS_SIG);
    new_signal<changed_signal_t>(CHANGED_OBJECTS_SIG);
    new_signal<removed_signal_t>(REMOVED_OBJECTS_SIG);
}

template<class C>
inline container<C>::container(const C& _container) :
    container<C>::container()
{
    this->container_wrapper<C>::operator=(_container);
}

template<class C>
inline container<C>::container(C&& _container) :
    container<C>::container()
{
    this->container_wrapper<C>::operator=(_container);
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool container<C>::operator==(const container& _other) const noexcept
{
    if(!core::is_equal(*this, _other))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool container<C>::operator!=(const container& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

template<class C>
inline void container<C>::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const container<C> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    if constexpr(core::is_container_dynamic<C>::value)
    {
        this->container<C>::clear();

        if constexpr(core::is_vector<C>::value || core::is_container_unordered_associative<C>::value)
        {
            this->container<C>::reserve(other->size());
        }
    }

    if constexpr(core::is_container<C>::value)
    {
        std::copy(other->cbegin(), other->cend(), inserter(*this));
    }

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool is_data_object_fn()
{
    if constexpr(core::is_map_like<C>::value)
    {
        if constexpr(core::is_shared_ptr<typename C::mapped_type>::value)
        {
            // "using" is required for MSVC
            using shared_ptr_type = typename C::mapped_type;
            return std::is_base_of<object, typename shared_ptr_type::element_type>::value;
        }
    }
    else
    {
        if constexpr(core::is_shared_ptr<typename C::value_type>::value)
        {
            // "using" is required for MSVC
            using shared_ptr_type = typename C::value_type;
            return std::is_base_of<object, typename shared_ptr_type::element_type>::value;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

template<class C>
inline void container<C>::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const container<C> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    // If the container is dynamic, we need to clear it before copying
    if constexpr(core::is_container_dynamic<C>::value)
    {
        // Clear the container
        this->container<C>::clear();

        // Reserve room in the container, if reserve() is available
        if constexpr(core::is_vector<C>::value || core::is_container_unordered_associative<C>::value)
        {
            this->container<C>::reserve(other->size());
        }
    }

    // True if the container contains shared_ptr<object>
    constexpr bool is_data_object = is_data_object_fn<C>();

    // Special case for map
    if constexpr(core::is_map_like<C>::value)
    {
        // Need to be done this way, because std::pair<key, value>::operator= is deleted..
        for(const auto& [key, value] : *other)
        {
            if constexpr(is_data_object)
            {
                this->container<C>::insert({key, object::copy(value, _cache)});
            }
            else
            {
                this->container<C>::insert({key, value});
            }
        }
    }
    else if constexpr(core::is_container<C>::value)
    {
        if constexpr(is_data_object)
        {
            std::transform(
                other->cbegin(),
                other->cend(),
                inserter(*this),
                [&](const auto& _value)
                {
                    return object::copy(_value, _cache);
                });
        }
        else
        {
            std::copy(other->cbegin(), other->cend(), inserter(*this));
        }
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

template<class C>
constexpr C container<C>::get_content() const noexcept
{
    C c;
    std::copy(this->container<C>::cbegin(), this->container<C>::cend(), inserter(c));
    return c;
}

//------------------------------------------------------------------------------

template<class C>
[[nodiscard]] constexpr auto container<C>::scoped_emit() const noexcept
{
    return std::make_unique<container<C>::scoped_emitter>(*this);
}

//------------------------------------------------------------------------------

template<class C>
constexpr container<C>::scoped_emitter::scoped_emitter(const container<C>& _container) noexcept :
    m_container(_container),
    m_backup(_container.get_content())
{
}

//------------------------------------------------------------------------------

template<class C>
inline container<C>::scoped_emitter::~scoped_emitter() noexcept
{
    emit();
}

//------------------------------------------------------------------------------

template<class C>
constexpr void container<C>::scoped_emitter::block(const core::com::slot_base::sptr& _slot) noexcept
{
    m_blocked_slots.push_back(_slot);
}

//------------------------------------------------------------------------------

template<class C>
void container<C>::scoped_emitter::emit() noexcept
{
    container<C>::container_t added;
    container<C>::container_t before;
    container<C>::container_t after;

    // Build the added and the changed lists
    for(const auto& element : m_container)
    {
        if constexpr(core::is_map_like<C>::value)
        {
            // If we cannot find the key in the backup, it means it was added
            if(const auto& it = m_backup.find(element.first); it == m_backup.cend())
            {
                added.insert(element);
            }
            // If we can find the key, but the element is not the same, it means it was changed
            else if(element.second != it->second)
            {
                before.insert(*it);
                after.insert(element);
            }
        }
        else
        {
            // If we cannot find the element in the backup, it means it was added
            if(std::find(m_backup.cbegin(), m_backup.cend(), element) == m_backup.cend())
            {
                added.insert(added.cend(), element);
            }
        }
    }

    container<C>::container_t removed;

    // Build the removed lists
    for(const auto& element : m_backup)
    {
        if constexpr(core::is_map_like<C>::value)
        {
            // If we cannot find the key in the container, it means it was removed
            if(m_container.find(element.first) == m_container.cend())
            {
                removed.insert(element);
            }
        }
        else
        {
            // If we cannot find the element in the container, it means it was removed
            if(std::find(m_container.cbegin(), m_container.cend(), element) == m_container.cend())
            {
                removed.insert(removed.cend(), element);
            }
        }
    }

    // Send the notifications
    if(!added.empty())
    {
        auto signal = m_container.template signal<container<C>::added_signal_t>(container<C>::ADDED_OBJECTS_SIG);
        std::vector<core::com::connection::blocker> blockers;
        for(auto& slot : m_blocked_slots)
        {
            blockers.emplace_back(core::com::connection::blocker(signal->get_connection(slot)));
        }

        signal->async_emit(added);
    }

    if(!before.empty() || !after.empty())
    {
        auto signal =
            m_container.template signal<container<C>::changed_signal_t>(container<C>::CHANGED_OBJECTS_SIG);

        std::vector<core::com::connection::blocker> blockers;
        for(auto& slot : m_blocked_slots)
        {
            blockers.emplace_back(core::com::connection::blocker(signal->get_connection(slot)));
        }

        signal->async_emit(before, after);
    }

    if(!removed.empty())
    {
        auto signal =
            m_container.template signal<container<C>::removed_signal_t>(container<C>::REMOVED_OBJECTS_SIG);

        std::vector<core::com::connection::blocker> blockers;
        for(auto& slot : m_blocked_slots)
        {
            blockers.emplace_back(core::com::connection::blocker(signal->get_connection(slot)));
        }

        signal->async_emit(removed);
    }

    reset();
}

//------------------------------------------------------------------------------

template<class C>
constexpr void container<C>::scoped_emitter::reset() noexcept
{
    m_backup = m_container.get_content();
}

} // namespace sight::data
