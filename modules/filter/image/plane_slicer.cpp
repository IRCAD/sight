/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

    m_reslicer(vtkSmartPointer<vtkImageReslice>::New())
{
    new_slot(UPDATE_SLICE_TYPE_SLOT, &plane_slicer::updateorientation_t, this);
    new_slot(UPDATE_DEFAULT_VALUE_SLOT, &plane_slicer::updateDefaultValue, this);
}

//------------------------------------------------------------------------------

plane_slicer::~plane_slicer() noexcept =
    default;

//------------------------------------------------------------------------------

void plane_slicer::starting()
{
    m_reslicer->SetOutputDimensionality(2);
    m_reslicer->SetInterpolationModeToLinear();

    updateDefaultValue();

    this->updating();
}

//------------------------------------------------------------------------------

void plane_slicer::stopping()
{
}

//------------------------------------------------------------------------------

void plane_slicer::updating()
{
    this->setReslicerExtent();
    this->setReslicerAxes();

    const auto image = m_image.lock();
    SIGHT_ASSERT("Cannot find " << s_IMAGE_IN, image);

    vtkSmartPointer<vtkImageData> vtk_img = vtkSmartPointer<vtkImageData>::New();

    io::vtk::to_vtk_image(image.get_shared(), vtk_img.Get());

    m_reslicer->SetInputData(vtk_img);
    m_reslicer->Update();

    auto slice = m_slice.lock();
    SIGHT_ASSERT("Cannot find " << s_SLICE_INOUT, slice);

    io::vtk::from_vtk_image(m_reslicer->GetOutput(), slice.get_shared());

    // HACK: Make output slice three-dimensional.
    // We need to do so in order to visualize it with ::visuVTKAdaptor::imageSlice.
    // This is because the adaptor uses a vtkImageActor which doesn't handle 2d images.
    const auto size = slice->size();
    slice->resize({{size[0], size[1], 1}}, slice->getType(), slice->getPixelFormat());
    const auto spacing = slice->getSpacing();
    slice->setSpacing({{spacing[0], spacing[1], 0}});
    const auto origin = slice->getOrigin();
    slice->setOrigin({{origin[0], origin[1], 0}});

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
        m_orientation = data::helper::medical_image::orientation_t::Z_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = data::helper::medical_image::orientation_t::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = data::helper::medical_image::orientation_t::Y_AXIS;
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
        {s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_DEFAULT_VALUE_SLOT},
        {s_EXTENT_IN, data::image::SLICE_INDEX_MODIFIED_SIG, service::slots::UPDATE},
        {s_EXTENT_IN, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT},
        {s_AXES_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void plane_slicer::setReslicerExtent()
{
    const auto extent_img = m_extent.lock();

    SIGHT_ASSERT("No " << s_EXTENT_IN << " found", extent_img);

    const auto& size    = extent_img->size();
    const auto& origin  = extent_img->getOrigin();
    const auto& spacing = extent_img->getSpacing();

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
        case data::helper::medical_image::orientation_t::X_AXIS:
            m_reslicer->SetOutputExtent(0, int_size[1], 0, int_size[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[1], origin[2], origin[0]);
            m_reslicer->SetOutputSpacing(spacing[1], spacing[2], spacing[0]);
            break;

        case data::helper::medical_image::orientation_t::Y_AXIS:
            m_reslicer->SetOutputExtent(0, int_size[0], 0, int_size[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[2], origin[1]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[2], spacing[1]);
            break;

        case data::helper::medical_image::orientation_t::Z_AXIS:
            m_reslicer->SetOutputExtent(0, int_size[0], 0, int_size[1], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[1], origin[2]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
            break;
    }
}

//------------------------------------------------------------------------------

void plane_slicer::setReslicerAxes()
{
    const auto axes = m_axes.lock();

    SIGHT_ASSERT("No " << s_AXES_IN << " found.", axes);

    vtkSmartPointer<vtkMatrix4x4> axes_matrix = io::vtk::to_vtk_matrix(axes.get_shared());

    this->applySliceTranslation(axes_matrix);

    // permutate axes.
    switch(m_orientation)
    {
        case data::helper::medical_image::orientation_t::X_AXIS:
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

        case data::helper::medical_image::orientation_t::Y_AXIS:
            // permutate Y with Z
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                const double y = axes_matrix->GetElement(i, 1);
                const double z = axes_matrix->GetElement(i, 2);
                axes_matrix->SetElement(i, 1, z);
                axes_matrix->SetElement(i, 2, y);
            }

            break;

        case data::helper::medical_image::orientation_t::Z_AXIS:
            break; // Nothing to do.
    }

    m_reslicer->SetResliceAxes(axes_matrix);
}

//------------------------------------------------------------------------------

void plane_slicer::applySliceTranslation(vtkSmartPointer<vtkMatrix4x4> _vtk_mat) const
{
    const auto image = m_extent.lock();
    SIGHT_ASSERT("Cannot find " << s_EXTENT_IN, image);

    std::int64_t idx = 0;
    switch(m_orientation)
    {
        case data::helper::medical_image::orientation_t::X_AXIS:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::orientation_t::SAGITTAL
            ).value_or(0);
            break;

        case data::helper::medical_image::orientation_t::Y_AXIS:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::orientation_t::FRONTAL
            ).value_or(0);
            break;

        case data::helper::medical_image::orientation_t::Z_AXIS:
            idx = data::helper::medical_image::get_slice_index(
                *image,
                data::helper::medical_image::orientation_t::AXIAL
            ).value_or(0);
            break;
    }

    const auto& spacing = image->getSpacing();
    const auto& origin  = image->getOrigin();

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
        m_orientation = static_cast<data::helper::medical_image::orientation_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<data::helper::medical_image::orientation_t>(_to);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void plane_slicer::updateDefaultValue()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("No " << s_IMAGE_IN << " found.", image);

    double min = NAN;
    double max = NAN;
    data::helper::medical_image::get_min_max(image.get_shared(), min, max);

    m_reslicer->SetBackgroundLevel(min);
}

//------------------------------------------------------------------------------

} //namespace sight::module::filter::image
