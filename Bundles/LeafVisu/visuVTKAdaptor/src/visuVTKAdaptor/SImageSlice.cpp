/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImageSlice.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageActor.h>
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageSlice );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SImageSlice::SImageSlice() noexcept :
    m_interpolation(true),
    m_actorOpacity(1.),
    m_imageSource(nullptr),
    m_imageActor(vtkImageActor::New()),
    m_planeOutlinePolyData(vtkPolyData::New()),
    m_planeOutlineMapper(vtkPolyDataMapper::New()),
    m_planeOutlineActor(vtkActor::New())
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SImageSlice::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SImageSlice::updateSliceType, this);
}

//------------------------------------------------------------------------------

SImageSlice::~SImageSlice() noexcept
{
    m_imageActor->Delete();
    m_imageActor = nullptr;

    m_planeOutlineActor->Delete();
    m_planeOutlineActor = nullptr;

    m_planeOutlineMapper->Delete();
    m_planeOutlineMapper = nullptr;

    m_planeOutlinePolyData->Delete();
    m_planeOutlinePolyData = nullptr;
}

//------------------------------------------------------------------------------

void SImageSlice::starting()
{
    this->initialize();

    this->addToRenderer(m_imageActor);
    this->addToRenderer(m_planeOutlineActor);
    this->addToPicker(m_imageActor);

    this->updating();
}

//------------------------------------------------------------------------------

void SImageSlice::stopping()
{
    this->removeFromPicker(m_imageActor);
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SImageSlice::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    if (imageIsValid)
    {
        this->buildPipeline();
        this->updateImage(image);
        this->updateSImageSliceIndex(image);
        this->updateOutline();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SImageSlice::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->updateSImageSliceIndex(image);
    this->updateOutline();
}

//-----------------------------------------------------------------------------

void SImageSlice::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_orientation) )
    {
        setOrientation( static_cast< Orientation >( from ));
    }
    else if(from == static_cast<int>(m_orientation))
    {
        setOrientation( static_cast< Orientation >( to ));
    }
    this->updating();
}

//------------------------------------------------------------------------------

void SImageSlice::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    const std::string orientation = config.get<std::string>("sliceIndex", "axial");
    if(orientation == "axial" )
    {
        m_orientation = Z_AXIS;
    }
    else if(orientation == "frontal" )
    {
        m_orientation = Y_AXIS;
    }
    else if(orientation == "sagittal" )
    {
        m_orientation = X_AXIS;
    }

    this->setVtkImageSourceId( config.get<std::string>("vtkimagesource", ""));

    const std::string interpolation = config.get<std::string>("interpolation", "off");
    SLM_ASSERT("'interpolation' value must be 'on' or 'off', actual: " + interpolation,
               interpolation == "on" || interpolation == "off");
    this->setInterpolation(interpolation == "yes");

    this->setActorOpacity(config.get<double>("actorOpacity", 1.));
}

//------------------------------------------------------------------------------

