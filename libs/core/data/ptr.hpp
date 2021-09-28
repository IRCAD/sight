/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <data/mt/weak_ptr.hpp>
#include <data/Object.hpp>

#include <charconv>
#include <string_view>
#include <system_error>

namespace sight::data
{

//------------------------------------------------------------------------------

enum class Access : std::uint8_t
{
    in     = 0,
    INPUT  = in,
    out    = 1,
    OUTPUT = out,
    inout  = 2,
    INOUT  = inout,
};

//------------------------------------------------------------------------------

// Traits that provides the const or not-const pointer depending on Access
template<class DATATYPE, data::Access access>
struct access_typed_traits;

template<class DATATYPE>
struct access_typed_traits<DATATYPE, data::Access::in>
{
    using value  = CSPTR(DATATYPE);
    using object = const DATATYPE;
    static constexpr bool optional = false;
};

template<class DATATYPE>
struct access_typed_traits<DATATYPE, data::Access::inout>
{
    using value  = SPTR(DATATYPE);
    using object = DATATYPE;
    static constexpr bool optional = false;
};

template<class DATATYPE>
struct access_typed_traits<DATATYPE, data::Access::out>
{
    using value  = SPTR(DATATYPE);
    using object = DATATYPE;
    static constexpr bool optional = true;
};

template<data::Access access>
struct assignable_traits;

template<>
struct assignable_traits<data::Access::in>
{
    static constexpr bool value = false;
};

template<>
struct assignable_traits<data::Access::inout>
{
    static constexpr bool value = false;
};

template<>
struct assignable_traits<data::Access::out>
{
    static constexpr bool value = true;
};

template<data::Access access>
using access_traits = access_typed_traits<sight::data::Object, access>;

class IHasData;

/**
 * @brief Interface class for ptr and ptr_vector.
 *
 * It provides an abstract assignment method, whatever the type and access type of the data.
 */
class base_ptr
{
public:

    base_ptr(IHasData* _holder, std::string_view _key) :
        m_holder(_holder),
        m_key(_key)
    {
    }

    virtual ~base_ptr() = default;

    /// Assignment operator
    DATA_API virtual void set(const sight::data::Object::sptr& _obj, size_t _index = 0) = 0;

protected:

    IHasData* m_holder;
    std::string_view m_key;
};

/**
 * @brief Interface that defines a owner of data::ptr and data::ptr_vector.
 *
 * It allows to register the pointers at instantiation time and set the actual content of the pointer.
 */
class IHasData
{
public:

    IHasData()          = default;
    virtual ~IHasData() = default;

    /**
     * @brief Define an object required by this service.
     *
     * This method defines the required objects to use the service.
     * hasAllRequiredObjects() can then be called to know if the service can be started.
     *
     * @param[in] _key key of the object
     * @param[in] _access access to the object (in or inout)
     * @param[in] _autoConnect if true, the service will be connected to the object's signals
     * @param[in] _optional if true, the service can be started even if the objet is not present
     */
    DATA_API virtual void _registerObject(
        std::string_view _key,
        const Access _access,
        size_t index,
        const bool _autoConnect = false,
        const bool _optional    = false
    )                           = 0;

    /**
     * @brief Define an object group required by this service.
     *
     * This method defines the required objects to use the service.
     * hasAllRequiredObjects() can then be called to know if the service can be started.
     *
     * @param[in] _key key of the object
     * @param[in] _access access to the object (in or inout)
     * @param[in] _minNbObject number of object to register (it is the minimum number of objects required by the
     * service)
     * @param[in] _autoConnect if true, the service will be connected to the object's signals
     * @param[in] _maxNbObject maximum number of object to register (they are defined as optional
     *
     * @note This method will register maxNbObject in the group named (<key>#0, <key>#1, ... <key>#<maxNbObject>). The
     * first Nth objects (minNbObject) are required, the _other are optional.
     */
    DATA_API virtual void _registerObjectGroup(
        std::string_view _key,
        const data::Access _access,
        const std::uint8_t _minNbObject,
        const bool _autoConnect         = false,
        const std::uint8_t _maxNbObject = 10
    )                                   = 0;

    /// Registers a pointer
    void _registerPtr(std::string_view _key, base_ptr* _data, size_t = 0)
    {
        m_dataContainer[_key] = _data;
    }

    DATA_API virtual void _setOutput(std::string_view key, data::Object::sptr object, size_t index = 0) = 0;

protected:

    /// Set the actual content of the pointer
    template<Access A>
    void setPtrObject(std::string_view _key, const typename access_traits<A>::value& _obj, size_t index = 0);

private:

