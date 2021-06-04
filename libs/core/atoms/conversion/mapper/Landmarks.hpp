/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "atoms/config.hpp"
#include "atoms/conversion/AtomVisitor.hpp"
#include "atoms/conversion/mapper/Base.hpp"

namespace sight::atoms
{

class Object;

}

namespace sight::data
{

class Object;

}

namespace sight::atoms::conversion
{

namespace mapper
{

/**
 * @brief Specific mapper used to convert a data::Landmarks.
 *
 * The attribute data::Landmarks::m_landmarks is represented in fwAtoms world like a atoms::Map that contains
 * atoms::Object with five attributes "color", "size", "shape", "visibility" and "points". The points are store in a
 * atoms::Sequence.
 */
class ATOMS_CLASS_API Landmarks : public Base
{
public:

    /**
     * @brief Convert a data::Object to a atoms::Object.
     * @param object data to convert. It must be a data::Landmarks.
     * @param cache  cache to register the data already converted, used when a data is referenced multiple times.
     */
    ATOMS_API virtual SPTR(atoms::Object) convert(
        SPTR(data::Object) object,
        DataVisitor::AtomCacheType& cache
    );

    /**
     * @brief Convert a atoms::Object to a data::Object (which represent a data::Landmarks).
     * @param atom atom to convert
     * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times.
     * @param uuidPolicy AtomVisitor policy
     */
    ATOMS_API virtual SPTR(data::Object) convert(
        SPTR(atoms::Object) atom,
        AtomVisitor::DataCacheType& cache,
        const AtomVisitor::IReadPolicy& uuidPolicy
    );
};

}

}
