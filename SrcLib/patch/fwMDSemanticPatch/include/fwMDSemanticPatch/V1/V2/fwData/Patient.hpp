/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V1_V2_FWDATA_PATIENT_HPP__
#define __FWMDSEMANTICPATCH_V1_V2_FWDATA_PATIENT_HPP__

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

/// Patch a patient from version 'V1' to 'V2' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Patient : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Patient)(::fwAtomsPatch::ISemanticPatch), (()), new Patient);

    /// Constructor
    FWMDSEMANTICPATCH_API Patient();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Patient();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Patient( const Patient& cpy );

    /**
     * @brief Applies patch.
     *
     * Sets the 'patient_id' attribute value to a newly generated UUID.
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

#endif // __FWMDSEMANTICPATCH_V1_V2_FWDATA_PATIENT_HPP__

