/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWDATATOOLS_HELPER_VECTOR_HPP__
#define __FWDATATOOLS_HELPER_VECTOR_HPP__

#include "fwDataTools/config.hpp"

#include <fwData/Vector.hpp>

namespace fwDataTools
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Vector and create in parallel the message to announce this modification.
 */
class FWDATATOOLS_CLASS_API Vector
{

public:

    /// Constructor. Initialize parameters.
    FWDATATOOLS_API Vector( ::fwData::Vector::wptr vector );

    /// Destrucotr. Do nothing.
    FWDATATOOLS_API ~Vector();

    /**
     * @brief Add an object in the vector.
     * @param[in] _newObject    object to add in the vector
     *
     * Prepare the signal to announce the modification.
     */
    FWDATATOOLS_API void add( ::fwData::Object::sptr _newObject );

    /**
     * @brief Remove an object in the vector.
     * @param[in] _oldObject object to remove from the vector.
     *
     * Prepare the signal to announce the modification.
     */
    FWDATATOOLS_API void remove( ::fwData::Object::sptr _oldObject );

    /**
     * @brief Clear all objects in the vector.
     *
     * Prepare the signal to announce the modification.
     */
    FWDATATOOLS_API void clear();

    /// Send the signal of modification
    FWDATATOOLS_API void notify();

private:

    ::fwData::Vector::ContainerType m_addedObjects;
    ::fwData::Vector::ContainerType m_removedObjects;
    ::fwData::Vector::wptr m_vector;
};

} // namespace helper
} // namespace fwDataTools

#endif // __FWDATATOOLS_HELPER_VECTOR_HPP__
