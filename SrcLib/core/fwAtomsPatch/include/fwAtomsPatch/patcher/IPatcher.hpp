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

#ifndef __FWATOMSPATCH_PATCHER_IPATCHER_HPP__
#define __FWATOMSPATCH_PATCHER_IPATCHER_HPP__

#include <string>

#include <fwTools/Object.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomsPatch/patcher/factory/new.hpp"
#include "fwAtomsPatch/patcher/registry/detail.hpp"

#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"
#include "fwAtomsPatch/VersionsGraph.hpp"

#include "fwAtomsPatch/patcher/registry/detail.hpp"
#include "fwAtomsPatch/patcher/factory/new.hpp"



#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

namespace patcher
{

/**
 * @class IPatcher
 * @brief Base class of all patchers
 */
class FWATOMSPATCH_CLASS_API IPatcher : public ::fwTools::Object
{

public:

    /// Factory key used by IPatcher implementations
    typedef ::fwAtomsPatch::patcher::factory::Key Key;

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
            ::fwAtomsPatch::patcher::registry::get()->addFactory(functorKey, &::fwAtomsPatch::patcher::factory::New<T>);
        }
    };


public:

    fwCoreNonInstanciableClassDefinitionsMacro((IPatcher)(::fwTools::Object));
    fwCoreAllowSharedFromThis();

    /// Constructor
    FWATOMSPATCH_API IPatcher();

    /// Destructor
    FWATOMSPATCH_API virtual ~IPatcher();

    /// Transform the specified object
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr transformObject(::fwAtoms::Object::sptr object,
                                                                     const std::string &context,
                                                                     const std::string &currentVersion,
                                                                     const std::string &targetVersion) = 0;

};

} // namespace patcher

} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_PATCHER_IPATCHER_HPP__ */
