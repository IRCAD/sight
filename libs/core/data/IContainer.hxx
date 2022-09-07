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

#include "data/Exception.hpp"
#include "data/IContainer.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

namespace sight::data
{

/// Special templated wrapper to std::inserter, std::back_inserter, iterator that can be used in std::copy()
template<class C>
constexpr static auto inserter(C& container)
{
    if constexpr(core::tools::is_container_associative<C>::value)
    {
        return std::inserter(container, container.begin());
    }
    else if constexpr(core::tools::is_container_back_inserter<C>::value)
    {
        return std::back_inserter(container);
    }
    else if constexpr(core::tools::is_container<C>::value)
    {
        // std::array case
        return container.begin();
    }
    else
    {
        static_assert(
            core::tools::is_container<C>::value,
            "sight::data::inserter(): Unsupported container type"
        );

        return container;
    }
}

template<class C>
constexpr IContainer<C>::IContainer() :
    Object(),
    ContainerWrapper<C>()
{
    newSignal<added_signal_t>(s_ADDED_OBJECTS_SIG);
    newSignal<changed_signal_t>(s_CHANGED_OBJECTS_SIG);
    newSignal<removed_signal_t>(s_REMOVED_OBJECTS_SIG);
}

template<class C>
inline IContainer<C>::IContainer(Object::Key /*unused*/) :
    IContainer<C>::IContainer()
{
}

template<class C>
inline IContainer<C>::IContainer(const C& container) :
    IContainer<C>::IContainer()
{
    this->ContainerWrapper<C>::operator=(container);
}

template<class C>
inline IContainer<C>::IContainer(C&& container) :
    IContainer<C>::IContainer()
{
    this->ContainerWrapper<C>::operator=(container);
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool IContainer<C>::operator==(const IContainer& other) const noexcept
{
    if(!core::tools::is_equal(*this, other))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool IContainer<C>::operator!=(const IContainer& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

template<class C>
inline void IContainer<C>::shallowCopy(const Object::csptr& source)
{
    const auto& other = IContainer<C>::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    fieldShallowCopy(source);

    if constexpr(core::tools::is_container_dynamic<C>::value)
    {
        this->IContainer<C>::clear();

        if constexpr(core::tools::is_vector<C>::value || core::tools::is_container_unordered_associative<C>::value)
        {
            this->IContainer<C>::reserve(other->size());
        }
    }

    if constexpr(core::tools::is_container<C>::value)
    {
        std::copy(other->cbegin(), other->cend(), inserter(*this));
    }
}

//------------------------------------------------------------------------------

template<class C>
constexpr bool is_data_object_fn()
{
    if constexpr(core::tools::is_map_like<C>::value)
    {
        if constexpr(core::tools::is_shared_ptr<typename C::mapped_type>::value)
        {
            // "using" is required for MSVC
            using shared_ptr_type = typename C::mapped_type;
            return std::is_base_of<Object, typename shared_ptr_type::element_type>::value;
        }
    }
    else
    {
        if constexpr(core::tools::is_shared_ptr<typename C::value_type>::value)
        {
            // "using" is required for MSVC
            using shared_ptr_type = typename C::value_type;
            return std::is_base_of<Object, typename shared_ptr_type::element_type>::value;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

template<class C>
inline void IContainer<C>::cachedDeepCopy(const Object::csptr& source, Object::DeepCopyCacheType& cache)
{
    const auto& other = IContainer<C>::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    fieldDeepCopy(other, cache);

    // If the container is dynamic, we need to clear it before copying
    if constexpr(core::tools::is_container_dynamic<C>::value)
    {
        // Clear the container
        this->IContainer<C>::clear();

        // Reserve room in the container, if reserve() is available
        if constexpr(core::tools::is_vector<C>::value || core::tools::is_container_unordered_associative<C>::value)
        {
            this->IContainer<C>::reserve(other->size());
        }
    }

    // True if the container contains shared_ptr<Object>
    constexpr bool is_data_object = is_data_object_fn<C>();

    // Special case for map
    if constexpr(core::tools::is_map_like<C>::value)
    {
        // Need to be done this way, because std::pair<key, value>::operator= is deleted..
        for(const auto& [key, value] : *other)
        {
            if constexpr(is_data_object)
            {
                this->IContainer<C>::insert({key, Object::copy(value, cache)});
            }
            else
            {
                this->IContainer<C>::insert({key, value});
            }
        }
    }
    else if constexpr(core::tools::is_container<C>::value)
    {
        if constexpr(is_data_object)
        {
            std::transform(
                other->cbegin(),
                other->cend(),
                inserter(*this),
                [&](const auto& value)
                {
                    return Object::copy(value, cache);
                });
        }
        else
        {
            std::copy(other->cbegin(), other->cend(), inserter(*this));
        }
    }
}

//------------------------------------------------------------------------------

template<class C>
constexpr C IContainer<C>::get_content() const noexcept
{
    C c;
    std::copy(this->IContainer<C>::cbegin(), this->IContainer<C>::cend(), inserter(c));
    return c;
}

//------------------------------------------------------------------------------

template<class C>
[[nodiscard]] constexpr auto IContainer<C>::scoped_emit() const noexcept
{
    return std::make_unique<IContainer<C>::ScopedEmitter>(*this);
}

//------------------------------------------------------------------------------

template<class C>
constexpr IContainer<C>::ScopedEmitter::ScopedEmitter(const IContainer<C>& container) noexcept :
    m_container(container),
    m_backup(container.get_content())
{
}

//------------------------------------------------------------------------------

template<class C>
inline IContainer<C>::ScopedEmitter::~ScopedEmitter() noexcept
{
    emit();
}

//------------------------------------------------------------------------------

template<class C>
constexpr void IContainer<C>::ScopedEmitter::emit() noexcept
{
    IContainer<C>::container_type added;
    IContainer<C>::container_type before;
    IContainer<C>::container_type after;

    // Build the added and the changed lists
    for(const auto& element : m_container)
    {
        if constexpr(core::tools::is_map_like<C>::value)
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

    IContainer<C>::container_type removed;

    // Build the removed lists
    for(const auto& element : m_backup)
    {
        if constexpr(core::tools::is_map_like<C>::value)
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
        auto signal = m_container.template signal<IContainer<C>::added_signal_t>(IContainer<C>::s_ADDED_OBJECTS_SIG);
        signal->asyncEmit(added);
    }

    if(!before.empty() || !after.empty())
    {
        auto signal =
            m_container.template signal<IContainer<C>::changed_signal_t>(IContainer<C>::s_CHANGED_OBJECTS_SIG);
        signal->asyncEmit(before, after);
    }

    if(!removed.empty())
    {
        auto signal =
            m_container.template signal<IContainer<C>::removed_signal_t>(IContainer<C>::s_REMOVED_OBJECTS_SIG);
        signal->asyncEmit(removed);
    }

    reset();
}

//------------------------------------------------------------------------------

template<class C>
constexpr void IContainer<C>::ScopedEmitter::reset() noexcept
{
    m_backup = m_container.get_content();
}

} // namespace sight::data
