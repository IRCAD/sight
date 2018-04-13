/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
