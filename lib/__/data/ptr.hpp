/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include <data/generic.hpp>
#include <data/has_data.hpp>
#include <data/mt/shared_ptr.hpp>

#include <optional>
#include <string_view>
#include <system_error>

namespace sight::data
{

template<class DATATYPE, data::access ACCESS>
using ptr_type_traits = std::conditional_t<ACCESS == data::access::out,
                                           data::mt::shared_ptr<typename access_type_traits<DATATYPE, ACCESS>::object>,
                                           data::mt::weak_ptr<typename access_type_traits<DATATYPE, ACCESS>::object> >;

class has_data;

/**
 * @brief Interface class for ptr and ptr_vector.
 *
 * It provides an abstract assignment method, whatever the type and access type of the data.
 */
class SIGHT_DATA_CLASS_API base_ptr
{
public:

    SIGHT_DATA_API base_ptr(
        has_data* _holder,
        std::string_view _key,
        bool _optional,
        access _access,
        std::optional<std::size_t> _index = std::nullopt
    );
    SIGHT_DATA_API virtual ~base_ptr();

    [[nodiscard]] std::string_view key() const;
    [[nodiscard]] std::optional<bool> auto_connect() const;
    [[nodiscard]] bool optional() const;
    [[nodiscard]] enum access access () const;

    // Returns key()
    [[nodiscard]] operator std::string_view() const;

    // Generic getter
    SIGHT_DATA_API virtual sight::data::object::csptr get() = 0;

protected:

    /// Only the owner of the pointer can update the content of the pointer
    friend class has_data;

    /// Internal setter of the pointer
    SIGHT_DATA_API virtual void set(
        const sight::data::object::sptr& _obj,
        std::optional<bool> _auto_connect,
        std::optional<bool> _optional,
        std::optional<std::size_t> _index = std::nullopt,
        bool _signal                      = false
    )                                     = 0;

    SIGHT_DATA_API virtual void set_deferred_id(
        const std::string& _id,
        std::optional<std::size_t> _index = std::nullopt
    )                                     = 0;

