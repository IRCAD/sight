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
using access_traits = access_typed_traits<sight::data::Object, access>;

/**
 * @brief Interface class for ptr and ptr_vector.
 *
 * It provides an abstract assignment method, whatever the type and access type of the data.
 */
class base_ptr
{
public:

    virtual ~base_ptr() = default;

    /// Assignment operator
    DATA_API virtual void set(const sight::data::Object::sptr& _obj, size_t _index = 0) = 0;
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
        const std::string& _key,
        const Access _access,
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
     * first Nth objects (minNbObject) are required, the other are optional.
     */
    DATA_API virtual void _registerObjectGroup(
        const std::string& _key,
        const data::Access _access,
        const std::uint8_t _minNbObject,
        const bool _autoConnect         = false,
        const std::uint8_t _maxNbObject = 10
    )                                   = 0;

    /// Registers a pointer
    void _registerPtr(const std::string& _key, base_ptr* _data)
    {
        m_dataContainer[_key] = _data;
    }

protected:

    /// Set the actual content of the pointer
    template<Access A>
    void setPtrObject(const std::string& _key, const typename access_traits<A>::value& _obj);

private:

    /// Map of pointers indexed by key name
    std::map<std::string, base_ptr*> m_dataContainer;
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
        const std::string& _key,
        bool _autoConnect = false,
        bool _optional    = access_typed_traits<DATATYPE, ACCESS>::optional
    ) noexcept
    {
        _holder->_registerObject(_key, ACCESS, _autoConnect, _optional);
        _holder->_registerPtr(_key, this);
    }

    /// Forbids default constructors, destructor and assignment operators
    ptr()                      = delete;
    ptr(const ptr&)            = delete;
    ptr(ptr&&)                 = delete;
    ptr& operator=(const ptr&) = delete;
    ptr& operator=(ptr&&)      = delete;

private:

    /// Only the owner of the pointer can update the content of the pointer
    friend class IHasData;

    /// Assign the content of the pointer
    void set(const sight::data::Object::sptr& _obj, size_t = 0) override
    {
        if(_obj == nullptr)
        {
            this->reset();
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

    using ptr_t           = data::mt::weak_ptr<typename access_typed_traits<DATATYPE, ACCESS>::object>;
    using container_ptr_t = std::map<size_t, ptr_t>;

    /// Constructor that registers the pointer into the owner, i.e. a service instance.
    ptr_vector(
        IHasData* _holder,
        const std::string& _key,
        bool _autoConnect               = false,
        const std::uint8_t _minNbObject = 0
    ) noexcept
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
    ptr_t& operator[](const size_t _index)
    {
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
            m_ptrs[_index].reset();
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

            m_ptrs[_index] = typedObj;
        }
    }

    /// Collection of data, indexed by key
    container_ptr_t m_ptrs;
};

//------------------------------------------------------------------------------

template<Access A>
inline void IHasData::setPtrObject(
    const std::string& _key,
    const typename access_traits<A>::value& _obj
)
{
    std::string key = _key;
    size_t index    = 0;
    if(auto keyDelimiterPos = _key.rfind('#'); keyDelimiterPos != std::string::npos)
    {
        key   = _key.substr(0, keyDelimiterPos);
        index = std::stoul(_key.substr(keyDelimiterPos + 1, _key.size()));
    }

    auto itData = m_dataContainer.find(key);
    if(itData != m_dataContainer.end())
    {
        itData->second->set(std::const_pointer_cast<sight::data::Object>(_obj), index);
    }
    else
    {
        SIGHT_WARN("Could not find any registered ptr with key '" + _key + "'");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::data
