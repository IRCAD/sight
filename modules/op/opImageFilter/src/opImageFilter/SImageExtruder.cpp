/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "opImageFilter/SImageExtruder.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <imageFilterOp/ImageExtruder.hpp>

namespace opImageFilter
{

static const std::string s_MESHES_INPUT = "meshes";
static const std::string s_IMAGE_INPUT  = "image";

static const std::string s_IMAGE_INOUT = "extrudedImage";

static const ::fwCom::Slots::SlotKeyType s_ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

//------------------------------------------------------------------------------

SImageExtruder::SImageExtruder()
{
    newSlot(s_ADD_RECONSTRUCTIONS_SLOT, &SImageExtruder::addReconstructions, this);
}

//------------------------------------------------------------------------------

SImageExtruder::~SImageExtruder()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::configuring()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::starting()
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageExtruder::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_ADD_RECONSTRUCTIONS_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SImageExtruder::updating()
{
    const auto image = this->getLockedInput< ::fwData::Image >(s_IMAGE_INPUT);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
    {
        // Copy the image into the output.
        {
            const auto imageOut = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("The image must be in 3 dimensions", image->getNumberOfDimensions() == 3);

            imageOut->deepCopy(image.get_shared());

            const auto sig = imageOut->signal< ::fwData::Image::ModifiedSignalType>(::fwData::Image::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        const auto meshes = this->getLockedInput< ::fwMedData::ModelSeries >(s_MESHES_INPUT);

        ::fwMedData::ModelSeries::ReconstructionVectorType reconstructions = meshes->getReconstructionDB();

        this->addReconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::stopping()
{
}

//------------------------------------------------------------------------------

void SImageExtruder::addReconstructions(::fwMedData::ModelSeries::ReconstructionVectorType _reconstructions) const
{
    const auto imageOut = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(imageOut.get_shared()))
    {
        for(const ::fwData::Reconstruction::csptr reconstruction : _reconstructions)
        {
            ::fwData::mt::locked_ptr lockedReconstruction(reconstruction);

            const ::fwData::Mesh::csptr mesh = lockedReconstruction->getMesh();

            ::fwData::mt::locked_ptr lockedMesh(mesh);

            this->extrudeMesh(lockedMesh.get_shared(), imageOut.get_shared());
        }
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::extrudeMesh(const ::fwData::Mesh::csptr _mesh, const fwData::Image::sptr _image) const
{
    // Extrude the image.
    ::imageFilterOp::ImageExtruder::extrude(_image, _mesh);

    // Send signals.
    const auto sig =
        _image->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

} // namespace opImageFilter.
