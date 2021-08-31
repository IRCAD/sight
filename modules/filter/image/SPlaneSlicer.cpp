/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "SPlaneSlicer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Point.hpp>

#include <io/vtk/vtk.hpp>

#include <service/macros.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>

#include <algorithm>

namespace sight::module::filter::image
{

static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT    = "updateSliceType";
static const core::com::Slots::SlotKeyType s_UPDATE_DEFAULT_VALUE_SLOT = "updateDefaultValue";

//------------------------------------------------------------------------------

SPlaneSlicer::SPlaneSlicer() noexcept :
    m_orientation(data::helper::MedicalImage::Orientation::Z_AXIS),
    m_reslicer(vtkSmartPointer<vtkImageReslice>::New())
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SPlaneSlicer::updateSliceOrientation, this);
    newSlot(s_UPDATE_DEFAULT_VALUE_SLOT, &SPlaneSlicer::updateDefaultValue, this);
}

//------------------------------------------------------------------------------

SPlaneSlicer::~SPlaneSlicer() noexcept
{
}

//------------------------------------------------------------------------------

void SPlaneSlicer::starting()
{
    m_reslicer->SetOutputDimensionality(2);
    m_reslicer->SetInterpolationModeToLinear();

    updateDefaultValue();

    this->updating();
}

//------------------------------------------------------------------------------

void SPlaneSlicer::stopping()
{
}

//------------------------------------------------------------------------------

void SPlaneSlicer::updating()
{
    this->setReslicerExtent();
    this->setReslicerAxes();

    const auto image = m_image.lock();
    SIGHT_ASSERT("Cannot find " << s_IMAGE_IN, image);

    vtkSmartPointer<vtkImageData> vtkimg = vtkSmartPointer<vtkImageData>::New();

    io::vtk::toVTKImage(image.get_shared(), vtkimg.Get());

    m_reslicer->SetInputData(vtkimg);
    m_reslicer->Update();

    auto slice = m_slice.lock();
    SIGHT_ASSERT("Cannot find " << s_SLICE_INOUT, slice);

    io::vtk::fromVTKImage(m_reslicer->GetOutput(), slice.get_shared());

    // HACK: Make output slice three-dimensional.
    // We need to do so in order to visualize it with ::visuVTKAdaptor::SImageSlice.
    // This is because the adaptor uses a vtkImageActor which doesn't handle 2d images.
    const auto size = slice->getSize2();
    slice->setSize2({{size[0], size[1], 1}});
    const auto spacing = slice->getSpacing2();
    slice->setSpacing2({{spacing[0], spacing[1], 0}});
    const auto origin = slice->getOrigin2();
    slice->setOrigin2({{origin[0], origin[1], 0}});

    auto sig = slice->signal<data::Image::ModifiedSignalType>(data::Image::s_MODIFIED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SPlaneSlicer::configuring()
{
    const auto& srvConf = this->getConfigTree();

    const auto& config = srvConf.get_child("config.<xmlattr>");

    const std::string& orientation = config.get<std::string>("orientation", "");

    if(orientation == "axial")
    {
        m_orientation = data::helper::MedicalImage::Orientation::Z_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = data::helper::MedicalImage::Orientation::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = data::helper::MedicalImage::Orientation::Y_AXIS;
    }
    else
    {
        SIGHT_FATAL("Unknown orientation: '" + orientation + "'.");
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SPlaneSlicer::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_DEFAULT_VALUE_SLOT);
    connections.push(s_EXTENT_IN, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_EXTENT_IN, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_AXES_IN, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setReslicerExtent()
{
    const auto extentImg = m_extent.lock();

    SIGHT_ASSERT("No " << s_EXTENT_IN << " found", extentImg);

    const auto& size    = extentImg->getSize2();
    const auto& origin  = extentImg->getOrigin2();
    const auto& spacing = extentImg->getSpacing2();

    // cast size_t to int.
    std::vector<int> intSize(size.size());
    std::transform(
        size.begin(),
        size.end(),
        intSize.begin(),
        [](size_t s) -> int
        {
            return std::max(static_cast<int>(s) - 1, 0);
        });

    switch(m_orientation)
    {
        case data::helper::MedicalImage::Orientation::X_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[1], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[1], origin[2], origin[0]);
            m_reslicer->SetOutputSpacing(spacing[1], spacing[2], spacing[0]);
            break;

        case data::helper::MedicalImage::Orientation::Y_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[2], origin[1]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[2], spacing[1]);
            break;

        case data::helper::MedicalImage::Orientation::Z_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[1], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[1], origin[2]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
            break;
    }
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setReslicerAxes()
{
    const auto axes = m_axes.lock();

    SIGHT_ASSERT("No " << s_AXES_IN << " found.", axes);

    vtkSmartPointer<vtkMatrix4x4> axesMatrix = io::vtk::toVTKMatrix(axes.get_shared());

    this->applySliceTranslation(axesMatrix);

    // permutate axes.
    switch(m_orientation)
    {
        case data::helper::MedicalImage::Orientation::X_AXIS:
            // permutate X with Y and Y with Z
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                const double x = axesMatrix->GetElement(i, 0);
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 0, y);
                axesMatrix->SetElement(i, 1, z);
                axesMatrix->SetElement(i, 2, x);
            }

            break;

        case data::helper::MedicalImage::Orientation::Y_AXIS:
            // permutate Y with Z
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 1, z);
                axesMatrix->SetElement(i, 2, y);
            }

            break;

        case data::helper::MedicalImage::Orientation::Z_AXIS:
            break; // Nothing to do.
    }

    m_reslicer->SetResliceAxes(axesMatrix);
}

