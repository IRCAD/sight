/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_NAMESPACE_HPP__
#define __FWSTRUCTURALPATCH_NAMESPACE_HPP__

/**
 * @brief Contains creators and structural patches.
 * @namespace fwStructuralPatch
 */
namespace fwStructuralPatch
{

    /**
     * @brief Contains creators (implementation if ::fwAtomsPatch::IStructuralCreator).
     * @namespace fwStructuralPatch::creator
     */
    namespace creator
    {

        /**
         * @brief Contains creators for fwMedData.
         * @namespace fwStructuralPatch::creator::fwMedData
         */
        namespace fwMedData
        {
        }
    } // namespace creator

    /// Contains the structural patch (::fwAtomsPatch::IStructuralPatch) to tranforms atoms from fwData objects.
    namespace fwData
    {
        /// Contains the structural patch to transform atoms from fwData::Acquisition.
        namespace Acquisition
        {
        }

        /// Contains the structural patch to transform atoms from fwData::Patient.
        namespace Patient
        {
        }

        /// Contains the structural patch to transform atoms from fwData::Reconstruction.
        namespace Reconstruction
        {
        }

        /// Contains the structural patch to transform atoms from fwData::Study.
        namespace Study
        {
        }
    } // namespace fwData

} // namespace fwStructuralPatch

#endif // __FWSTRUCTURALPATCH_NAMESPACE_HPP__