    /// Map of pointers indexed by key name
    std::map<std::string_view, base_ptr*> m_dataContainer;
};

/**
 * @brief This class holds a non-owning ("weak") reference on a data object.
 *
 * This class purpose is to be used as a service class member to declare and access data.
 * It must be converted to a locked_ptr via the lock() function in order to access the referenced object.
 */
template<class DATATYPE, data::Access ACCESS>
class ptr final : public data::mt::weak_ptr<typename access_typed_traits<DATATYPE,
                                                                         ACCESS>::object>,
                  public base_ptr
{
public:

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr(
        IHasData* _holder,
        std::string_view _key,
        bool _autoConnect = false,
        bool _optional    = access_typed_traits<DATATYPE, ACCESS>::optional
    ) noexcept :
        base_ptr(_holder, _key)
    {
        _holder->_registerObject(_key, ACCESS, 0, _autoConnect, _optional);
        _holder->_registerPtr(_key, this);
    }

    /// Forbids default constructors, destructor and assignment operators
    ptr()                      = delete;
    ptr(const ptr&)            = delete;
    ptr(ptr&&)                 = delete;
    ptr& operator=(const ptr&) = delete;
    ptr& operator=(ptr&&)      = delete;

    /// This method is only available if it is an output
    template<data::Access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::value> >
    ptr& operator=(const typename access_typed_traits<DATATYPE, ACCESS>::value& _obj)
    {
        m_holder->_setOutput(m_key, _obj, 0);
        using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
        data::mt::weak_ptr<target_t>::operator=(_obj);
        return *this;
    }

    /// This method is only available if it is an output
    template<data::Access A = ACCESS, typename = typename std::enable_if_t<assignable_traits<A>::value> >
    void reset()
    {
        m_holder->_setOutput(m_key, nullptr, 0);
        using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
        data::mt::weak_ptr<target_t>::reset();
    }

private:

    /// Only the owner of the pointer can update the content of the pointer
    friend class IHasData;

    /// Assign the content of the pointer
    void set(const sight::data::Object::sptr& _obj, size_t = 0) override
    {
        if(_obj == nullptr)
        {
            using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
            data::mt::weak_ptr<target_t>::reset();
        }
        else
        {
            using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
            auto typedObj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->getClassname()
                + "' to '" + target_t::classname() + "'",
                typedObj
            );
            data::mt::weak_ptr<target_t>::operator=(typedObj);
        }
    }
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

    class ptr_t final : public data::mt::weak_ptr<typename access_typed_traits<DATATYPE,
                                                                               ACCESS>::object>
    {
    friend class ptr_vector;

    public:

        ptr_t(
            IHasData* _holder,
            std::string_view _key,
            size_t _index
        ) noexcept :
            m_holder(_holder),
            m_key(_key),
            m_index(_index)
        {
        }

        ptr_t() = default;
        ptr_t(const ptr_t& _other) :
            m_holder(_other.m_holder),
            m_key(_other.m_key),
            m_index(_other.m_index)
        {
        }

        ptr_t(ptr_t&& _other) :
            m_holder(_other.m_holder),
            m_key(_other.m_key),
            m_index(_other.m_index)
        {
        }

        //------------------------------------------------------------------------------

        ptr_t& operator=(const ptr_t& _other)
        {
            m_holder = _other.m_holder;
            m_key    = _other.m_key;
            m_index  = _other.m_index;
        }

        //------------------------------------------------------------------------------

        ptr_t& operator=(ptr_t&& _other)
        {
            m_holder = _other.m_holder;
            m_key    = _other.m_key;
            m_index  = _other.m_index;
        }

        /// This method is only available if it is an output
        template<data::Access A = ACCESS, typename = typename std::enable_if<assignable_traits<A>::value>::type>
        ptr_t& operator=(const typename access_typed_traits<DATATYPE, ACCESS>::value& _obj)
        {
            m_holder->_setOutput(m_key, _obj, m_index);
            return *this;
        }

    private:

        /// Pointer assignment
        void set(typename access_typed_traits<DATATYPE, ACCESS>::value _obj)
        {
            using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
            data::mt::weak_ptr<target_t>::operator=(_obj);
        }

        IHasData* m_holder;
        std::string_view m_key;
        size_t m_index;
    };

    using container_ptr_t = std::map<size_t, ptr_t>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr_vector(
        IHasData* _holder,
        std::string_view _key,
        bool _autoConnect               = false,
        const std::uint8_t _minNbObject = 0
    ) noexcept :
        base_ptr(_holder, _key)
    {
        _holder->_registerObjectGroup(_key, ACCESS, _minNbObject, _autoConnect);
        _holder->_registerPtr(_key, this);
    }

    /// Default constructors, destructor and assignment operators
    ptr_vector()                             = delete;
    ptr_vector(const ptr_vector&)            = delete;
    ptr_vector(ptr_vector&&)                 = delete;
    ptr_vector& operator=(const ptr_vector&) = delete;
    ptr_vector& operator=(ptr_vector&&)      = delete;

    /// Accessor for individual weak pointers
    /// This method is only available if it is an output
    ptr_t& operator[](const size_t _index)
    {
        if(m_ptrs.find(_index) == m_ptrs.end())
        {
            // Initializes members
            m_ptrs.emplace(std::make_pair(_index, ptr_t(m_holder, m_key, _index)));
        }

        return m_ptrs[_index];
    }

    /// Accessor for individual weak pointers
    const ptr_t& operator[](const size_t _index) const
    {
        return m_ptrs[_index];
    }

    /// Return the number of registered pointers
    size_t size() const
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

    /// Pointer assignment
    void set(const sight::data::Object::sptr& _obj, size_t _index = 0) override
    {
        if(_obj == nullptr)
        {
            m_ptrs.erase(_index);
        }
        else
        {
            using target_t = typename access_typed_traits<DATATYPE, ACCESS>::object;
            auto typedObj = std::dynamic_pointer_cast<target_t>(_obj);
            SIGHT_ASSERT(
                "Can not convert pointer type from '" + _obj->getClassname()
                + "' to '" + target_t::classname() + "'",
                typedObj
            );

            if(m_ptrs.find(_index) == m_ptrs.end())
            {
                m_ptrs.emplace(std::make_pair(_index, ptr_t(m_holder, m_key, _index)));
            }

            m_ptrs[_index].set(typedObj);
        }
    }

    /// Collection of data, indexed by key
    container_ptr_t m_ptrs;
};

//------------------------------------------------------------------------------

template<Access A>
inline void IHasData::setPtrObject(
    std::string_view _key,
    const typename access_traits<A>::value& _obj,
    size_t index
)
{
    auto itData = m_dataContainer.find(_key);
    if(itData != m_dataContainer.end())
    {
        itData->second->set(std::const_pointer_cast<sight::data::Object>(_obj), index);
    }
    else
    {
        SIGHT_WARN("Could not find any registered ptr with key '" << _key << "'");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::data
