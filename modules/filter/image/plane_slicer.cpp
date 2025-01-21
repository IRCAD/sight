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
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/point.hpp>

#include <geometry/data/matrix4.hpp>

#include <io/vtk/vtk.hpp>

#include <service/macros.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix3x3.h>

#include <algorithm>
#include <cmath>

namespace sight::module::filter::image
{

static const core::com::slots::key_t UPDATE_DEFAULT_VALUE_SLOT = "update_default_value";

//------------------------------------------------------------------------------

plane_slicer::plane_slicer() noexcept :
    filter(m_signals),
    m_reslicer(vtkSmartPointer<vtkImageReslice>::New())
{
    new_slot(UPDATE_DEFAULT_VALUE_SLOT, &plane_slicer::update_default_value, this);
}

//------------------------------------------------------------------------------

plane_slicer::~plane_slicer() noexcept =
    default;

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
    vtkSmartPointer<vtkImageData> vtk_img;

    const auto axes                           = m_axes.lock();
    vtkSmartPointer<vtkMatrix4x4> axes_matrix = io::vtk::to_vtk_matrix(axes.get_shared());

    // Get the orientation/position from original input image and reconstruct the pose
    const auto image_in = m_image.lock();

    vtkSmartPointer<vtkMatrix4x4> pose = vtkSmartPointer<vtkMatrix4x4>::New();
    {
        vtk_img = vtkSmartPointer<vtkImageData>::New();
        io::vtk::to_vtk_image(image_in.get_shared(), vtk_img.Get());

        std::array<double, 3> origin {};
        vtk_img->GetOrigin(origin.data());
        vtkMatrix3x3* mat3  = vtk_img->GetDirectionMatrix();
        double* orientation = mat3->GetData();

        pose->Identity();
        double* data = pose->GetData();

        data[3]  = origin[0];
        data[7]  = origin[1];
        data[11] = origin[2];

        data[0] = orientation[0];
        data[1] = orientation[1];
        data[2] = orientation[2];

        data[4] = orientation[3];
        data[5] = orientation[4];
        data[6] = orientation[5];

        data[8]  = orientation[6];
        data[9]  = orientation[7];
        data[10] = orientation[8];
    }

    // Make a shallow-copied input image, centered at the origin for the resampling
    // This is due to the fact that VTK version prior to 9.2 do not take the data orientation into account.
    // Starting from VTK 9.2, we should be able to use SetOutputDirection rather than this trick.
    auto image = std::make_shared<sight::data::image>();
    image->shallow_copy(image_in.get_shared());
    image->set_origin({0.0, 0.0, 0.0});
    image->set_orientation({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

    vtk_img = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_img.Get());

    // Build a matrix to perform the data resampling at the origin
    vtkSmartPointer<vtkMatrix4x4> inter        = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkMatrix4x4> reslice_axes = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Invert(pose, inter);
    vtkMatrix4x4::Multiply4x4(inter, axes_matrix, reslice_axes);

    // Configure and perform the reslice
    m_reslicer->SetResliceAxes(reslice_axes);
    m_reslicer->SetInputData(vtk_img);
    m_reslicer->SetOutputOrigin(0.0, 0.0, 0.0);
    m_reslicer->Update();

    // Build the output matrix
    vtkMatrix4x4::Multiply4x4(reslice_axes, pose, inter);
    std::shared_ptr<sight::data::matrix4> mat = std::make_shared<sight::data::matrix4>();
    io::vtk::from_vtk_matrix(inter, mat);

    // Build the output slice
    auto slice = m_slice.lock();
    io::vtk::from_vtk_image(m_reslicer->GetOutput(), slice.get_shared());

    const auto size = slice->size();
    slice->resize({{size[0], size[1], 1}}, slice->type(), slice->pixel_format());
    const auto spacing = slice->spacing();
    slice->set_spacing({{spacing[0], spacing[1], 0}});

    // Position the slice at the position of the input matrix
    slice->set_origin(axes->position());
    slice->set_orientation(axes->orientation());

    auto sig = slice->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void plane_slicer::configuring()
{
}

//------------------------------------------------------------------------------

service::connections_t plane_slicer::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_DEFAULT_VALUE_SLOT},
        {AXES_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void plane_slicer::update_default_value()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("No " << IMAGE_IN << " found.", image);

    const auto& [min, max] = data::helper::medical_image::get_min_max<double>(image.get_shared());

    m_reslicer->SetBackgroundLevel(min);
}

//------------------------------------------------------------------------------

} //namespace sight::module::filter::image
