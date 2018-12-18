/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWSTRUCTURALPATCH_FWDATA_RECONSTRUCTION_V2TOV3_HPP__
#define __FWSTRUCTURALPATCH_FWDATA_RECONSTRUCTION_V2TOV3_HPP__

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace fwData
{

namespace Reconstruction
{

/// Structural patch to convert a fwData::Reconstruction from version '2' to version '3'.
class FWSTRUCTURALPATCH_CLASS_API V2ToV3 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (V2ToV3)(::fwStructuralPatch::fwData::Reconstruction::V2ToV3), (()), new V2ToV3);

    /// Constructor
    FWSTRUCTURALPATCH_API V2ToV3();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V2ToV3();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V2ToV3( const V2ToV3& cpy );

    /**
     * @brief Applies patch
     *
     * Removes attributes from reconstruction.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Reconstruction

} // namespace fwData

} // namespace fwStructuralPatch

#endif /* __FWSTRUCTURALPATCH_FWDATA_RECONSTRUCTION_V2TOV3_HPP__ */