void SImageSlice::updateImage( ::fwData::Image::sptr image  )
{
    this->updateImageInfos(image);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageSlice::updateSImageSliceIndex( ::fwData::Image::sptr image )
{
    int axialIndex    = m_axialIndex->value();
    int frontalIndex  = m_frontalIndex->value();
    int sagittalIndex = m_sagittalIndex->value();

    const int pos[3] = { sagittalIndex, frontalIndex, axialIndex  };

    this->setSlice( pos[ m_orientation], image );
}

//------------------------------------------------------------------------------

void SImageSlice::setSlice( int slice, ::fwData::Image::sptr image  )
{
    int extent[6];
    std::fill(  extent, extent+6, 0);
    extent[1]                 = static_cast<int>(image->getSize()[0]-1);
    extent[3]                 = static_cast<int>(image->getSize()[1]-1);
    extent[5]                 = static_cast<int>(image->getSize()[2]-1);
    extent[2*m_orientation]   = slice;
    extent[2*m_orientation+1] = slice;

    OSLM_TRACE("DisplayExtent : " << " X min: " << extent[0] << " X max: " << extent[1] <<
               " Y min: " << extent[2] << " Y max: " << extent[3] <<
               " Z min: " << extent[4] << " Z max: " << extent[5]
               );

    m_imageActor->SetDisplayExtent( extent );

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageSlice::buildPipeline( )
{
    if (!m_imageSourceId.empty())
    {
        m_imageSource = this->getVtkObject(m_imageSourceId);
    }

    vtkImageAlgorithm* algorithm = vtkImageAlgorithm::SafeDownCast(m_imageSource);
    vtkImageData* imageData      = vtkImageData::SafeDownCast(m_imageSource);
    //vtkImageBlend     *imageBlend = vtkImageBlend::SafeDownCast(m_imageSource);

    SLM_ASSERT("Invalid vtk image source", algorithm||imageData );
    if (algorithm)
    {
        SLM_TRACE("Input is a vtkImageAlgorithm");
        m_imageActor->GetMapper()->SetInputConnection(algorithm->GetOutputPort());
        //if (imageBlend)
        //{
        //imageBlend->SetBlendModeToCompound();
        //imageBlend->SetCompoundThreshold(0);
        //}
    }
    else if (imageData)
    {
        SLM_TRACE("Input is a vtkImageData");
        m_imageActor->SetInputData(imageData);
    }

    if(!this->getTransformId().empty())
    {
        m_imageActor->SetUserTransform(this->getTransform());
    }

    m_imageActor->SetInterpolate(m_interpolation);
    m_imageActor->SetOpacity(m_actorOpacity);

    this->buildOutline();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageSlice::buildOutline()
{
    vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
    points->SetNumberOfPoints(4);
    int i;
    for (i = 0; i < 4; i++)
    {
        points->SetPoint(i, 0.0, 0.0, 0.0);
    }

    vtkCellArray* cells = vtkCellArray::New();
    cells->Allocate(cells->EstimateSize(4, 2));
    vtkIdType pts[2];
    pts[0] = 3; pts[1] = 2;       // top edge
    cells->InsertNextCell(2, pts);
    pts[0] = 0; pts[1] = 1;       // bottom edge
    cells->InsertNextCell(2, pts);
    pts[0] = 0; pts[1] = 3;       // left edge
    cells->InsertNextCell(2, pts);
    pts[0] = 1; pts[1] = 2;       // right edge
    cells->InsertNextCell(2, pts);

    m_planeOutlinePolyData->SetPoints(points);
    points->Delete();
    points = nullptr;
    m_planeOutlinePolyData->SetLines(cells);
    cells->Delete();
    cells = nullptr;

    m_planeOutlineMapper = vtkPolyDataMapper::New();
    m_planeOutlineMapper->SetInputData( m_planeOutlinePolyData );
    m_planeOutlineMapper->SetResolveCoincidentTopologyToPolygonOffset();
    m_planeOutlineActor->SetMapper(m_planeOutlineMapper);
    m_planeOutlineActor->PickableOff();
    m_planeOutlineActor->GetProperty()->SetOpacity(0.9);
    if(!this->getTransformId().empty())
    {
        m_planeOutlineActor->SetUserTransform(this->getTransform());
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImageSlice::updateOutline()
{
    static const int indexZ[12]   = { 0, 2, 4, 1, 2, 4,  1, 3, 4, 0, 3, 4 };
    static const int indexY[12]   = { 0, 2, 4, 1, 2, 4,  1, 2, 5, 0, 2, 5 };
    static const int indexX[12]   = { 0, 2, 4, 0, 2, 5,  0, 3, 5, 0, 3, 4 };
    static const int* indexSet[3] = { indexX, indexY, indexZ  };
    static double colors[3][3]    = { {0., 0., 1.}, {0., 1., 0.}, {1., 0., 0.} };

    double* extent    = m_imageActor->GetBounds();
    vtkPoints* points = m_planeOutlinePolyData->GetPoints();

    const int* index = indexSet[ m_orientation ];
    for ( int i = 0; i < 4; ++i)
    {
        double pt[3];
        pt[0] = extent[ *(index++) ];
        pt[1] = extent[ *(index++) ];
        pt[2] = extent[ *(index++) ];
        points->SetPoint(i, pt);
    }

    points->GetData()->Modified();
    m_planeOutlinePolyData->Modified();

    m_planeOutlineActor->GetProperty()->SetColor( colors[m_orientation]);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageSlice::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