//------------------------------------------------------------------------------

void SPlaneSlicer::applySliceTranslation(vtkSmartPointer<vtkMatrix4x4> vtkMat) const
{
    const auto image = m_extent.lock();
    SIGHT_ASSERT("Cannot find " << s_EXTENT_IN, image);

    data::Object::sptr index;
    switch(m_orientation)
    {
        case data::helper::MedicalImage::Orientation::X_AXIS:
            index = image->getField(data::fieldHelper::Image::m_sagittalSliceIndexId);
            break;

        case data::helper::MedicalImage::Orientation::Y_AXIS:
            index = image->getField(data::fieldHelper::Image::m_frontalSliceIndexId);
            break;

        case data::helper::MedicalImage::Orientation::Z_AXIS:
            index = image->getField(data::fieldHelper::Image::m_axialSliceIndexId);
            break;
    }

    const int idx = data::Integer::dynamicCast(index)->value();

    const auto& spacing = image->getSpacing2();
    const auto& origin  = image->getOrigin2();

    const std::uint8_t axis = static_cast<std::uint8_t>(m_orientation);

    const double trans = spacing[axis] * static_cast<double>(idx) + origin[axis];

    vtkSmartPointer<vtkMatrix4x4> transMat = vtkSmartPointer<vtkMatrix4x4>::New();
    transMat->Identity();
    transMat->SetElement(axis, 3, trans);

    vtkMatrix4x4::Multiply4x4(vtkMat, transMat, vtkMat);
}

//------------------------------------------------------------------------------

void SPlaneSlicer::updateSliceOrientation(int from, int to)
{
    if(to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<data::helper::MedicalImage::Orientation>(from);
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<data::helper::MedicalImage::Orientation>(to);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SPlaneSlicer::updateDefaultValue()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("No " << s_IMAGE_IN << " found.", image);

    double min, max;
    data::fieldHelper::MedicalImageHelpers::getMinMax(image.get_shared(), min, max);

    m_reslicer->SetBackgroundLevel(min);
}

//------------------------------------------------------------------------------

} //namespace sight::module::filter::image
