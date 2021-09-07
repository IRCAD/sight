/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

class Object;
/**
 * @brief   Defines ID for core::tools::Object. It is used to associate ID with object.
 */
class CORE_CLASS_API fwID
{
public:

    SIGHT_DECLARE_CLASS(fwID);

    typedef std::string IDType;

    typedef enum
    {
        EMPTY = 1, ///< return an empty id if no one set
        GENERATE,  ///< generate a new id if necessary
        MUST_EXIST ///< throw an exception if object has not id.
    } Policy;

    /**
     * Test if the given id exist (i.e recorded in fwID dictionary)
     * @param[in] _id : the id to test.
     * @return true iff the given id is recorded in fwID dictionary.
     * @note This method is thread-safe.
     */
    CORE_API static bool exist(IDType _id);

    /**
     * @brief Retrieve the object attached to the given id. Return a null sptr if no correspondence exist.
     * @note This method is thread-safe.
     */
    CORE_API static SPTR(core::tools::Object) getObject(IDType requestID);

    CORE_API virtual ~fwID();

protected:

    // API to expose in core::tools::Object
    /**
     * @brief Return true if the object has an id set.
     * @note This method is thread-safe.
     */
    CORE_API bool hasID() const;

    /**
     * @brief Returns the id of the object. If it is not set and the policy value is
     * \li EMPTY then an empty id is returned
     * \li GENERATE (default) then a new ID will be generated (and recorded ) using the pattern "CLASSNAME-NUM". NUM is
     * always increasing
     * \li MUST_EXIST then an exception Failed is raised
     * @note We consider an object be constant whatever if its id is generated.
     * @note This method is thread-safe.
     */
    CORE_API IDType getID(Policy policy = GENERATE) const;

    /**
     * @brief Set a newID  for the object, (newID must not exist in fwID), the oldest one is released.
     * @warning Cannot set a empty ID.
     * @note This method is thread-safe.
     */
    CORE_API virtual void setID(IDType newID); // cannot set a empty one

    /**
     * @brief Release the id for the object.
     * @note This method is thread-safe
     */
    CORE_API void resetID();

    /**
     * @brief   Constructor : does nothing.
     */
    fwID()
    {
    } // cannot be instantiated

private:

    /**
     * @brief Will generate a new ID using the pattern "CLASSNAME-NUM". NUM is always increasing.
     * @note This method is NOT thread-safe.
     */
    IDType generate() const;

    /**
     * @brief Remove ID from the dictionary.
     * @note This method is NOT thread-safe.
     */
    static void removeIDfromDictionary(IDType _id);

    /**
     * @brief return true if the  _id is found in the dictionary.
     * @note This method is NOT thread-safe.
     */
    static bool isIdFound(IDType _id);

    /**
     * @brief Add newID in the dictionary (newID must not exist in fwID).
     * @note This method is NOT thread-safe
     */
    void addIDInDictionary(IDType newID);

    IDType m_id;

    typedef std::unordered_map<IDType, WPTR(core::tools::Object)> Dictionary;
    typedef std::unordered_map<std::string, std::uint32_t> CategorizedCounter;

    static Dictionary m_dictionary;
    static CategorizedCounter m_CategorizedCounter;

    /// Mutex used to lock dictionary access
    static core::mt::ReadWriteMutex s_dictionaryMutex;

    /// Mutex used by generate() to lock m_CategorizedCounter changes.
    static core::mt::Mutex s_mutexCounter;

    /// Mutex used to lock m_id access
    mutable core::mt::ReadWriteMutex m_idMutex;
};

} // namespace sight::core
