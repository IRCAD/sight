/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <data/mt/weak_ptr.hpp>
#include <data/object.hpp>
#include <data/ptr_access.hpp>

#include <optional>

namespace sight::data
{

class base_ptr;

//------------------------------------------------------------------------------

/**
 * @brief Interface that defines a owner of data::ptr and data::ptr_vector.
 *
 * It allows to register the pointers at instantiation time and provides generic getters and setters of the pointers.
 */
class DATA_CLASS_API has_data
{
public:

    DATA_API has_data()          = default;
    DATA_API virtual ~has_data() = default;

    /**
     * @brief Return a weak data pointer of the input object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE = sight::data::object, typename CDATATYPE = std::add_const_t<DATATYPE> >
    inline data::mt::weak_ptr<CDATATYPE> input(std::string_view key, std::optional<std::size_t> index = {}) const;

    /**
     * @brief Return a weak data pointer of the in/out object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE = sight::data::object>
    inline data::mt::weak_ptr<DATATYPE> inout(std::string_view key, std::optional<std::size_t> index = {}) const;

    /**
     * @brief Return a weak data pointer of the out object at the given key and index.
     * @param key key of data to retrieve.
     * @param index of the data to retrieve.
     * @return weak data pointer in the right type, expired pointer if not found.
     */
    template<class DATATYPE = sight::data::object>
    inline data::mt::weak_ptr<DATATYPE> output(std::string_view key, std::optional<std::size_t> index = {}) const;

    /**
     * @brief Return the input, inout or output object at the given key.
     * @param _key key of data to retrieve.
     * @param[in] _access access to the object (in/inout/out)
     * @param _index optional index of the data to retrieve.
     * @return data object, nullptr if not found.
     */
    DATA_API data::object::csptr object(
        std::string_view _key,
        data::Access _access,
        std::optional<std::size_t> index = {}) const;

    /**
     * @brief Set an input object, and overrides the default auto_connect and optional settings.
     *
     * @param[in] _obj input object used by the service
     * @param[in] _key key of the object
     * @param[in] _autoConnect if true, the service will be connected to the object's signals
     * @param[in] _optional if true, the service can be started even if the objet is not present
     * @param[in] _index if specified, indicates the object is part of a group and gives its index
     */
    DATA_API void set_input(
        data::object::csptr _obj,
        std::string_view _key,
        std::optional<bool> _autoConnect  = {},
        std::optional<bool> _optional     = {},
        std::optional<std::size_t> _index = {});

    /**
     * @brief Set an in/outs object, and overrides the default auto_connect and optional settings.
     *
     * @param[in] _obj in/out object used by the service
     * @param[in] _key key of the object
     * @param[in] _autoConnect if true, the service will be connected to the object's signals
     * @param[in] _optional if true, the service can be started even if the objet is not present
     * @param[in] _index if specified, indicates the object is part of a group and gives its index
     */
    DATA_API void set_inout(
        data::object::sptr _obj,
        std::string_view _key,
        std::optional<bool> _autoConnect  = {},
        std::optional<bool> _optional     = {},
        std::optional<std::size_t> _index = {});

    /**
     * @brief Register an output object, replacing it if it already exists.
     * @param _key name of the data or the group to register.
     * @param _object pointer to the object to register.
     * @param _index optional index of the key in the case of a member of a group of keys.
     * @warning The service manages the output object lifetime: if it creates a new object, it will be the only
     * maintainer of this object, when calling set_output, it allows to share the object with other services. But these
     * services will not maintain a reference to this object (only weak_ptr). When the service stops, it should remove
     * its outputs by calling set_output(key, nullptr). Otherwise, a service may work on an expired object.
     */
    DATA_API void set_output(
        std::string_view _key,
        data::object::sptr _object,
        std::optional<std::size_t> _index = {});

protected:

    /**
     * @brief Set a registered object for this service
     *
     * @param[in] _obj object used by the service
     * @param[in] _key key of the object
     * @param[in] _index index of the data in the group
     * @param[in] _access access to the object (in/inout/out)
     * @param[in] _autoConnect if true, the service will be connected to the object's signals
     * @param[in] _optional if true, the service can be started even if the objet is not present
     */
    DATA_API void set_object(
        data::object::sptr _obj,
        std::string_view _key,
        std::optional<std::size_t> _index,
        data::Access _access,
        bool _autoConnect,
        bool _optional
    );

    /**
     * @brief Unset a registered object for this service
     *
     * @param[in] _key key of the object
     * @param[in] _index index of the data in the group
     */
    DATA_API void reset_object(std::string_view _key, std::optional<std::size_t> _index);

    /**
     * @brief Set the deferred identifier of a key. This is useful to declare an object that is not present at start of
     * the application and that will be created later.
     * @param[in] _key key of the object
     * @param[in] _id label of the object
     * @param[in] _index index of the data in the group
     */
    DATA_API void set_deferred_id(
        std::string_view _key,
        const std::string& _id,
        std::optional<std::size_t> _index = {});

    using container_t = std::map<std::pair<std::string_view, std::optional<std::size_t> >, base_ptr*>;
    DATA_API const container_t& container() const;

private:

    friend class base_ptr;
    template<class DATATYPE, data::Access ACCESS>
    friend class ptr;
    template<class DATATYPE, data::Access ACCESS>
    friend class ptr_vector;

    /// Registers a pointer
    DATA_API void registerPtr(std::string_view _key, base_ptr* _data, std::optional<std::size_t> _index = 0);
    /// Unregisters a pointer
    DATA_API void unregisterPtr(base_ptr* _data);
    /// Notifies that a new object has been created and available
    DATA_API virtual void notify_register_out(data::object::sptr, const std::string&) = 0;
    /// Notifies that a new object is being destroyed and no longer available
    DATA_API virtual void notify_unregister_out(data::object::sptr, const std::string&) = 0;

    /**
     * @brief Map of data pointers, data::ptr and data::ptr_vector.
     * Each pointer registers itself in this map from its constructor.
     * They always register with {} as index.
     * Then, data::ptr_vector registers a new data::ptr each time a new data is inserted with a real index this time.
     * This means that a data::ptr_vector of size 4, is actually registered 5 times, once for the data::ptr_vector, and
     * 4 times for each contained data::ptr.
     * Accessing elements with {} index is used to get initial properties (autoconnect, optional).
     */
    container_t m_dataContainer;
};

//------------------------------------------------------------------------------

template<class DATATYPE, typename CDATATYPE>
inline data::mt::weak_ptr<CDATATYPE> has_data::input(
    std::string_view _key,
    std::optional<std::size_t> _index
) const
{
    data::mt::weak_ptr<CDATATYPE> input;
    input = std::dynamic_pointer_cast<CDATATYPE>(this->object(_key, data::Access::in, _index));
    return input;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> has_data::inout(std::string_view _key, std::optional<std::size_t> _index) const
{
    data::mt::weak_ptr<DATATYPE> inout;
    inout =
        std::dynamic_pointer_cast<DATATYPE>(
            std::const_pointer_cast<data::object>(
                this->object(
                    _key,
                    data::Access::inout,
                    _index
                )
            )
        );
    return inout;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> has_data::output(std::string_view _key, std::optional<std::size_t> _index) const
{
    data::mt::weak_ptr<DATATYPE> out;
    out =
        std::dynamic_pointer_cast<DATATYPE>(
            std::const_pointer_cast<data::object>(
                this->object(
                    _key,
                    data::Access::out,
                    _index
                )
            )
        );
    return out;
}

//------------------------------------------------------------------------------

} // namespace sight::data
