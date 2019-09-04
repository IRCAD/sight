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

#include "fwAtomsPatch/config.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtomsPatch/patcher/IPatcher.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwCore/base.hpp>

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

    fwCoreClassDefinitionsWithFactoryMacro((DefaultPatcher)(::fwAtomsPatch::patcher::IPatcher),
                                           ::fwAtomsPatch::patcher::factory::New< DefaultPatcher >);

    /// Constructor
    FWATOMSPATCH_API DefaultPatcher(::fwAtomsPatch::patcher::IPatcher::Key key);

    /// Destructor
    FWATOMSPATCH_API virtual ~DefaultPatcher();

    /// Apply the patch to the specified object
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr transformObject(::fwAtoms::Object::sptr object,
                                                                     const std::string& context,
                                                                     const std::string& currentVersion,
                                                                     const std::string& targetVersion) override;

protected:

    /// Process structural atom object.
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr processStructuralObject(::fwAtoms::Object::sptr current);

    /// Process contextual atom object.
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr processContextualObject(::fwAtoms::Object::sptr current);

    /// Process atom base.
    FWATOMSPATCH_API virtual ::fwAtoms::Base::sptr processBase(::fwAtoms::Base::sptr base);

    /// Process atom mapping.
    FWATOMSPATCH_API virtual ::fwAtoms::Map::sptr processMapping(::fwAtoms::Map::sptr map);

    /// Process atom sequence.
    FWATOMSPATCH_API virtual ::fwAtoms::Sequence::sptr processSequence(::fwAtoms::Sequence::sptr seq);

    /// Apply structural patch.
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr applyStructuralPatch(::fwAtoms::Object::sptr previous,
                                                                          ::fwAtoms::Object::sptr current);

    /// Apply contextual patch.
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr applyContextualPatch(::fwAtoms::Object::sptr previous,
                                                                          ::fwAtoms::Object::sptr current);
    /// Return true if the object is known in the target version
    FWATOMSPATCH_API bool isKnown(const ::fwAtoms::Base::sptr& base);

    /// Type of the pass
    enum PassType { Structural, Contextual };

    /**
     * @name Typedefs
     * @{ */
    typedef std::map<std::string, ::fwAtoms::Object::sptr > CacheType;
    typedef std::map< ::fwAtoms::Object::sptr, ::fwAtoms::Object::sptr > NewVersionsType;
    /**  @} */

    /// Current type of pass during patching.
    PassType m_pass;

    /// Cache used to store objects which have already been patched (in order to avoid patching the same object twice).
    CacheType m_cache;

    /// Map used to store the correspondence between an old object and a new one
    NewVersionsType m_newVersions;

    /// Current object being patched.
    ::fwAtoms::Object::sptr m_object;

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
