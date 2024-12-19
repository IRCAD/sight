/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <io/vtk/vtk.hpp>

#include <service/macros.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>

#include <algorithm>
#include <cmath>

namespace sight::module::filter::image
{

static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT    = "updateSliceType";
static const core::com::slots::key_t UPDATE_DEFAULT_VALUE_SLOT = "updateDefaultValue";

//------------------------------------------------------------------------------

plane_slicer::plane_slicer() noexcept :
    filter(m_signals),
    m_reslicer(vtkSmartPointer<vtkImageReslice>::New())
{
    new_slot(UPDATE_SLICE_TYPE_SLOT, &plane_slicer::updateorientation_t, this);
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
    this->set_reslicer_extent();
    this->set_reslicer_axes();

    const auto image = m_image.lock();
    SIGHT_ASSERT("Cannot find " << IMAGE_IN, image);

    vtkSmartPointer<vtkImageData> vtk_img = vtkSmartPointer<vtkImageData>::New();

    io::vtk::to_vtk_image(image.get_shared(), vtk_img.Get());

    m_reslicer->SetInputData(vtk_img);
    m_reslicer->Update();

    auto slice = m_slice.lock();
    SIGHT_ASSERT("Cannot find " << SLICE_INOUT, slice);

    io::vtk::from_vtk_image(m_reslicer->GetOutput(), slice.get_shared());

    // HACK: Make output slice three-dimensional.
    // We need to do so in order to visualize it with ::visuVTKAdaptor::imageSlice.
    // This is because the adaptor uses a vtkImageActor which doesn't handle 2d images.
    const auto size = slice->size();
    slice->resize({{size[0], size[1], 1}}, slice->type(), slice->pixel_format());
    const auto spacing = slice->spacing();
    slice->set_spacing({{spacing[0], spacing[1], 0}});
    const auto origin = slice->origin();
    slice->set_origin({{origin[0], origin[1], 0}});

    auto sig = slice->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void plane_slicer::configuring()
{
    const auto& srv_conf = this->get_config();

    const auto& config = srv_conf.get_child("config.<xmlattr>");

    const std::string& orientation = config.get<std::string>("orientation", "");

    if(orientation == "axial")
    {
        m_orientation = data::helper::medical_image::axis_t::z_axis;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = data::helper::medical_image::axis_t::x_axis;
    }
    else if(orientation == "frontal")
    {
        m_orientation = data::helper::medical_image::axis_t::y_axis;
    }
    else
    {
        SIGHT_FATAL("Unknown orientation: '" + orientation + "'.");
    }
}

//------------------------------------------------------------------------------

service::connections_t plane_slicer::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_DEFAULT_VALUE_SLOT},
        {EXTENT_IN, data::image::SLICE_INDEX_MODIFIED_SIG, service::slots::UPDATE},
        {EXTENT_IN, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT},
        {AXES_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void plane_slicer::set_reslicer_extent()
{
    const auto extent_img = m_extent.lock();

    SIGHT_ASSERT("No " << EXTENT_IN << " found", extent_img);

    const auto& size    = extent_img->size();
    const auto& origin  = extent_img->origin();
    const auto& spacing = extent_img->spacing();

    // cast std::size_t to int.
    std::vector<int> int_size(size.size());
    std::transform(
        size.begin(),
        size.end(),
        int_size.begin(),
        [](std::size_t _s) -> int
        {
            return std::max(static_cast<int>(_s) - 1, 0);
        });

    switch(m_orientation)
    {
        case data::helper::medical_image::axis_t::x_axis:
            m_reslicer->SetOutputExtent(0, int_size[1], 0, int_size[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[1], origin[2], origin[0]);
            m_reslicer->SetOutputSpacing(spacing[1], spacing[2], spacing[0]);
            break;

        case data::helper::medical_image::axis_t::y_axis:
            m_reslicer->SetOutputExtent(0, int_size[0], 0, int_size[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[2], origin[1]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[2], spacing[1]);
            break;

        case data::helper::medical_image::axis_t::z_axis:
            m_reslicer->SetOutputExtent(0, int_size[0], 0, int_size[1], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[1], origin[2]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
            break;
    }
}

//------------------------------------------------------------------------------

void plane_slicer::set_reslicer_axes()
{
    const auto axes = m_axes.lock();

    SIGHT_ASSERT("No " << AXES_IN << " found.", axes);

    vtkSmartPointer<vtkMatrix4x4> axes_matrix = io::vtk::to_vtk_matrix(axes.get_shared());

    this->apply_slice_translation(axes_matrix);

    // permutate axes.
    switch(m_orientation)
    {
        case data::helper::medical_image::axis_t::x_axis:
            // permutate X with Y and Y with Z
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                const double x = axes_matrix->GetElement(i, 0);
                const double y = axes_matrix->GetElement(i, 1);
                const double z = axes_matrix->GetElement(i, 2);
                axes_matrix->SetElement(i, 0, y);
                axes_matrix->SetElement(i, 1, z);
                axes_matrix->SetElement(i, 2, x);
            }

            break;

        case data::helper::medical_image::axis_t::y_axis:
            // permutate Y with Z
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                const double y = axes_matrix->GetElement(i, 1);
                const double z = axes_matrix->GetElement(i, 2);
                axes_matrix->SetElement(i, 1, z);
                axes_matrix->SetElement(i, 2, y);
            }

            break;

        case data::helper::medical_image::axis_t::z_axis:
            break; // Nothing to do.
    }

    m_reslicer->SetResliceAxes(axes_matrix);
}

//------------------------------------------------------------------------------

void plane_slicer::apply_slice_translation(vtkSmartPointer<vtkMatrix4x4> _vtk_mat) const
{
    const auto image = m_extent.lock();
    SIGHT_ASSERT("Cannot find " << EXTENT_IN, image);

    std::int64_t idx = 0;
    switch(m_orientation)
    {
        case data::helper::medical_image::axis_t::x_axis:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::axis_t::sagittal
            ).value_or(0);
            break;

        case data::helper::medical_image::axis_t::y_axis:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::axis_t::frontal
            ).value_or(0);
            break;

        case data::helper::medical_image::axis_t::z_axis:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::axis_t::axial
            ).value_or(0);
            break;
    }

    const auto& spacing = image->spacing();
    const auto& origin  = image->origin();

    const auto axis = static_cast<std::uint8_t>(m_orientation);

    const double trans = spacing[axis] * static_cast<double>(idx) + origin[axis];

    vtkSmartPointer<vtkMatrix4x4> trans_mat = vtkSmartPointer<vtkMatrix4x4>::New();
    trans_mat->Identity();
    trans_mat->SetElement(axis, 3, trans);

    vtkMatrix4x4::Multiply4x4(_vtk_mat, trans_mat, _vtk_mat);
}

//------------------------------------------------------------------------------

void plane_slicer::updateorientation_t(int _from, int _to)
{
    if(_to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<data::helper::medical_image::axis_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<data::helper::medical_image::axis_t>(_to);
    }

    this->updating();
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
