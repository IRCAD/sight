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

#include "fwAtomsPatch/patcher/DefaultPatcher.hpp"

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwCore/base.hpp>

#include <string>

namespace fwMDSemanticPatch
{

namespace V1
{
namespace V2
{

/**
 * @brief This patcher adds required meta infos on composites describing a 'Medical workspace'.
 *
 * Composite describing a 'Medical workspace' contains childs designated by keys 'patientDB', 'planningDB' and
 * 'processingDB'.
 */
class FWMDSEMANTICPATCH_CLASS_API MedicalWorkspacePatcher : public ::fwAtomsPatch::patcher::DefaultPatcher
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (MedicalWorkspacePatcher)(::fwAtomsPatch::patcher::DefaultPatcher),
        ::fwAtomsPatch::patcher::factory::New< MedicalWorkspacePatcher >);

    /// Constructor
    FWMDSEMANTICPATCH_API MedicalWorkspacePatcher(::fwAtomsPatch::patcher::IPatcher::Key key);

    /// Destructor
    FWMDSEMANTICPATCH_API virtual ~MedicalWorkspacePatcher();

    /**
     * @brief Applies the patch to the specified object
     *
     * Adds meta info 'compositeType' with value 'MedicalWorkspace' if current object has childs designated with keys
     * 'patientDB', 'planningDB' and 'processingDB', and applies patcher 'DefaultPatcher'.
     */
    FWMDSEMANTICPATCH_API virtual ::fwAtoms::Object::sptr transformObject(::fwAtoms::Object::sptr object,
                                                                          const std::string& context,
                                                                          const std::string& currentVersion,
                                                                          const std::string& targetVersion) override;

protected:

    /// Adds a meta info "compositeType" if the object is a composite with a special type.
    FWMDSEMANTICPATCH_API void addCompositeTypes(::fwAtoms::Object::sptr object);

};

} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch
