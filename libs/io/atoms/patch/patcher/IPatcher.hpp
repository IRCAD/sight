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
#include "io/atoms/patch/helper/Object.hpp"
#include "io/atoms/patch/patcher/factory/new.hpp"
#include "io/atoms/patch/patcher/registry/detail.hpp"
#include "io/atoms/patch/VersionDescriptor.hpp"
#include "io/atoms/patch/VersionsGraph.hpp"

#include <atoms/Object.hpp>

#include <core/tools/Object.hpp>

#include <string>

namespace sight::io::atoms::patch
{

namespace patcher
{

/**
 * @brief Base class of all patchers
 */
class IO_ATOMS_CLASS_API IPatcher : public core::tools::Object
{
public:

    /// Factory key used by IPatcher implementations
    typedef io::atoms::patch::patcher::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry(std::string functorKey)
        {
            io::atoms::patch::patcher::registry::get()->addFactory(
                functorKey,
                &io::atoms::patch::patcher::factory::New<T>
            );
        }
    };

public:

    SIGHT_DECLARE_CLASS(IPatcher, core::tools::Object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Constructor
    IO_ATOMS_API IPatcher();

    /// Destructor
    IO_ATOMS_API virtual ~IPatcher();

    /// Transform the specified object
    IO_ATOMS_API virtual sight::atoms::Object::sptr transformObject(
        sight::atoms::Object::sptr object,
        const std::string& context,
        const std::string& currentVersion,
        const std::string& targetVersion
    ) = 0;
};

} // namespace patcher

} // namespace sight::io::atoms::patch
