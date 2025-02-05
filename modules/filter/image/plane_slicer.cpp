/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "plane_slicer.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/point.hpp>

#include <geometry/__/line.hpp>
#include <geometry/data/matrix4.hpp>
#include <geometry/data/image.hpp>

#include <io/vtk/vtk.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

plane_slicer::plane_slicer() noexcept :
    filter(m_signals),
    m_reslicer(vtkSmartPointer<vtkImageReslice>::New())
{
    new_signal<signals::slice_range_changed_t>(signals::SLICE_RANGE_CHANGED);
    new_slot(slots::UPDATE_DEFAULT_VALUE, &plane_slicer::update_default_value, this);
}

//------------------------------------------------------------------------------

void plane_slicer::configuring()
{
}

//------------------------------------------------------------------------------

void plane_slicer::starting()
{
    m_reslicer->SetOutputDimensionality(2);
    m_reslicer->SetInterpolationModeToLinear();

    update_default_value();

    this->updating();
}

//------------------------------------------------------------------------------

void plane_slicer::stopping()
{
}

//------------------------------------------------------------------------------

void plane_slicer::updating()
{
    const auto image_in = m_image.lock();
    if(not sight::data::helper::medical_image::check_image_validity(*image_in))
    {
        return;
    }

    const auto axes     = m_axes.lock();
    auto reslice_matrix = std::make_shared<sight::data::matrix4>();

    // If we have an offset, add it to the reslice matrix
    const auto offset = m_offset.lock();
    if(offset)
    {
        sight::geometry::data::multiply(*axes, *offset, *reslice_matrix);
    }
    else
    {
        reslice_matrix->deep_copy(axes.get_shared());
    }

    const auto reslice_axes = std::make_shared<sight::data::matrix4>();
    {
        const auto origin      = image_in->origin();
        const auto orientation = image_in->orientation();

        // Get the orientation/position from original input image and reconstruct the pose
        sight::data::matrix4 image_pose_to_world_transform({
                orientation[0], orientation[1], orientation[2], origin[0],
                orientation[3], orientation[4], orientation[5], origin[1],
                orientation[6], orientation[7], orientation[8], origin[2],
                0., 0., 0., 1
            });
        // Compute the reslice by taking into account the image orientation
        sight::data::matrix4 world_to_image_pose_transform;
        sight::geometry::data::invert(image_pose_to_world_transform, world_to_image_pose_transform);

        sight::geometry::data::multiply(world_to_image_pose_transform, *reslice_matrix, *reslice_axes);
    }

    // Make a shallow-copied input image, centered at the origin for the resampling
    // This is due to the fact that VTK version prior to 9.2 do not take the data orientation into account.
    // Starting from VTK 9.2, we should be able to use SetOutputDirection rather than this trick.
    {
        auto image = std::make_shared<sight::data::image>();
        image->shallow_copy(image_in.get_shared());

        image->set_origin({0.0, 0.0, 0.0});
        image->set_orientation({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        auto vtk_img = vtkSmartPointer<vtkImageData>::New();
        io::vtk::to_vtk_image(image, vtk_img.Get());

        // Configure and perform the reslice
        vtkSmartPointer<vtkMatrix4x4> vtk_reslice_axes = io::vtk::to_vtk_matrix(reslice_axes);
        m_reslicer->SetResliceAxes(vtk_reslice_axes);
        m_reslicer->SetInputData(vtk_img);
        m_reslicer->SetOutputOrigin(0.0, 0.0, 0.0);
        m_reslicer->Update();
    }

    // Build the output slice
    {
        auto slice = m_slice.lock();

        sight::data::matrix4 output_slice_matrix;
        if(*m_center)
        {
            // To compute the reslice in the center, we reuse the slice computed from the bottom-left corner
            // to get the size of the slice, offset it to the center and compute it again
            // This is maybe not the smartest way, but since we do rely on VTK to perform the reslice
            // It could be difficult to try to redo the exact intersection computation
            auto* const bottom_left_slice       = m_reslicer->GetOutput();
            const auto* const size              = bottom_left_slice->GetDimensions();
            const auto* const spacing           = bottom_left_slice->GetSpacing();
            std::array<double, 2> center_offset =
            {
                -static_cast<double>(size[0]) * .5 * spacing[0],
                -static_cast<double>(size[1]) * .5 * spacing[1]
            };

            // The first "trick" is to offset the output origin to move the resampling to the center
            m_reslicer->SetOutputOrigin(center_offset[0], center_offset[1], 0.0);
            m_reslicer->Update();

            // However we will still need to apply this center offset to the slice location to move it physically
            // Thus the second trick apply the center offset to the origin and orientation of the slice
            sight::data::matrix4 center_matrix;
            center_matrix.set_position({center_offset[0], center_offset[1], 0.0});
            sight::geometry::data::multiply(*reslice_matrix, center_matrix, output_slice_matrix);
        }
        else
        {
            output_slice_matrix.deep_copy(reslice_matrix);
        }

        io::vtk::from_vtk_image(m_reslicer->GetOutput(), slice.get_shared());

        const auto    size = slice->size();
        slice->resize({{size[0], size[1], 1}}, slice->type(), slice->pixel_format());
        const auto    spacing = slice->spacing();
        slice->set_spacing({{spacing[0], spacing[1], 0}});

        // Position the slice at the position of the input matrix
        slice->set_origin(output_slice_matrix.position());
        slice->set_orientation(output_slice_matrix.orientation());

        slice->async_emit(data::image::MODIFIED_SIG);
    }

    // Compute the range so that we can slide on this axis
    // First we check at least one of the four edges of the slice intersect the image
    // Then, we compute the range trying to find the farthest planes of the eight corners of the image
    // Otherwise we reset the range
    {
        const auto size = image_in->size();

        auto slice            = m_slice.lock();
        const auto slice_size = slice->size();

        std::array<glm::dvec4, 4> slice_corners =
        {
            glm::dvec4(0., 0., 0., 1.),
            glm::dvec4(slice_size[0], 0, 0, 1.),
            glm::dvec4(slice_size[0], slice_size[1], 0, 1.),
            glm::dvec4(0., slice_size[1], 0., 1.)
        };

        const glm::dmat4 transform = geometry::data::image_to_world_transform(*slice);
        for(auto& slice_corner : slice_corners)
        {
            slice_corner = transform * slice_corner;
        }

        if(offset)
        {
            const glm::dmat4 inverse_offset = glm::inverse(geometry::data::to_glm_mat(*offset));
            // Do not take into account the offset for the intersection, go back to the reslice location if there
            // was no offset - beware it does not work if the offset contains a rotation !
            const glm::dmat3 slice_orientation = glm::transpose(glm::make_mat3(slice->orientation().data()));
            const glm::dmat4 rotate            = glm::dmat4(slice_orientation) * inverse_offset;
            const glm::dvec3 rotated_offset    = glm::dvec3(rotate[3][0], rotate[3][1], rotate[3][2]);
            const glm::dmat4 translate_offset  = glm::translate(glm::identity<glm::dmat4>(), rotated_offset);
            for(auto& slice_corner : slice_corners)
            {
                slice_corner = translate_offset * slice_corner;
            }
        }

        // Planes border + two diagonals
        std::array<geometry::line_t, 6> slice_edges =
        {
            geometry::line_t({slice_corners[0], slice_corners[1]}),
            geometry::line_t({slice_corners[1], slice_corners[2]}),
            geometry::line_t({slice_corners[2], slice_corners[3]}),
            geometry::line_t({slice_corners[3], slice_corners[0]}),
            geometry::line_t({slice_corners[0], slice_corners[2]}),
            geometry::line_t({slice_corners[1], slice_corners[3]})
        };

        const glm::dvec3 center = geometry::data::image_to_world(*image_in, {size[0] / 2, size[1] / 2, size[2] / 2});

        const auto spacing = image_in->spacing();
        const auto extent  = 0.5 * glm::dvec3(
            static_cast<double>(size[0]) * spacing[0],
            static_cast<double>(size[1]) * spacing[1],
            static_cast<double>(size[2]) * spacing[2]
        );

        const auto image_orientation = image_in->orientation();
        const glm::dmat3 orientation = glm::transpose(glm::make_mat3(image_orientation.data()));

        geometry::oriented_box_t image_box = {.center = center, .extent = extent, .orientation = orientation};

        bool does_plane_intersect_image = false;
        for(auto edge : slice_edges)
        {
            if(geometry::intersect_box(edge, image_box))
            {
                does_plane_intersect_image = true;
                break;
            }
        }

        if(does_plane_intersect_image)
        {
            std::array<glm::dvec3, 8> corners =
            {{
                {0., 0., 0.},
                {size[0], 0., 0.},
                {0., size[1], 0.},
                {0., 0., size[2]},
                {size[0], size[1], 0.},
                {size[0], 0., size[2]},
                {0., size[1], size[2]},
                {size[0], size[1], size[2]},
            }
            };

            // For each corner, we will compute the intersection with the planes oriented in the direction and in the
            // opposite direction to get the greatest distance
            const auto ray_origin = glm::make_vec3(axes->position().data());
            const glm::dvec3 ray_direction(axes->orientation()[2], axes->orientation()[5], axes->orientation()[8]);

            double t1_distance = std::numeric_limits<double>::min();
            double t2_distance = std::numeric_limits<double>::min();
            size_t t1_index    = ~0UL;
            size_t t2_index    = ~0UL;
            size_t i           = 0;

            const glm::dmat4 image_transform = geometry::data::image_to_world_transform(*image_in);
            for(auto corner : corners)
            {
                corner = image_transform * glm::dvec4(corner, 1.0);

                if(double dist = 0.; glm::intersectRayPlane(ray_origin, ray_direction, corner, ray_direction, dist))
                {
                    if(t1_distance < dist)
                    {
                        t1_distance = dist;
                        t1_index    = i;
                    }
                }

                if(double dist = 0.; glm::intersectRayPlane(ray_origin, -ray_direction, corner, ray_direction, dist))
                {
                    if(t2_distance < dist)
                    {
                        t2_distance = dist;
                        t2_index    = i;
                    }
                }

                ++i;
            }

            double min_range = 0.;
            double max_range = 0.;
            if(t1_index != ~0UL && t2_index != ~0UL)
            {
                min_range = std::min(t1_distance, -t2_distance);
                max_range = std::max(t1_distance, -t2_distance);
            }

            this->async_emit(signals::SLICE_RANGE_CHANGED, min_range, max_range);
        }
        else
        {
            this->async_emit(signals::SLICE_RANGE_CHANGED, 0.0, 0.0);
        }
    }
}

//------------------------------------------------------------------------------

service::connections_t plane_slicer::auto_connections() const
{
    return {
        {m_image, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {m_image, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {m_image, data::image::MODIFIED_SIG, slots::UPDATE_DEFAULT_VALUE},
        {m_image, data::image::BUFFER_MODIFIED_SIG, slots::UPDATE_DEFAULT_VALUE},
        {m_axes, data::matrix4::MODIFIED_SIG, service::slots::UPDATE},
        {m_offset, data::matrix4::MODIFIED_SIG, service::slots::UPDATE},
        {m_center, data::object::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void plane_slicer::update_default_value()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("No " << m_image.key() << " found.", image);

    const auto& [min, max] = data::helper::medical_image::get_min_max<double>(image.get_shared());

    m_reslicer->SetBackgroundLevel(min);
}

//------------------------------------------------------------------------------

} //namespace sight::module::filter::image
