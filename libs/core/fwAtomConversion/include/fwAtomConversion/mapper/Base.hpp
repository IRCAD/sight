/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWATOMCONVERSION_MAPPER_BASE_HPP__
#define __FWATOMCONVERSION_MAPPER_BASE_HPP__

#include <string>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/mapper/factory/new.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

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

/// Base class of custom mapper between fwData and fwAtoms
class FWATOMCONVERSION_CLASS_API Base
{
public:

    /**
     * @brief Class used to register a class factory in factory registry.
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwAtomConversion::mapper::registry::get()->addFactory(
                functorKey,
                &::fwAtomConversion::mapper::factory::New<T>
                );
        }
    };

    virtual ~Base()
    {
    }

    /**
     * @brief Convert a ::fwData::Object to a ::fwAtoms::Object.
     * @param object data to convert
     * @param cache  cache to register the data already converted, used when a data is referenced multiple times.
     */
    FWATOMCONVERSION_API virtual SPTR(::fwAtoms::Object) convert(SPTR(::fwData::Object) object,
                                                                 DataVisitor::AtomCacheType & cache) = 0;

    /**
     * @brief Convert a ::fwAtoms::Object to a ::fwData::Object.
     * @param atom atom to convert
     * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times.
     * @param uuidPolicy AtomVisitor's policy
     */
    FWATOMCONVERSION_API virtual SPTR(::fwData::Object) convert(SPTR(::fwAtoms::Object) atom,
                                                                AtomVisitor::DataCacheType & cache,
                                                                const AtomVisitor::IReadPolicy &uuidPolicy
                                                                ) = 0;

};

}
}

#endif /* __FWATOMCONVERSION_MAPPER_BASE_HPP__ */
