/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_NAMESPACE_HPP__
#define __FWMDSEMANTICPATCH_NAMESPACE_HPP__

/**
 * @brief Contains the semantic patches within the context "MedicalData".
 * @namespace fwMDSemanticPatch
 */
namespace fwMDSemanticPatch
{

    /**
     * @brief Contains the semantic patches to transform atoms from version V1.
     */
    namespace V1
    {
        /**
         * @brief Contains the semantic patches to transform atoms from version V1 to version V2.
         */
        namespace V2
        {
            /**
             * @brief Contains the semantic patches to transform atoms (from fwData objects) from version V1 to version
             * V2.
             */
            namespace fwData
            {
            } // namespace fwData
        } // namespace V2
    } // namespace V1
} // namespace fwMDSemanticPatch

#endif /* __FWMDSEMANTICPATCH_NAMESPACE_HPP__ */
