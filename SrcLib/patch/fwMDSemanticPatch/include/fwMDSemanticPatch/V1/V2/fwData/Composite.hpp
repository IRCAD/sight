/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V1_V2_FWDATA_COMPOSITE_HPP__
#define __FWMDSEMANTICPATCH_V1_V2_FWDATA_COMPOSITE_HPP__

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

/// Patch the composite describing a 'Medical workspace' from version 'V1' to 'V2' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Composite : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Composite)(::fwAtomsPatch::ISemanticPatch), (()), new Composite);

    /// Constructor
    FWMDSEMANTICPATCH_API Composite();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Composite();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Composite( const Composite& cpy );

    /**
     * @brief Applies patch
     *
     * If the related composite describes a 'Medical workspace' (i.e has value 'MedicalWorkspace' for 'compositeType'
     * meta info), then converts the composite to a ::fwMedData::SeriesDB and insert model series built from previous
     * acquisitions.
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

#endif /* __FWMDSEMANTICPATCH_V1_V2_FWDATA_COMPOSITE_HPP__ */

