/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "modules/filter/image/SImageExtruder.hpp"

#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Reconstruction.hpp>

#include <filter/image/ImageExtruder.hpp>

namespace sight::module::filter::image
{

static const core::com::Slots::SlotKeyType s_ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

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

service::IService::KeyConnectionsMap SImageExtruder::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MESHES_INPUT, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESHES_INPUT, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_ADD_RECONSTRUCTIONS_SLOT);
    connections.push(s_MESHES_INPUT, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

    connections.push(s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SImageExtruder::updating()
{
    const auto image = m_image.lock();

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        // Copy the image into the output.
        {
            const auto imageOut = m_extrudedImage.lock();
            SIGHT_ASSERT("The image must be in 3 dimensions", image->numDimensions() == 3);

            imageOut->deepCopy(image.get_shared());

            const auto sig = imageOut->signal<data::Image::ModifiedSignalType>(data::Image::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        const auto meshes = m_meshes.lock();

        data::ModelSeries::ReconstructionVectorType reconstructions = meshes->getReconstructionDB();

        this->addReconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::stopping()
{
}

//------------------------------------------------------------------------------

void SImageExtruder::addReconstructions(data::ModelSeries::ReconstructionVectorType _reconstructions) const
{
    const auto imageOut = m_extrudedImage.lock();

    if(data::helper::MedicalImage::checkImageValidity(imageOut.get_shared()))
    {
        for(const data::Reconstruction::csptr reconstruction : _reconstructions)
        {
            data::mt::locked_ptr lockedReconstruction(reconstruction);

            const data::Mesh::csptr mesh = lockedReconstruction->getMesh();

            data::mt::locked_ptr lockedMesh(mesh);

            this->extrudeMesh(lockedMesh.get_shared(), imageOut.get_shared());
        }
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::extrudeMesh(const data::Mesh::csptr _mesh, const data::Image::sptr _image) const
{
    // Extrude the image.
    sight::filter::image::ImageExtruder::extrude(_image, _mesh);

    // Send signals.
    const auto sig =
        _image->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

} // namespace sight::module::filter::image.
