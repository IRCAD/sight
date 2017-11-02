/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_FWDATA_STUDY_V1TOFWMEDDATASTUDYV1_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_STUDY_V1TOFWMEDDATASTUDYV1_HPP__

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Study
{

/// Transforms a ::fwData::Study to a ::fwMedData::Study.
class FWSTRUCTURALPATCH_CLASS_API V1ToFwMedDataStudyV1 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V1ToFwMedDataStudyV1)(::fwStructuralPatch::fwData::Study::V1ToFwMedDataStudyV1),
        (()),
        new V1ToFwMedDataStudyV1);

    /// Constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataStudyV1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V1ToFwMedDataStudyV1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V1ToFwMedDataStudyV1( const V1ToFwMedDataStudyV1& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes 'hospital', 'modality', 'acquisition_zone', 'ris_id', 'db_id' and 'acquisitions', renames
     * 'uid' to 'instance_uid', adds 'referring_physicians_name' and 'patient_age'.
     */
    FWSTRUCTURALPATCH_API virtual void apply(const ::fwAtoms::Object::sptr& previous,
                                             const ::fwAtoms::Object::sptr& current,
                                             ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Study

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_STUDY_V1TOFWMEDDATASTUDYV1_HPP__ */

