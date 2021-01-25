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

#include "fwAtomsPatch/config.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtomsPatch/patcher/IPatcher.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/base.hpp>

#include <string>

namespace fwAtomsPatch
{

namespace patcher
{

/**
 * @brief Default patcher
 */
class FWATOMSPATCH_CLASS_API DefaultPatcher : public IPatcher
{

public:

    fwCoreClassMacro(DefaultPatcher, ::fwAtomsPatch::patcher::IPatcher,
                     ::fwAtomsPatch::patcher::factory::New< DefaultPatcher >)

    /// Constructor
    FWATOMSPATCH_API DefaultPatcher(::fwAtomsPatch::patcher::IPatcher::Key key);

    /// Destructor
    FWATOMSPATCH_API virtual ~DefaultPatcher();

    /// Apply the patch to the specified object
    FWATOMSPATCH_API virtual atoms::Object::sptr transformObject(atoms::Object::sptr object,
                                                                 const std::string& context,
                                                                 const std::string& currentVersion,
                                                                 const std::string& targetVersion) override;

protected:

    /// Process structural atom object.
    FWATOMSPATCH_API virtual atoms::Object::sptr processStructuralObject(atoms::Object::sptr current);

    /// Process contextual atom object.
    FWATOMSPATCH_API virtual atoms::Object::sptr processContextualObject(atoms::Object::sptr current);

    /// Process atom base.
    FWATOMSPATCH_API virtual atoms::Base::sptr processBase(atoms::Base::sptr base);

    /// Process atom mapping.
    FWATOMSPATCH_API virtual atoms::Map::sptr processMapping(atoms::Map::sptr map);

    /// Process atom sequence.
    FWATOMSPATCH_API virtual atoms::Sequence::sptr processSequence(atoms::Sequence::sptr seq);

    /// Apply structural patch.
    FWATOMSPATCH_API virtual atoms::Object::sptr applyStructuralPatch(atoms::Object::sptr previous,
                                                                      atoms::Object::sptr current);

    /// Apply contextual patch.
    FWATOMSPATCH_API virtual atoms::Object::sptr applyContextualPatch(atoms::Object::sptr previous,
                                                                      atoms::Object::sptr current);
    /// Return true if the object is known in the target version
    FWATOMSPATCH_API bool isKnown(const atoms::Base::sptr& base);

    /// Type of the pass
    enum PassType { Structural, Contextual };

    /**
     * @name Typedefs
     * @{ */
    typedef std::map<std::string, atoms::Object::sptr > CacheType;
    typedef std::map< atoms::Object::sptr, atoms::Object::sptr > NewVersionsType;
    /**  @} */

    /// Current type of pass during patching.
    PassType m_pass;

    /// Cache used to store objects which have already been patched (in order to avoid patching the same object twice).
    CacheType m_cache;

    /// Map used to store the correspondence between an old object and a new one
    NewVersionsType m_newVersions;

    /// Current object being patched.
    atoms::Object::sptr m_object;

    /// Context where the object is patched.
    std::string m_context;

    /// Current object version.
    ::fwAtomsPatch::VersionsGraph::NodeIDType m_currentVersion;

    /// Target version for current object.
    ::fwAtomsPatch::VersionsGraph::NodeIDType m_targetVersion;

    /// Versions graph.
    ::fwAtomsPatch::VersionsGraph::sptr m_versionsGraph;

};

} // namespace patcher

} // namespace fwAtomsPatch
