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

#include "io/atoms/patch/patcher/DefaultPatcher.hpp"
#include "io/patch/config.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/base.hpp>

#include <string>

namespace sight::io::patch::semantic
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
class IO_PATCH_CLASS_API MedicalWorkspacePatcher : public io::atoms::patch::patcher::DefaultPatcher
{

public:

    fwCoreClassMacro(MedicalWorkspacePatcher, io::atoms::patch::patcher::DefaultPatcher,
                     io::atoms::patch::patcher::factory::New< MedicalWorkspacePatcher >)

    /// Constructor
    IO_PATCH_API MedicalWorkspacePatcher(io::atoms::patch::patcher::IPatcher::Key key);

    /// Destructor
    IO_PATCH_API virtual ~MedicalWorkspacePatcher();

    /**
     * @brief Applies the patch to the specified object
     *
     * Adds meta info 'compositeType' with value 'MedicalWorkspace' if current object has childs designated with keys
     * 'patientDB', 'planningDB' and 'processingDB', and applies patcher 'DefaultPatcher'.
     */
    IO_PATCH_API virtual sight::atoms::Object::sptr transformObject(sight::atoms::Object::sptr object,
                                                                    const std::string& context,
                                                                    const std::string& currentVersion,
                                                                    const std::string& targetVersion) override;

protected:

    /// Adds a meta info "compositeType" if the object is a composite with a special type.
    IO_PATCH_API void addCompositeTypes(sight::atoms::Object::sptr object);

};

} // namespace V2
} // namespace V1
} // namespace sight::io::patch::semantic
