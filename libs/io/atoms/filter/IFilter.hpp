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

#include "io/atoms/config.hpp"
#include "io/atoms/filter/factory/new.hpp"
#include "io/atoms/filter/registry/detail.hpp"

#include <atoms/Object.hpp>

#include <core/base.hpp>

#include <map>
#include <string>

namespace sight::atoms
{
class Object;
}

namespace sight::io::atoms::filter
{

/**
 * @brief Removes atom attributes which are not managed by a context.
 */
class IO_ATOMS_CLASS_API IFilter : public core::BaseObject
{

public:
    /// Factory key used by IFilter implementations
    typedef io::atoms::filter::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registry
    {
    public:
        Registry(std::string functorKey)
        {
            io::atoms::filter::registry::get()->addFactory(functorKey, &io::atoms::filter::factory::New<T>);
        }
    };

    SIGHT_DECLARE_CLASS(IFilter)
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Constructor.
    IO_ATOMS_API IFilter();

    /// Destructor.
    IO_ATOMS_API virtual ~IFilter();

    /// Applies the filter onto the specified object
    IO_ATOMS_API virtual void apply(const SPTR(sight::atoms::Object)& object) = 0;

};

} //fwAtomsFilter
