/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwAtomConversion/AtomVisitor.hpp"
#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/mapper/Base.hpp"

namespace fwAtoms
{
class Object;
}

namespace fwData
{
class Object;
}

namespace fwAtomConversion
{
namespace mapper
{

/**
 * @brief Specific mapper used to convert a ::fwData::Mesh
 *
 * The attribute ::fwData::Mesh::m_attributes ( of type ::fwData::Mesh::Attributes ) is
 * represented in fwAtoms as a UINT Numeric value.
 */
class FWATOMCONVERSION_CLASS_API Mesh : public Base
{
public:

    /**
     * @brief Convert a ::fwData::Object to a ::fwAtoms::Object.
     * @param object data to convert. It must be a ::fwData::Mesh.
     * @param cache  cache to register the data already converted, used when a data is referenced multiple times.
     */
    FWATOMCONVERSION_API virtual SPTR(::fwAtoms::Object) convert( SPTR(::fwData::Object) object,
                                                                  DataVisitor::AtomCacheType& cache );

    /**
     * @brief Convert a ::fwAtoms::Object to a ::fwData::Object (which represent a ::fwData::Mesh).
     * @param atom atom to convert
     * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times.
     * @param uuidPolicy AtomVisitor policy
     */
    FWATOMCONVERSION_API virtual SPTR(::fwData::Object) convert( SPTR(::fwAtoms::Object) atom,
                                                                 AtomVisitor::DataCacheType& cache,
                                                                 const AtomVisitor::IReadPolicy& uuidPolicy
                                                                 );

};

}
}
