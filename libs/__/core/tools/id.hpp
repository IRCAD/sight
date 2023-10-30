/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "core/config.hpp"

#include <core/base.hpp>
#include <core/mt/types.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace sight::core::tools
{

class object;
/**
 * @brief   Defines ID for core::tools::object. It is used to associate ID with object.
 */
class CORE_CLASS_API id
{
public:

    SIGHT_DECLARE_CLASS(id);

    using type = std::string;

    enum class policy
    {
        empty = 1, ///< return an empty id if no one set
        generate,  ///< generate a new id if necessary
        must_exist ///< throw an exception if object has not id.
    };

    /**
     * Test if the given id exist (i.e recorded in fwID dictionary)
     * @param[in] _id : the id to test.
     * @return true iff the given id is recorded in fwID dictionary.
     * @note This method is thread-safe.
     */
    CORE_API static bool exist(type _id);

    /**
     * @brief Retrieve the object attached to the given id. Return a null sptr if no correspondence exist.
     * @note This method is thread-safe.
     */
    CORE_API static SPTR(core::tools::object) get_object(type _request_id);

    CORE_API virtual ~id();

protected:

    // API to expose in core::tools::object
    /**
     * @brief Return true if the object has an id set.
     * @note This method is thread-safe.
     */
    CORE_API bool has_id() const;

    /**
     * @brief Returns the id of the object. If it is not set and the policy value is
     * \li EMPTY then an empty id is returned
     * \li GENERATE (default) then a new ID will be generated (and recorded ) using the pattern "CLASSNAME-NUM". NUM is
     * always increasing
     * \li MUST_EXIST then an exception Failed is raised
     * @note We consider an object be constant whatever if its id is generated.
     * @note This method is thread-safe.
     */
    CORE_API type get_id(policy _policy = policy::generate) const;

    /**
     * @brief Set a newID  for the object, (newID must not exist in fwID), the oldest one is released.
     * @warning Cannot set a empty ID.
     * @note This method is thread-safe.
     */
    CORE_API virtual void set_id(type _new_id); // cannot set a empty one

    /**
     * @brief Release the id for the object.
     * @note This method is thread-safe
     */
    CORE_API void reset_id();

    /**
     * @brief   Constructor : does nothing.
     */
    id()
    = default; // cannot be instantiated

private:

    /**
     * @brief Will generate a new ID using the pattern "CLASSNAME-NUM". NUM is always increasing.
     * @note This method is NOT thread-safe.
     */
    type generate() const;

    /**
     * @brief Remove ID from the dictionary.
     * @note This method is NOT thread-safe.
     */
    static void remove_id_from_dictionary(type _id);

    /**
     * @brief return true if the  _id is found in the dictionary.
     * @note This method is NOT thread-safe.
     */
    static bool is_id_found(type _id);

    /**
     * @brief Add newID in the dictionary (newID must not exist in fwID).
     * @note This method is NOT thread-safe
     */
    void add_id_in_dictionary(type _new_id) const;

    /// The ID associated with the object. It is mutable, as it may be modified with a call to get_id(GENERATE).
    mutable type m_id;

    using dictionary          = std::unordered_map<type, std::weak_ptr<core::tools::object> >;
    using categorized_counter = std::unordered_map<std::string, std::uint32_t>;

    static dictionary s_dictionary;
    static categorized_counter s_categorized_counter;

    /// Mutex used to lock dictionary access
    static core::mt::read_write_mutex s_dictionary_mutex;

    /// Mutex used by generate() to lock m_CategorizedCounter changes.
    static core::mt::mutex s_mutex_counter;

    /// Mutex used to lock m_id access
    mutable core::mt::read_write_mutex m_id_mutex;
};

} // namespace sight::core::tools
