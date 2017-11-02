/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_V2_V03AGO_FWDATA_IMAGE_HPP__
#define __FWMDSEMANTICPATCH_V2_V03AGO_FWDATA_IMAGE_HPP__

#include "fwMDSemanticPatch/config.hpp"

#include <fwAtomsPatch/ISemanticPatch.hpp>

namespace fwMDSemanticPatch
{
namespace V2
{
namespace V03AGO
{
namespace fwData
{

/// Patch an image from version 'V2' to 'V03AGO' within 'MedicalData' context.
class FWMDSEMANTICPATCH_CLASS_API Image : public ::fwAtomsPatch::ISemanticPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Image)(::fwAtomsPatch::ISemanticPatch), (()), new Image);

    /// Constructor
    FWMDSEMANTICPATCH_API Image();

    /// Destructor
    FWMDSEMANTICPATCH_API ~Image();

    /// Copy constructor
    FWMDSEMANTICPATCH_API Image( const Image& cpy );

    /**
     * @brief Applies patch.
     *
     * Sets the 'nb_components', 'window_center' and 'window_width' attributes to a newly generated UUID.
     */
    FWMDSEMANTICPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace fwData
} // namespace V03AGO
} // namespace V2
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_V2_V03AGO_FWDATA_IMAGE_HPP__ */

