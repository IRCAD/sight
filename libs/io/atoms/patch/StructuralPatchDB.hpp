/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "io/atoms/config.hpp"
#include "io/atoms/patch/IStructuralPatch.hpp"

#include <core/macros.hpp>
#include <core/mt/types.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sight::io::atoms::patch
{

class IStructuralPatch;

/**
 * @brief Structural patch database.
 * All structural patches are stored in this database.
 */
class IO_ATOMS_CLASS_API StructuralPatchDB
{

public:
    typedef SPTR (StructuralPatchDB) sptr;
    ///Typedef used to store the pair classname/version of a data structure
    typedef std::pair< std::string, std::string > VersionIDType;
    ///Typedef used to store link to data structure
    typedef std::pair< VersionIDType, VersionIDType > PatchKeyType;
    ///Typedef used to store all the structural patches
    typedef std::map< PatchKeyType, SPTR(io::atoms::patch::IStructuralPatch) > PatchesType;

    /**
     * @brief Registers a new Path.
     * @param patch the new Patch.
     * @note This method is thread safe.
     */
    IO_ATOMS_API void registerPatch(SPTR(io::atoms::patch::IStructuralPatch) patch);

    /**
     * @brief Retrieves patch to go to specified data revision.
     * @param originClassname origin classname.
     * @param originVersion origin version.
     * @param targetClassname target classname.
     * @param targetVersion target version.
     *
     * @return patch that must be applied, or empty pointer if no such patch is found.
     * @note This method is thread safe.
     */
    IO_ATOMS_API SPTR(io::atoms::patch::IStructuralPatch) getPatch(const std::string& originClassname,
                                                                   const std::string& originVersion,
                                                                   const std::string& targetClassname,
                                                                   const std::string& targetVersion) const;

    /// Retrieves number of patch.
    IO_ATOMS_API size_t size() const;

    /// Returns default instance.
    static io::atoms::patch::StructuralPatchDB::sptr getDefault()
    {
        return s_default;
    }

    /// Constructor
    StructuralPatchDB()
    {
    }

    /// Destructor
    ~StructuralPatchDB()
    {
    }

private:

    /// Copy constructor
    StructuralPatchDB( const StructuralPatchDB& cpy )
    {
    }

    /// Mutex to protect concurrent access for m_patches
    mutable core::mt::ReadWriteMutex m_mutex;

    /// Patches
    PatchesType m_patches;

    /// Default instance;
    IO_ATOMS_API static io::atoms::patch::StructuralPatchDB::sptr s_default;
};

} // fwAtomsPatch
