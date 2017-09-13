/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opVTKSlicer/SPlaneSlicer.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageData.h>
#include <vtkImageReslice.h>

#include <algorithm>

fwServicesRegisterMacro( ::fwServices::IOperator, ::opVTKSlicer::SPlaneSlicer, ::fwData::Image );

namespace opVTKSlicer
{

static const ::fwServices::IService::KeyType s_IMAGE_IN  = "image";
static const ::fwServices::IService::KeyType s_EXTENT_IN = "imageExtent";
static const ::fwServices::IService::KeyType s_AXES_IN   = "axes";
static const ::fwServices::IService::KeyType s_SLICE_OUT = "slice";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";

//------------------------------------------------------------------------------

SPlaneSlicer::SPlaneSlicer() noexcept :
    m_reslicer(::vtkImageReslice::New())
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SPlaneSlicer::updateSliceOrientation, this);
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

    auto image = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    vtkSmartPointer<vtkImageData> vtkimg(vtkImageData::New());

    ::fwVtkIO::toVTKImage(image, vtkimg.Get());

    m_reslicer->SetInputData(vtkimg);
    m_reslicer->Update();

    auto slice = this->getInOut< ::fwData::Image >(s_SLICE_OUT);

    ::fwVtkIO::fromVTKImage(m_reslicer->GetOutput(), slice);

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
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Z_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::Orientation::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Y_AXIS;
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
    std::transform(size.begin(), size.end(), intSize.begin(), [](size_t s){ return static_cast<int>(s) - 1; });

    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::X_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[1], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[1], origin[2], origin[0]);
            m_reslicer->SetOutputSpacing(spacing[1], spacing[2], spacing[0]);
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Y_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[2], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[2], origin[1]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[2], spacing[1]);
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Z_AXIS:
            m_reslicer->SetOutputExtent(0, intSize[0], 0, intSize[1], 0, 0);
            m_reslicer->SetOutputOrigin(origin[0], origin[1], origin[2]);
            m_reslicer->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
            break;
    }
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setReslicerAxes()
{
    ::fwData::Image::csptr extentImg             = this->getInput< ::fwData::Image >(s_EXTENT_IN);
    ::fwData::TransformationMatrix3D::csptr axes = this->getInput< ::fwData::TransformationMatrix3D>(s_AXES_IN);

    SLM_ASSERT("No axes found.", axes);

    // TODO: const correct function signature in fwVtkIO.
    vtkMatrix4x4* axesMatrix(::fwVtkIO::toVTKMatrix(std::const_pointer_cast< ::fwData::TransformationMatrix3D>(axes)));

    //axesMatrix->Invert();

    const auto& size    = extentImg->getSize();
    const auto& origin  = extentImg->getOrigin();
    const auto& spacing = extentImg->getSpacing();

    std::array<double, 3> center = {{ origin[0],
                                      origin[1],
                                      origin[2] }};

    // permutate axes.
    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::X_AXIS:
            // permutate X with Y and Y with Z
            for(std::uint8_t i = 0; i < 4; ++i)
            {
                const double x = axesMatrix->GetElement(i, 0);
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 0, -y);
                axesMatrix->SetElement(i, 1, -z);
                axesMatrix->SetElement(i, 2, x);

                center = {{ origin[0],
                            origin[1] + spacing[1] * size[1],
                            origin[2] + spacing[2] * size[2] }};
            }
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Y_AXIS:
            // permutate Y with Z
            for(std::uint8_t i = 0; i < 4; ++i)
            {
                const double y = axesMatrix->GetElement(i, 1);
                const double z = axesMatrix->GetElement(i, 2);
                axesMatrix->SetElement(i, 1, -z);
                axesMatrix->SetElement(i, 2, y);

                center = {{ origin[0],
                            origin[1],
                            origin[2] + spacing[2] * size[2] }};
            }
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Z_AXIS: break; // Nothing to do.
    }

    axesMatrix->SetElement(0, 3, center[0]);
    axesMatrix->SetElement(1, 3, center[1]);
    axesMatrix->SetElement(2, 3, center[2]);

    SLM_FATAL_IF("BOOM", axesMatrix->Determinant() < 0);

    this->setSliceAxes(axesMatrix);
    m_reslicer->SetResliceAxes(axesMatrix);
}

//------------------------------------------------------------------------------

void SPlaneSlicer::setSliceAxes(vtkMatrix4x4* vtkMat) const
{
    auto image = this->getInput< ::fwData::Image >(s_EXTENT_IN);

    ::fwData::Object::sptr index;
    switch (m_orientation)
    {
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::X_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId);
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Y_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId);
            break;
        case ::fwDataTools::helper::MedicalImageAdaptor::Orientation::Z_AXIS:
            index = image->getField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId);
            break;
    }

    const int idx = ::fwData::Integer::dynamicCast(index)->value();

    const auto& spacing = image->getSpacing();

    const uint8_t axis = static_cast<uint8_t>(m_orientation);

    const double trans = spacing[axis] * static_cast<double>(idx);

    vtkMatrix4x4* transMat = vtkMatrix4x4::New();
    transMat->Identity();
    transMat->SetElement(axis, 3, trans);

    vtkMatrix4x4::Multiply4x4(transMat, vtkMat, vtkMat);
}

//------------------------------------------------------------------------------

void SPlaneSlicer::updateSliceOrientation(int from, int to)
{
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImageAdaptor::Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImageAdaptor::Orientation >( to );
    }

    this->updating();
}
//------------------------------------------------------------------------------

} //namespace opVTKSlicer
