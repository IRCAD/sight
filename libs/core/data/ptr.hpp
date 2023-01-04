/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "data/config.hpp"

#include <data/IHasData.hpp>
#include <data/mt/shared_ptr.hpp>

#include <optional>
#include <string_view>
#include <system_error>

namespace sight::data
{

template<class DATATYPE, data::Access ACCESS>
using ptr_type_traits = std::conditional_t<ACCESS == data::Access::out,
                                           data::mt::shared_ptr<typename access_type_traits<DATATYPE, ACCESS>::object>,
                                           data::mt::weak_ptr<typename access_type_traits<DATATYPE, ACCESS>::object> >;

class IHasData;

/**
 * @brief Interface class for ptr and ptr_vector.
 *
 * It provides an abstract assignment method, whatever the type and access type of the data.
 */
class DATA_CLASS_API base_ptr
{
public:

    DATA_API base_ptr(
        IHasData* _holder,
        std::string_view _key,
        bool _autoConnect,
        bool _optional,
        Access _access,
        std::optional<std::size_t> _index = std::nullopt
    );
    DATA_API virtual ~base_ptr();

    [[nodiscard]] std::string_view key() const;
    [[nodiscard]] bool autoConnect() const;
    [[nodiscard]] bool optional() const;
    [[nodiscard]] Access access() const;

    // Generic getter
    DATA_API virtual sight::data::Object::csptr get() = 0;

protected:

    /// Only the owner of the pointer can update the content of the pointer
    friend class IHasData;

    /// Internal setter of the pointer
    DATA_API virtual void set(
        const sight::data::Object::sptr& _obj,
        std::optional<bool> autoConnect,
        std::optional<bool> optional,
        std::optional<std::size_t> index = std::nullopt,
        bool signal                      = false
    )                                    = 0;

    DATA_API virtual void setDeferredId(const std::string& _id, std::optional<std::size_t> index = std::nullopt) = 0;

    IHasData* m_holder {nullptr};
    std::string_view m_key;
    bool m_autoConnect {false};
    bool m_optional {false};
    Access m_access {Access::in};
};

//------------------------------------------------------------------------------

inline std::string_view base_ptr::key() const
{
    return m_key;
}

//------------------------------------------------------------------------------

inline bool base_ptr::autoConnect() const
{
    return m_autoConnect;
}

//------------------------------------------------------------------------------

inline bool base_ptr::optional() const
{
    return m_optional;
}

//------------------------------------------------------------------------------

inline Access base_ptr::access() const
{
    return m_access;
}

/**
 * @brief This class holds a non-owning ("weak") reference on a data object.
 *
 * This class purpose is to be used as a service class member to declare and access data.
 * It must be converted to a locked_ptr via the lock() function in order to access the referenced object.
 */
template<class DATATYPE, data::Access ACCESS>
class ptr final : public ptr_type_traits<DATATYPE,
                                         ACCESS>,
                  public base_ptr
{
public:

    using base_ptr_t = ptr_type_traits<DATATYPE, ACCESS>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr(
        IHasData* _holder,
        std::string_view _key,
        bool _autoConnect                 = false,
        bool _optional                    = access_type_traits<DATATYPE, ACCESS>::optional,
        std::optional<std::size_t> _index = {}) noexcept :
        base_ptr(_holder, _key, _autoConnect, _optional, ACCESS, _index)
    {
    }

    ~ptr() final = default;

    /// Forbids default constructors, destructor and assignment operators
    ptr()                      = delete;
    ptr(const ptr&)            = delete;
    ptr(ptr&&)                 = delete;
    ptr& operator=(const ptr&) = delete;
    ptr& operator=(ptr&&)      = delete;

    /// This method is only available if it is an output
    template<data::Access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::value> >
    ptr& operator=(const typename access_type_traits<DATATYPE, ACCESS>::value& _obj)
    {
        this->set(_obj, {}, {}, {}, true);
        return *this;
    }

    /// This method is only available if it is an output
    template<data::Access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::value> >
    void reset()
    {
        this->set(nullptr, {}, {}, {}, true);
    }

private:

    /// Only the owner of the pointer can update the content of the pointer
    friend class IHasData;
    template<class, data::Access>
    friend class ptr_vector;

    //------------------------------------------------------------------------------

    sight::data::Object::csptr get() final
    {
        return std::dynamic_pointer_cast<const data::Object>(base_ptr_t::get_shared());
    }

    /// Assign the content of the pointer
    void set(
        const sight::data::Object::sptr& _obj,
        std::optional<bool> autoConnect,
        std::optional<bool> optional,
        std::optional<std::size_t> /*index*/ = std::nullopt,
        bool signal                          = false
    ) final
    {
        if constexpr(ACCESS == data::Access::out)
        {
            if(signal)
            {
                const auto ptr = this->lock();
                if(ptr)
                {
                    m_holder->notifyUnregisterOut(ptr.get_shared(), m_deferredId);
                }
            }
        }

        if(_obj == nullptr)
        {
            base_ptr_t::reset();
        }
        else
        {
            using target_t = typename access_type_traits<DATATYPE, ACCESS>::object;
            auto typedObj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->getClassname()
                + "' to '" + target_t::classname() + "'",
                typedObj
            );
            base_ptr_t::operator=(typedObj);

            if(autoConnect.has_value())
            {
                m_autoConnect = autoConnect.value();
            }

            if(optional.has_value())
            {
                m_optional = optional.value();
            }

            if constexpr(ACCESS == data::Access::out)
            {
                if(signal)
                {
                    m_holder->notifyRegisterOut(_obj, m_deferredId);
                }
            }
        }
    }

