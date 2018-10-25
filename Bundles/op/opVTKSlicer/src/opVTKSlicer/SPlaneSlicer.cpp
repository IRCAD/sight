/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opVTKSlicer/SPlaneSlicer.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>

#include <algorithm>

fwServicesRegisterMacro( ::fwServices::IOperator, ::opVTKSlicer::SPlaneSlicer );

namespace opVTKSlicer
{

static const ::fwServices::IService::KeyType s_IMAGE_IN  = "image";
static const ::fwServices::IService::KeyType s_EXTENT_IN = "imageExtent";
static const ::fwServices::IService::KeyType s_AXES_IN   = "axes";
static const ::fwServices::IService::KeyType s_SLICE_OUT = "slice";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT    = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_DEFAULT_VALUE_SLOT = "updateDefaultValue";

//------------------------------------------------------------------------------

SPlaneSlicer::SPlaneSlicer() noexcept :
    m_orientation(::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS),
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

    auto image                           = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    vtkSmartPointer<vtkImageData> vtkimg = vtkSmartPointer<vtkImageData>::New();

    ::fwVtkIO::toVTKImage(image, vtkimg.Get());

    m_reslicer->SetInputData(vtkimg);
    m_reslicer->Update();

    auto slice = this->getInOut< ::fwData::Image >(s_SLICE_OUT);

    ::fwVtkIO::fromVTKImage(m_reslicer->GetOutput(), slice);

    // HACK: Make output slice three-dimensional.
    // We need to do so in order to visualize it with ::visuVTKAdaptor::SImageSlice.
    // This is because the adaptor uses a vtkImageActor which doesn't handle 2d images.
    auto size = slice->getSize();
    slice->setSize({{size[0], size[1], 1}});
    auto spacing = slice->getSpacing();
    slice->setSpacing({{spacing[0], spacing[1], 0 }});
    auto origin = slice->getOrigin();
    slice->setOrigin({{origin[0], origin[1], 0}});

    auto sig = slice->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG);

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
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS;
    }
    else
    {
        SLM_FATAL("Unknown orientation: '" + orientation + "'.");
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPlaneSlicer::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_DEFAULT_VALUE_SLOT);
    connections.push(s_EXTENT_IN, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_EXTENT_IN, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_AXES_IN, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setReslicerExtent()
{
    ::fwData::Image::csptr extentImg = this->getInput< ::fwData::Image >(s_EXTENT_IN);

    SLM_ASSERT("No extentImg.", extentImg);

    const auto& size    = extentImg->getSize();
    const auto& origin  = extentImg->getOrigin();
    const auto& spacing = extentImg->getSpacing();

    // cast size_t to int.
    std::vector<int> intSize(size.size());
    std::transform(size.begin(), size.end(), intSize.begin(), [](size_t s) -> int
        {
            return std::max(static_cast<int>(s) - 1, 0);
        });

    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[1], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[1], origin[2], origin[0]);
            m_reslicer->SetOutputSpacing(spacing[1], spacing[2], spacing[0]);
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[2], origin[1]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[2], spacing[1]);
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[1], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[1], origin[2]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
            break;
    }
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setReslicerAxes()
{
    ::fwData::TransformationMatrix3D::csptr axes = this->getInput< ::fwData::TransformationMatrix3D>(s_AXES_IN);

    SLM_ASSERT("No axes found.", axes);

    vtkSmartPointer<vtkMatrix4x4> axesMatrix = ::fwVtkIO::toVTKMatrix(axes);

    this->applySliceTranslation(axesMatrix);

    // permutate axes.
    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
            // permutate X with Y and Y with Z
            for(std::uint8_t i = 0; i < 4; ++i)
            {
                const double x = axesMatrix->GetElement(i, 0);
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 0, y);
                axesMatrix->SetElement(i, 1, z);
                axesMatrix->SetElement(i, 2, x);
            }
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
            // permutate Y with Z
            for(std::uint8_t i = 0; i < 4; ++i)
            {
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 1, z);
                axesMatrix->SetElement(i, 2, y);
            }
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS: break; // Nothing to do.
    }

    m_reslicer->SetResliceAxes(axesMatrix);
}

//------------------------------------------------------------------------------

void SPlaneSlicer::applySliceTranslation(vtkSmartPointer<vtkMatrix4x4> vtkMat) const
{
    auto image = this->getInput< ::fwData::Image >(s_EXTENT_IN);

    ::fwData::Object::sptr index;
    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId);
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId);
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId);
            break;
    }

    const int idx = ::fwData::Integer::dynamicCast(index)->value();

    const auto& spacing = image->getSpacing();
    const auto& origin  = image->getOrigin();

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
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImage::Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImage::Orientation >( to );
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SPlaneSlicer::updateDefaultValue()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    SLM_ASSERT("No 'image' found.", image);

    double min, max;
    ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);

    m_reslicer->SetBackgroundLevel(min);
}

//------------------------------------------------------------------------------

} //namespace opVTKSlicer
