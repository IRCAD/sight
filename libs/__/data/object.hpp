/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/factory/new.hpp"
#include "data/registry/detail.hpp"

#include <core/base.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/mt/types.hpp>
#include <core/tools/compare.hpp>
#include <core/tools/object.hpp>

#include <string>
#include <unordered_map>

namespace sight::data
{

namespace mt
{

template<typename T>
class locked_ptr;

} // namespace mt

/**
 * @brief   Base class for each data object.
 *
 * Each object can optionally embed sub-object with string identifier. We name this concept a field.
 * An object containing a field name "dummy" corresponds to having a labeledObject with label "dummy" and
 * containing a specific object. When accessing to this object with get_field("dummy") we get the specific object
 */
class SIGHT_DATA_CLASS_API object : public core::tools::object,
                                    public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(object, core::tools::object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        registry()
        {
            sight::data::registry::get()->add_factory(T::classname(), &factory::make<T>);
        }
    };

    /**
     * @name Signals
     * @{
     */
    /// Type of signal m_sigModified
    using modified_signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigModified
    SIGHT_DATA_API static const core::com::signals::key_t MODIFIED_SIG;

    using fields_container_t = std::map<std::string, object::sptr>;
    /// Type of signal when objects are added
    using added_fields_signal_t = core::com::signal<void (fields_container_t)>;
    SIGHT_DATA_API static const core::com::signals::key_t ADDED_FIELDS_SIG;

    /// Type of signal when objects are changed (newObjects, oldObjects)
    using changed_fields_signal_t = core::com::signal<void (fields_container_t, fields_container_t)>;
    SIGHT_DATA_API static const core::com::signals::key_t CHANGED_FIELDS_SIG;

    /// Type of signal when objects are removed
    using removed_fields_signal_t = core::com::signal<void (fields_container_t)>;
    SIGHT_DATA_API static const core::com::signals::key_t REMOVED_FIELDS_SIG;
    /**
     * @}
     */

    using field_name_t        = std::string;
    using field_name_vector_t = std::vector<field_name_t>;
    using field_map_t         = std::unordered_map<field_name_t, object::sptr>;

    using deep_copy_cache_t = std::unordered_map<object::csptr, object::sptr>;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] _name field name
     * @param[in] _default_value Default value
     * @return defaultValue if field is not found
     */
    SIGHT_DATA_API object::sptr get_field(
        const field_name_t& _name,
        object::sptr _default_value = object::sptr()
    ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] _name field name
     * @return pointer to corresponding field, nullptr if field is not found.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) get_field(const field_name_t& _name) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] _name field name
     * @param[in] _default_value Default value
     * @return pointer to corresponding field, defaultValue if field is not found.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) get_field(const field_name_t& _name, SPTR(DATA_TYPE) _default_value) const;

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] _name field name
     * @param[in] _default_value default return value if field was not found
     * @return pointer to corresponding field.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) set_default_field(const field_name_t& _name, SPTR(DATA_TYPE) _default_value);

    /**
     * @brief Returns fields map.
     */
    SIGHT_DATA_API const field_map_t& get_fields() const;

    /**
     * @brief Returns vector of field names.
     */
    SIGHT_DATA_API field_name_vector_t get_field_names() const;

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * @param[in] _name field name
     * @param[in] _obj  field
     */
    SIGHT_DATA_API void set_field(const field_name_t& _name, object::sptr _obj);

    /**
     * @brief Replace the field map content.
     */
    SIGHT_DATA_API void set_fields(const field_map_t& _field_map);

    /**
     * @brief Removes field with specified name.
     * @param[in] _name field name
     */
    SIGHT_DATA_API void remove_field(const field_name_t& _name);

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API virtual void shallow_copy(const object::csptr& _source);

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API virtual void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    );

    /**
     * @brief return a copy of the source. if source is a null pointer, return a null pointer.
     * @{
     */
    SIGHT_DATA_API static object::sptr copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    );

    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(
        const CSPTR(DATA_TYPE) & _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    );

    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(
        const SPTR(DATA_TYPE) & _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    );
    /** @} */

    /// Returns the object's mutex.
    inline core::mt::read_write_mutex& get_mutex() const
    {
        return m_mutex;
    }

    SIGHT_DATA_API ~object() noexcept override = default;

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const object& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const object& _other) const noexcept;
    /// @}

    /// Accessors
    /// @{
    inline std::string get_description() const noexcept;
    inline void set_description(const std::string& _description) noexcept;
    /// @}

    /// Returns a timestamp to know when the object was last modified
    inline std::uint64_t last_modified() const noexcept;

protected:

    SIGHT_DATA_API object();

    /// Description allows to distinguish between different instances of the same class
    std::string m_description;

    /// Fields
    field_map_t m_fields;

    /// Mutex to protect object access.
    mutable core::mt::read_write_mutex m_mutex;

    /// Timestamp that can be incremented each time the object is modified
    std::uint64_t m_last_modified {0};

private:

    template<class T>
    friend class sight::data::mt::locked_ptr;

    /// Increments the last modified timestamp
    SIGHT_DATA_API inline void set_modified() noexcept;
};

template<typename DATA_TYPE>
SPTR(DATA_TYPE) object::copy(const CSPTR(DATA_TYPE) & _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    return std::dynamic_pointer_cast<DATA_TYPE>(object::copy(object::csptr(_source), _cache));
}

template<typename DATA_TYPE>
SPTR(DATA_TYPE) object::copy(const SPTR(DATA_TYPE) & _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    return std::dynamic_pointer_cast<DATA_TYPE>(object::copy(object::csptr(_source), _cache));
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) object::get_field(const field_name_t& _name) const
{
    object::sptr field;
    field                  = this->get_field(_name, field);
    SPTR(DATA_TYPE) result = std::dynamic_pointer_cast<DATA_TYPE>(field);
    return result;
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) object::get_field(const field_name_t& _name, SPTR(DATA_TYPE) _default_value) const
{
    object::sptr field = _default_value;
    field                  = this->get_field(_name, field);
    SPTR(DATA_TYPE) result = std::dynamic_pointer_cast<DATA_TYPE>(field);
    return result;
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) object::set_default_field(const field_name_t& _name, SPTR(DATA_TYPE) _default_value)
{
    SPTR(DATA_TYPE) result = get_field<DATA_TYPE>(_name);
    if(!result && _default_value)
    {
        result = _default_value;
        this->set_field(_name, _default_value);
    }

    return result;
}

//------------------------------------------------------------------------------

inline std::string object::get_description() const noexcept
{
    return m_description;
}

//------------------------------------------------------------------------------

inline void object::set_description(const std::string& _description) noexcept
{
    m_description = _description;
}

//-----------------------------------------------------------------------------

inline std::uint64_t object::last_modified() const noexcept
{
    return m_last_modified;
}

//-----------------------------------------------------------------------------

inline void object::set_modified() noexcept
{
    ++m_last_modified;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
