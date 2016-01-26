/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
