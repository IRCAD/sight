/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_PATCHINGMANAGER_HPP__
#define __FWATOMSPATCH_PATCHINGMANAGER_HPP__

#include <fwCore/macros.hpp>

#include "fwAtomsPatch/VersionsGraph.hpp"

#include "fwAtomsPatch/config.hpp"

namespace fwAtoms
{
class Object;
}

namespace fwAtomsPatch
{

/**
 * @class PatchingManager
 * @brief Manages object patching.
 *
 * To be used, PatchingManager requires that available contexts and versions are loaded into
 * the graph of versions through the VersionsManager.
 */
class FWATOMSPATCH_CLASS_API PatchingManager
{
public:

    /// Default constructor
    FWATOMSPATCH_API PatchingManager(SPTR(::fwAtoms::Object)object);

    /// Default destructor
    FWATOMSPATCH_API ~PatchingManager();

    /**
     * @brief Transforms the object to the given version.
     * @pre Given version must be known by the VersionsManager
     *
     * @throw ::fwAtomsPatch::exceptions::MissingInformation if an information about patching is missing
     * @throw ::fwAtomsPatch::exceptions::ImpossibleConversion if there is no patch available to transform an object
     */
    FWATOMSPATCH_API ::fwAtoms::Object::sptr transformTo(const std::string& newVersion);

protected:

    /// Object that we want to patch
    SPTR(::fwAtoms::Object) m_object;
};

}

#endif //__FWATOMSPATCH_PATCHINGMANAGER_HPP__
