/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

/// Contains creators and structural patches.
namespace fwStructuralPatch
{

/// Contains creators (implementation if ::fwAtomsPatch::IStructuralCreator).
namespace creator
{

/// Contains creators for fwMedData.
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

/// Contains the structural patch (::fwAtomsPatch::IStructuralPatch) to tranforms atoms from fwMedData objects.
namespace fwMedData
{
/// Contains the structural patch to transform atoms from fwMedData::ImageSeries.
namespace ImageSeries
{
}
/// Contains the structural patch to transform atoms from fwMedData::ModelSeries.
namespace ModelSeries
{
}
} // namespace fwMedData

} // namespace fwStructuralPatch