    //------------------------------------------------------------------------------

    void setDeferredId(const std::string& _id, std::optional<std::size_t> = std::nullopt) final
    {
        m_deferredId = _id;
    }

    // Pointer on deferred objects (created at runtime) may reference different objects over time
    // To reference the same object amongst different services, we use a specific label
    std::string m_deferredId;
};

/**
 * @brief This class holds a non-owning ("weak") reference on a collection of data objects.
 *
 * This class purpose is to be used as a service class member to declare and access multiple data of the same type.
 */
template<class DATATYPE, data::Access ACCESS>
class ptr_vector final : public base_ptr
{
public:

    using ptr_t           = data::ptr<DATATYPE, ACCESS>;
    using container_ptr_t = std::map<std::size_t, ptr_t*>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr_vector(
        IHasData* _holder,
        std::string_view _key,
        bool _autoConnect = false,
        bool _optional    = access_type_traits<DATATYPE, ACCESS>::optional
    ) noexcept :
        base_ptr(_holder, _key, _autoConnect, _optional, ACCESS, {})
    {
    }

    ~ptr_vector() final
    {
        std::for_each(m_ptrs.begin(), m_ptrs.end(), [](const auto& p){delete p.second;});
    }

    /// Default constructors, destructor and assignment operators
    ptr_vector()                             = delete;
    ptr_vector(const ptr_vector&)            = delete;
    ptr_vector(ptr_vector&&)                 = delete;
    ptr_vector& operator=(const ptr_vector&) = delete;
    ptr_vector& operator=(ptr_vector&&)      = delete;

    /// Accessor for individual weak pointers
    /// This method is only available if it is an output
    ptr_t& operator[](const std::size_t _index)
    {
        if(m_ptrs.find(_index) == m_ptrs.end())
        {
            // Initializes members
            m_ptrs.emplace(std::make_pair(_index, new ptr_t(m_holder, m_key, m_autoConnect, m_optional, _index)));
        }

        return *m_ptrs[_index];
    }

    /// Accessor for individual weak pointers
    const ptr_t& operator[](const std::size_t _index) const
    {
        return m_ptrs[_index];
    }

    /// Return the number of registered pointers
    [[nodiscard]] std::size_t size() const
    {
        return m_ptrs.size();
    }

    //------------------------------------------------------------------------------

    typename container_ptr_t::iterator begin()
    {
        return m_ptrs.begin();
    }

    //------------------------------------------------------------------------------

    typename container_ptr_t::iterator end()
    {
        return m_ptrs.end();
    }

    //------------------------------------------------------------------------------

    typename container_ptr_t::const_iterator cbegin()
    {
        return m_ptrs.cbegin();
    }

    //------------------------------------------------------------------------------

    typename container_ptr_t::const_iterator cend()
    {
        return m_ptrs.cend();
    }

private:

    /// Only the owner of the pointer can update the content of the pointer
    friend class IHasData;

    //------------------------------------------------------------------------------

    sight::data::Object::csptr get() final
    {
        return nullptr;
    }

    /// Pointer assignment
    void set(
        const sight::data::Object::sptr& _obj,
        std::optional<bool> autoConnect,
        std::optional<bool> optional,
        std::optional<std::size_t> _index = std::nullopt,
        bool signal                       = false
    ) final
    {
        auto index = _index.value();
        if(_obj == nullptr)
        {
            m_ptrs[index]->set(nullptr, {}, {}, signal);
            delete m_ptrs[index];
            m_ptrs.erase(index);
        }
        else
        {
            using target_t = typename access_type_traits<DATATYPE, ACCESS>::object;
            auto typedObj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->getClassname()
                + "' to '" + target_t::classname() + "'",
                typedObj
            );

            if(m_ptrs.find(index) == m_ptrs.end())
            {
                m_ptrs.emplace(std::make_pair(index, new ptr_t(m_holder, m_key, *autoConnect, *optional, index)));
            }

            m_ptrs[index]->set(_obj, autoConnect, optional, signal);
        }
    }

    //------------------------------------------------------------------------------

    void setDeferredId(const std::string& _id, std::optional<std::size_t> index = std::nullopt) final
    {
        if(m_ptrs.find(*index) == m_ptrs.end())
        {
            m_ptrs.emplace(std::make_pair(index.value(), new ptr_t(m_holder, m_key, m_autoConnect, m_optional, index)));
        }

        m_ptrs[*index]->m_deferredId = _id;
    }

    /// Collection of data, indexed by key
    container_ptr_t m_ptrs;
};

//------------------------------------------------------------------------------

} // namespace sight::data
