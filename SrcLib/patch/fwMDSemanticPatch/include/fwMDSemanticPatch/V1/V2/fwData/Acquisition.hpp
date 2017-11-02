/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V1_V2_FWDATA_ACQUISITION_HPP__
#define __FWMDSEMANTICPATCH_V1_V2_FWDATA_ACQUISITION_HPP__

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

/// Patch an acquisition from version 'V1' to 'V2' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Acquisition : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Acquisition)(::fwAtomsPatch::ISemanticPatch), (()), new Acquisition);

    /// Constructor
    FWMDSEMANTICPATCH_API Acquisition();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Acquisition();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Acquisition( const Acquisition& cpy );

    /**
     * @brief Applies patch
     *
     * Retrieves 'comment' field stored into acquisition's image and moves it into current object 'description'
     * attribute.
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

#endif // __FWMDSEMANTICPATCH_V1_V2_FWDATA_ACQUISITION_HPP__

