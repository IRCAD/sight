/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwAtomsFilter/config.hpp"
#include "fwAtomsFilter/factory/new.hpp"
#include "fwAtomsFilter/registry/detail.hpp"

#include <fwAtoms/Object.hpp>

#include <fwCore/base.hpp>

#include <map>
#include <string>

namespace fwAtoms
{
class Object;
}

namespace fwAtomsFilter
{

/**
 * @brief Removes atom attributes which are not managed by a context.
 */
class FWATOMSFILTER_CLASS_API IFilter : public ::fwCore::BaseObject
{

public:
    /// Factory key used by IFilter implementations
    typedef ::fwAtomsFilter::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwAtomsFilter::registry::get()->addFactory(functorKey, &::fwAtomsFilter::factory::New<T>);
        }
    };

    fwCoreClassMacro(IFilter);
    fwCoreAllowSharedFromThis();

    /// Constructor.
    FWATOMSFILTER_API IFilter();

    /// Destructor.
    FWATOMSFILTER_API virtual ~IFilter();

    /// Applies the filter onto the specified object
    FWATOMSFILTER_API virtual void apply(const SPTR(::fwAtoms::Object)& object) = 0;

};

} //fwAtomsFilter
