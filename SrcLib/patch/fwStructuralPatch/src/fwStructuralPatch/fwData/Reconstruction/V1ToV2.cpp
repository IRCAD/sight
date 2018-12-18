/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/String.hpp>

#include "fwStructuralPatch/fwData/Reconstruction/V1ToV2.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Reconstruction
{

V1ToV2::V1ToV2() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Reconstruction";
    m_targetClassname = "::fwData::Reconstruction";
    m_originVersion   = "1";
    m_targetVersion   = "2";

}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    helper.removeAttribute("reconstruction_format");
    helper.removeAttribute("is_closed");
    helper.removeAttribute("is_automatic");
    helper.removeAttribute("avg_volume");
    helper.removeAttribute("vol_deviation");
    helper.removeAttribute("vol_pct_confidence");
    helper.removeAttribute("reconstruction_time");
    helper.removeAttribute("mask_generated");
    helper.removeAttribute("level");
    helper.removeAttribute("label");
    helper.removeAttribute("generated_3D");
    helper.removeAttribute("type_3D");
    helper.removeAttribute("db_id");
}

} // namespace Reconstruction

} // namespace fwData

} // namespace fwStructuralPatch

