/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1_HPP__

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Patient
{

/// Transforms a ::fwData::Patient to a ::fwMedData::Patient.
class FWSTRUCTURALPATCH_CLASS_API V1ToFwMedDataPatientV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToFwMedDataPatientV1)(::fwStructuralPatch::fwData::Patient::V1ToFwMedDataPatientV1),
        (()),
        new V1ToFwMedDataPatientV1);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataPatientV1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToFwMedDataPatientV1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataPatientV1( const V1ToFwMedDataPatientV1& cpy );

    /**
     * @brief Applies patch
     *
     * Merges 'firstname' and 'name' attributes, removes 'db_id' and 'studies', updates attribues.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Patient

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1_HPP__ */

