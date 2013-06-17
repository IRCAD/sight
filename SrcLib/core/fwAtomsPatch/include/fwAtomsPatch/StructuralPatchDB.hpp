/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_STRUCTURALPATCHDB_HPP__
#define __FWATOMSPATCH_STRUCTURALPATCHDB_HPP__

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <fwCore/macros.hpp>
#include <fwCore/mt/types.hpp>

#include "fwAtomsPatch/config.hpp"

#include "fwAtomsPatch/IStructuralPatch.hpp"

namespace fwAtomsPatch
{

class IStructuralPatch;

/**
 * @brief Structural patch database.
 * All structural patches are stored in this database.
 */
class FWATOMSPATCH_CLASS_API StructuralPatchDB
{

public:
    typedef SPTR(StructuralPatchDB) sptr;
    ///Typedef used to store the pair classname/version of a data structure
    typedef std::pair< std::string, std::string > VersionIDType;
    ///Typedef used to store link to data structure
    typedef std::pair< VersionIDType, VersionIDType > PatchKeyType;
    ///Typedef used to store all the structural patches
    typedef std::map< PatchKeyType, SPTR(::fwAtomsPatch::IStructuralPatch) > PatchesType;

    /**
     * @brief Registers a new Path.
     * @param patch the new Patch.
     * @note This method is thread safe.
     */
    FWATOMSPATCH_API void registerPatch(SPTR(::fwAtomsPatch::IStructuralPatch) patch);

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
    FWATOMSPATCH_API SPTR(::fwAtomsPatch::IStructuralPatch) getPatch(const std::string& originClassname,
                                                                            const std::string& originVersion,
                                                                            const std::string& targetClassname,
                                                                            const std::string& targetVersion) const;

    /// Retrieves number of patch.
   FWATOMSPATCH_API size_t size() const;

   /// Returns default instance.
   static ::fwAtomsPatch::StructuralPatchDB::sptr getDefault() { return s_default; }

   /// Constructor
    StructuralPatchDB() {};

    /// Destructor
    ~StructuralPatchDB() {};

private:


    /// Copy constructor
    StructuralPatchDB( const StructuralPatchDB &cpy ) {};

    /// Mutex to protect concurrent access for m_patches
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;

    /// Patches
    PatchesType m_patches;

    /// Default instance;
    FWATOMSPATCH_API static ::fwAtomsPatch::StructuralPatchDB::sptr s_default;
};

} // fwAtomsPatch

#endif /* __FWATOMSPATCH_STRUCTURALPATCHDB_HPP__ */