    has_data* m_holder {nullptr};
    std::string_view m_key;
    std::optional<bool> m_auto_connect;
    bool m_optional {false};
    enum access m_access
    {
        access::in
    };
};

//------------------------------------------------------------------------------

inline std::string_view base_ptr::key() const
{
    return m_key;
}

//------------------------------------------------------------------------------

inline std::optional<bool> base_ptr::auto_connect() const
{
    return m_auto_connect;
}

//------------------------------------------------------------------------------

inline bool base_ptr::optional() const
{
    return m_optional;
}

//------------------------------------------------------------------------------

inline enum access base_ptr::access() const
{
    return m_access;
}

//------------------------------------------------------------------------------

inline base_ptr::operator std::string_view() const
{
    return m_key;
}

/**
 * @brief This class holds a non-owning ("weak") reference on a data object.
 *
 * This class purpose is to be used as a service class member to declare and access data.
 * It must be converted to a locked_ptr via the lock() function in order to access the referenced object.
 */
template<class DATATYPE, data::access ACCESS = data::access::inout>
class ptr : public ptr_type_traits<DATATYPE,
                                   ACCESS>,
            public base_ptr
{
public:

    using base_ptr_t = ptr_type_traits<DATATYPE, ACCESS>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr(
        has_data* _holder,
        std::string_view _key,
        bool _optional                    = access_type_traits<DATATYPE, ACCESS>::OPTIONAL_DEFAULT,
        std::optional<std::size_t> _index = {}) noexcept :
        base_ptr(_holder, _key, _optional, ACCESS, _index)
    {
    }

    ~ptr() override = default;

    /// Forbids default constructors, destructor and assignment operators
    ptr()                      = delete;
    ptr(const ptr&)            = delete;
    ptr(ptr&&)                 = delete;
    ptr& operator=(const ptr&) = delete;
    ptr& operator=(ptr&&)      = delete;

    /// This method is only available if it is an output
    template<data::access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::VALUE> >
    ptr& operator=(const typename access_type_traits<DATATYPE, ACCESS>::value& _obj)
    {
        this->set(_obj, {}, {}, {}, true);
        return *this;
    }

    /// This method is only available if it is an output
    template<data::access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::VALUE> >
    void reset()
    {
        this->set(nullptr, {}, {}, {}, true);
    }

protected:

    /// Assign the content of the pointer
    void set(
        const sight::data::object::sptr& _obj,
        std::optional<bool> _auto_connect,
        std::optional<bool> _optional,
        std::optional<std::size_t>/*index*/ = std::nullopt,
        bool _signal                        = false
    ) final
    {
        if constexpr(ACCESS == data::access::out)
        {
            if(_signal)
            {
                const auto ptr = this->lock();
                if(ptr)
                {
                    if(!m_deferred_id.empty())
                    {
                        m_holder->notify_unregister_out(ptr.get_shared(), m_deferred_id);
                    }
                    else
                    {
                        SIGHT_WARN("Object id is empty");
                    }
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
            auto typed_obj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->get_classname()
                + "' to '" + target_t::classname() + "'",
                typed_obj
            );
            base_ptr_t::operator=(typed_obj);

            if(_auto_connect.has_value())
            {
                m_auto_connect = _auto_connect.value();
            }

            if(_optional.has_value())
            {
                m_optional = _optional.value();
            }

            if constexpr(ACCESS == data::access::out)
            {
                if(_signal)
                {
                    if(!m_deferred_id.empty())
                    {
                        m_holder->notify_register_out(_obj, m_deferred_id);
                    }
                    else
                    {
                        SIGHT_WARN("Object id is empty");
                    }
                }
            }
        }
    }

private:

    /// Constructor used by ptr_vector, allowing to duplicate the auto_connect status between elements.
    ptr(
        has_data* _holder,
        std::string_view _key,
        bool _optional,
        std::optional<std::size_t> _index,
        std::optional<bool> _auto_connect
    ) noexcept :
        base_ptr(_holder, _key, _optional, ACCESS, _index)
    {
        m_auto_connect = _auto_connect;
    }

    /// Only the owner of the pointer can update the content of the pointer
    friend class has_data;
    template<class, data::access>
    friend class ptr_vector;

    //------------------------------------------------------------------------------

    sight::data::object::csptr get() final
    {
        return std::dynamic_pointer_cast<const data::object>(base_ptr_t::get_shared());
    }

    //------------------------------------------------------------------------------

    void set_deferred_id(const std::string& _id, std::optional<std::size_t> = std::nullopt) final
    {
        SIGHT_ASSERT("Object id can not be empty", !_id.empty());
        m_deferred_id = _id;
    }

    /// Only the owner of the pointer can update the content of the pointer
    friend class has_data;
    template<class, data::access>
    friend class ptr_vector;

    // Pointer on deferred objects (created at runtime) may reference different objects over time
    // To reference the same object amongst different services, we use a specific label
    std::string m_deferred_id;
};

/**
 * @brief This class holds a non-owning ("weak") reference on a collection of data objects.
 *
 * This class purpose is to be used as a service class member to declare and access multiple data of the same type.
 */
template<class DATATYPE, data::access ACCESS>
class ptr_vector final : public base_ptr
{
public:

    using ptr_t           = data::ptr<DATATYPE, ACCESS>;
    using container_ptr_t = std::map<std::size_t, ptr_t*>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr_vector(
        has_data* _holder,
        std::string_view _key,
        bool _optional = access_type_traits<DATATYPE, ACCESS>::OPTIONAL_DEFAULT
    ) noexcept :
        base_ptr(_holder, _key, _optional, ACCESS, {})
    {
    }

    ~ptr_vector() final
    {
        std::for_each(m_ptrs.begin(), m_ptrs.end(), [](const auto& _p){delete _p.second;});
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
    requires(ACCESS == data::access::out)
    {
        if(m_ptrs.find(_index) == m_ptrs.end())
        {
            // Initializes members
            m_ptrs.emplace(std::make_pair(_index, new ptr_t(m_holder, m_key, m_optional, _index, m_auto_connect)));
        }

        return *m_ptrs[_index];
    }

    /// Accessor for individual weak pointers
    const ptr_t& operator[](const std::size_t _index) const
    {
        return *m_ptrs.at(_index);
    }

    /// Returns true if the map of registered pointers is empty
    [[nodiscard]] bool empty() const
    {
        return m_ptrs.empty();
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
    friend class has_data;

    //------------------------------------------------------------------------------

    sight::data::object::csptr get() final
    {
        return nullptr;
    }

    /// Pointer assignment
    void set(
        const sight::data::object::sptr& _obj,
        std::optional<bool> _auto_connect,
        std::optional<bool> _optional,
        std::optional<std::size_t> _index = std::nullopt,
        bool _signal                      = false
    ) final
    {
        auto index = _index.value();
        if(_obj == nullptr)
        {
            m_ptrs[index]->set(nullptr, {}, {}, _signal);
            delete m_ptrs[index];
            m_ptrs.erase(index);
        }
        else
        {
            using target_t = typename access_type_traits<DATATYPE, ACCESS>::object;
            auto typed_obj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->get_classname()
                + "' to '" + target_t::classname() + "'",
                typed_obj
            );

            if(m_ptrs.find(index) == m_ptrs.end())
            {
                const bool optional = _optional.has_value() ? *_optional : this->optional();
                m_ptrs.emplace(std::make_pair(index, new ptr_t(m_holder, m_key, optional, index)));
            }

            m_ptrs[index]->set(_obj, _auto_connect, _optional, _signal);
        }
    }

    //------------------------------------------------------------------------------

    void set_deferred_id(const std::string& _id, std::optional<std::size_t> _index = std::nullopt) final
    {
        SIGHT_ASSERT("Object id can not be empty", !_id.empty());

        if(m_ptrs.find(*_index) == m_ptrs.end())
        {
            m_ptrs.emplace(
                std::make_pair(
                    _index.value(),
                    new ptr_t(m_holder, m_key, m_optional, _index)
                )
            );
        }

        m_ptrs[*_index]->m_deferred_id = _id;
    }

    /// Collection of data, indexed by key
    container_ptr_t m_ptrs;
};

//------------------------------------------------------------------------------

class property_base
{
public:

    virtual sight::data::string_serializable::sptr make_default() = 0;
};
/**
 * @brief This class holds a non-owning ("weak") reference on a data object.
 *
 * This class purpose is to be used as a service class member to declare and access data.
 * It must be converted to a locked_ptr via the lock() function in order to access the referenced object.
 */
template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
class property final : public ptr<DATATYPE,
                                  data::access::inout>,
                       public property_base
{
public:

    /// Constructor that registers the pointer with a default value.
    property(
        has_data* _holder,
        std::string_view _key,
        const DATATYPE& _default_value
    ) noexcept :
        ptr<DATATYPE, data::access::inout>(_holder, _key, true),
        m_default_value(_default_value)
    {
    }

    ~property() override = default;

    //------------------------------------------------------------------------------

    const DATATYPE::value_t& value() const
    {
        const auto prop = this->const_lock();
        return prop->value();
    }

    //------------------------------------------------------------------------------

    const DATATYPE::value_t& operator*() const
    {
        return this->value();
    }

private:

    //------------------------------------------------------------------------------

    sight::data::string_serializable::sptr make_default() override
    {
        auto default_object = std::make_shared<DATATYPE>(m_default_value);
        this->set(default_object, {}, {}, {}, false);
        return default_object;
    }

    const DATATYPE m_default_value;
};

} // namespace sight::data
