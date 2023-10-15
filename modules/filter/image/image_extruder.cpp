/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/filter/image/image_extruder.hpp"

#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/reconstruction.hpp>

#include <filter/image/image_extruder.hpp>

namespace sight::module::filter::image
{

static const core::com::slots::key_t ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

//------------------------------------------------------------------------------

image_extruder::image_extruder()
{
    new_slot(ADD_RECONSTRUCTIONS_SLOT, &image_extruder::addReconstructions, this);
}

//------------------------------------------------------------------------------

image_extruder::~image_extruder()
= default;

//------------------------------------------------------------------------------

void image_extruder::configuring()
{
}

//------------------------------------------------------------------------------

void image_extruder::starting()
{
}

//------------------------------------------------------------------------------

service::connections_t image_extruder::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MESHES_INPUT, data::model_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MESHES_INPUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, ADD_RECONSTRUCTIONS_SLOT);
    connections.push(s_MESHES_INPUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE);

    connections.push(s_IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void image_extruder::updating()
{
    const auto image = m_image.lock();

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        // Initializes the mask
        {
            const auto imageOut = m_extrudedImage.lock();
            SIGHT_ASSERT("The image must be in 3 dimensions", image->numDimensions() == 3);

            imageOut->resize(image->size(), core::type::UINT8, data::image::PixelFormat::GRAY_SCALE);
            imageOut->setSpacing(image->getSpacing());
            imageOut->setOrigin(image->getOrigin());
            std::fill(imageOut->begin(), imageOut->end(), std::uint8_t(255));
        }

        const auto meshes = m_meshes.lock();

        data::model_series::ReconstructionVectorType reconstructions = meshes->getReconstructionDB();

        this->addReconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void image_extruder::stopping()
{
}

//------------------------------------------------------------------------------

void image_extruder::addReconstructions(data::model_series::ReconstructionVectorType _reconstructions) const
{
    const auto imageOut = m_extrudedImage.lock();

    if(data::helper::MedicalImage::checkImageValidity(imageOut.get_shared()))
    {
        for(const data::reconstruction::csptr reconstruction : _reconstructions)
        {
            data::mt::locked_ptr lockedReconstruction(reconstruction);

            const data::mesh::csptr mesh = lockedReconstruction->getMesh();

            data::mt::locked_ptr lockedMesh(mesh);

            const auto transform = m_transform.lock();

            sight::filter::image::image_extruder::extrude(
                imageOut.get_shared(),
                lockedMesh.get_shared(),
                transform.get_shared()
            );
        }
    }

    // Send signals.
    const auto sig = imageOut->signal<data::image::BufferModifiedSignalType>(data::image::MODIFIED_SIG);
    sig->async_emit();

    m_sigComputed->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
