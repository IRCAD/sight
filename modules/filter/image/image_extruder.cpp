/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include <data/helper/medical_image.hpp>
#include <data/reconstruction.hpp>

#include <filter/image/image_extruder.hpp>

namespace sight::module::filter::image
{

static const core::com::slots::key_t ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

//------------------------------------------------------------------------------

image_extruder::image_extruder() :
    filter(m_signals)
{
    new_slot(ADD_RECONSTRUCTIONS_SLOT, &image_extruder::add_reconstructions, this);
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
    connections.push(MESHES_INPUT, data::model_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(MESHES_INPUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, ADD_RECONSTRUCTIONS_SLOT);
    connections.push(MESHES_INPUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE);

    connections.push(IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void image_extruder::updating()
{
    const auto image = m_image.lock();

    if(data::helper::medical_image::check_image_validity(image.get_shared()))
    {
        // Initializes the mask
        {
            const auto image_out = m_extruded_image.lock();
            SIGHT_ASSERT("The image must be in 3 dimensions", image->num_dimensions() == 3);

            image_out->resize(image->size(), core::type::UINT8, data::image::pixel_format::gray_scale);
            image_out->set_spacing(image->spacing());
            image_out->set_origin(image->origin());
            std::fill(image_out->begin(), image_out->end(), std::uint8_t(255));
        }

        const auto meshes = m_meshes.lock();

        data::model_series::reconstruction_vector_t reconstructions = meshes->get_reconstruction_db();

        this->add_reconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void image_extruder::stopping()
{
}

//------------------------------------------------------------------------------

void image_extruder::add_reconstructions(data::model_series::reconstruction_vector_t _reconstructions) const
{
    const auto image_out = m_extruded_image.lock();

    if(data::helper::medical_image::check_image_validity(image_out.get_shared()))
    {
        for(const data::reconstruction::csptr reconstruction : _reconstructions)
        {
            data::mt::locked_ptr locked_reconstruction(reconstruction);

            const data::mesh::csptr mesh = locked_reconstruction->get_mesh();

            data::mt::locked_ptr locked_mesh(mesh);

            const auto transform = m_transform.lock();

            sight::filter::image::image_extruder::extrude(
                image_out.get_shared(),
                locked_mesh.get_shared(),
                transform.get_shared()
            );
        }
    }

    // Send signals.
    const auto sig = image_out->signal<data::image::buffer_modified_signal_t>(data::image::MODIFIED_SIG);
    sig->async_emit();

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
