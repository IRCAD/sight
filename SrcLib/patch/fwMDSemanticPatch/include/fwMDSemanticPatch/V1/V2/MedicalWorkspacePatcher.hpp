/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V1_V2_MEDICALWORKSPACEPATCHER_HPP__
#define __FWMDSEMANTICPATCH_V1_V2_MEDICALWORKSPACEPATCHER_HPP__

#include <string>

#include <fwCore/base.hpp>
#include <fwAtoms/Object.hpp>

#include <fwAtoms/String.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Sequence.hpp>

#include "fwAtomsPatch/patcher/DefaultPatcher.hpp"
#include "fwMDSemanticPatch/config.hpp"


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
        (MedicalWorkspacePatcher)(::fwAtomsPatch::patcher::DefaultPatcher), (()),
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
                                                                          const std::string& targetVersion);

protected:

    /// Adds a meta info "compositeType" if the object is a composite with a special type.
    FWMDSEMANTICPATCH_API void addCompositeTypes(::fwAtoms::Object::sptr object);

};

} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_V1_V2_MEDICALWORKSPACEPATCHER_HPP__ */

