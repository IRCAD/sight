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

#ifndef __FWATOMSPATCH_SEMANTICPATCHDB_HPP__
#define __FWATOMSPATCH_SEMANTICPATCHDB_HPP__

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>

#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

class ISemanticPatch;

/**
 * @brief Contextual patch database.
 * All contextual patches are stored in this database.
 */
class FWATOMSPATCH_CLASS_API SemanticPatchDB
{
public:

    typedef SPTR (SemanticPatchDB) sptr;
    ///Typedef used to store the pair type/version of a data structure
    typedef std::pair< std::string, std::string > VersionIDType;
    ///Typedef used to store a list of contextual patches applicable on an object
    typedef std::vector< SPTR(::fwAtomsPatch::ISemanticPatch) > PatchVectorType;
    ///Typedef used to store all the contextual patches
    typedef std::map< VersionIDType, PatchVectorType > PatchesType;

    /// Registers a new patch.
    FWATOMSPATCH_API void registerPatch(SPTR(::fwAtomsPatch::ISemanticPatch) patch);

    /**
     * @brief Retrieves a patch allowing to go to specified data type/version target.
     * @param context current context
     * @param originVersion origin context version.
     * @param targetVersion target version.
     * @param objOriginClassname origin classname of the object.
     * @param objOriginVersion origin version of the object.
     *
     * @return patch applicable for data patching, or empty pointer if no such patch found.
     */
    FWATOMSPATCH_API SPTR(::fwAtomsPatch::ISemanticPatch) getPatch(
        const std::string& context,
        const std::string& originVersion,
        const std::string& targetVersion,
        const std::string& objOriginClassname,
        const std::string& objOriginVersion) const;

    /// Returns the number of patches.
    FWATOMSPATCH_API size_t size() const;

    /// Return default instance of SemanticPatchDB
    FWATOMSPATCH_API static SemanticPatchDB::sptr getDefault();

    /// Constructor. Does nothing.
    FWATOMSPATCH_API SemanticPatchDB();

    /// Destructor. Does nothing.
    FWATOMSPATCH_API virtual ~SemanticPatchDB();

private:

    /// Copy constructor. Does nothing.
    FWATOMSPATCH_API SemanticPatchDB( const SemanticPatchDB &cpy );

    /// Mutex to protect concurrent access for m_patches
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;

    /// Patches
    PatchesType m_patches;

    /// Default instance of SemanticPatchDB;
    static SemanticPatchDB::sptr s_default;

};

} // fwAtomsPatch

#endif /* __FWATOMSPATCH_SEMANTICPATCHDB_HPP__ */

