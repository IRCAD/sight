/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V1_V2_FWDATA_STUDY_HPP__
#define __FWMDSEMANTICPATCH_V1_V2_FWDATA_STUDY_HPP__

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtomsPatch/ISemanticPatch.hpp>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

/// Patch a study from version V'1' to 'V2' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Study : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Study)(::fwAtomsPatch::ISemanticPatch), (()), new Study);

    /// Constructor
    FWMDSEMANTICPATCH_API Study();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Study();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Study( const Study& cpy );

    /**
     * @brief Applies patch.
     *
     * Sets the 'instance_uid' attribute value to a newly generated UUID.
     */
    FWMDSEMANTICPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_V1_V2_FWDATA_STUDY_HPP__ */

