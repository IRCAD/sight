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
#include "io/atoms/patch/patcher/IPatcher.hpp"
#include "io/atoms/patch/VersionDescriptor.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/base.hpp>

#include <string>

namespace sight::io::atoms::patch
{

namespace patcher
{

/**
 * @brief Default patcher
 */
class IO_ATOMS_CLASS_API DefaultPatcher : public IPatcher
{
public:

    SIGHT_DECLARE_CLASS(
        DefaultPatcher,
        io::atoms::patch::patcher::IPatcher,
        io::atoms::patch::patcher::factory::New<DefaultPatcher>
    )

    /// Constructor
    IO_ATOMS_API DefaultPatcher(io::atoms::patch::patcher::IPatcher::Key key);

    /// Destructor
    IO_ATOMS_API virtual ~DefaultPatcher();

    /// Apply the patch to the specified object
    IO_ATOMS_API virtual sight::atoms::Object::sptr transformObject(
        sight::atoms::Object::sptr object,
        const std::string& context,
        const std::string& currentVersion,
        const std::string& targetVersion
    ) override;

protected:

    /// Process structural atom object.
    IO_ATOMS_API virtual sight::atoms::Object::sptr processStructuralObject(sight::atoms::Object::sptr current);

    /// Process contextual atom object.
    IO_ATOMS_API virtual sight::atoms::Object::sptr processContextualObject(sight::atoms::Object::sptr current);

    /// Process atom base.
    IO_ATOMS_API virtual sight::atoms::Base::sptr processBase(sight::atoms::Base::sptr base);

    /// Process atom mapping.
    IO_ATOMS_API virtual sight::atoms::Map::sptr processMapping(sight::atoms::Map::sptr map);

    /// Process atom sequence.
    IO_ATOMS_API virtual sight::atoms::Sequence::sptr processSequence(sight::atoms::Sequence::sptr seq);

    /// Apply structural patch.
    IO_ATOMS_API virtual sight::atoms::Object::sptr applyStructuralPatch(
        sight::atoms::Object::sptr previous,
        sight::atoms::Object::sptr current
    );

    /// Apply contextual patch.
    IO_ATOMS_API virtual sight::atoms::Object::sptr applyContextualPatch(
        sight::atoms::Object::sptr previous,
        sight::atoms::Object::sptr current
    );
    /// Return true if the object is known in the target version
    IO_ATOMS_API bool isKnown(const sight::atoms::Base::sptr& base);

    /// Type of the pass
    enum PassType
    {
        Structural,
        Contextual
    };

    /**
     * @name Typedefs
     * @{ */
    typedef std::map<std::string, sight::atoms::Object::sptr> CacheType;
    typedef std::map<sight::atoms::Object::sptr, sight::atoms::Object::sptr> NewVersionsType;
    /**  @} */

    /// Current type of pass during patching.
    PassType m_pass;

    /// Cache used to store objects which have already been patched (in order to avoid patching the same object twice).
    CacheType m_cache;

    /// Map used to store the correspondence between an old object and a new one
    NewVersionsType m_newVersions;

    /// Current object being patched.
    sight::atoms::Object::sptr m_object;

    /// Context where the object is patched.
    std::string m_context;

    /// Current object version.
    io::atoms::patch::VersionsGraph::NodeIDType m_currentVersion;

    /// Target version for current object.
    io::atoms::patch::VersionsGraph::NodeIDType m_targetVersion;

    /// Versions graph.
    io::atoms::patch::VersionsGraph::sptr m_versionsGraph;
};

} // namespace patcher

} // namespace sight::io::atoms::patch
