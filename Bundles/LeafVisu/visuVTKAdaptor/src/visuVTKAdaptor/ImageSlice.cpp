/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImageSlice.hpp"

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


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageSlice, ::fwData::Composite );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_CHECK_CTRL_IMAGE_SLOT   = "checkCtrlImage";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

//------------------------------------------------------------------------------

ImageSlice::ImageSlice() throw() :
    m_interpolation(true),
    m_useImageTF(false),
    m_actorOpacity(1.),
    m_imageSource(nullptr),
    m_imageActor(vtkImageActor::New()),
    m_planeOutlinePolyData(vtkPolyData::New()),
    m_planeOutlineMapper(vtkPolyDataMapper::New()),
    m_planeOutlineActor(vtkActor::New())
{
    newSlot(s_CHECK_CTRL_IMAGE_SLOT, &ImageSlice::checkCtrlImage, this);
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &ImageSlice::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &ImageSlice::updateSliceType, this);
}

//------------------------------------------------------------------------------

ImageSlice::~ImageSlice() throw()
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

void ImageSlice::doStart() throw(fwTools::Failed)
{
    this->addToRenderer(m_imageActor);
    this->addToRenderer(m_planeOutlineActor);
    this->addToPicker(m_imageActor);

    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLICE_INDEX_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLICE_TYPE_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_BUFFER_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLOT);
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageSlice::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_connections.disconnect();
    this->removeFromPicker(m_imageActor);
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void ImageSlice::doSwap() throw(fwTools::Failed)
{
    m_connections.disconnect();
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLICE_INDEX_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLICE_TYPE_SLOT);
    m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_BUFFER_MODIFIED_SIG,
                          this->getSptr(), s_UPDATE_SLOT);
    this->doUpdate();
}

//------------------------------------------------------------------------------

::fwData::Image::sptr ImageSlice::getCtrlImage()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    if (m_ctrlImage.expired())
    {
        if (!m_ctrlImageId.empty())
        {
            m_ctrlImage.reset();
            ::fwData::Composite::iterator it = (*composite).find(m_ctrlImageId);
            if (it != (*composite).end())
            {
                m_ctrlImage = ::fwData::Image::dynamicCast((*it).second);
            }
        }
    }
    SLM_ASSERT("Null control image", !m_ctrlImage.expired());
    return m_ctrlImage.lock();
}

//------------------------------------------------------------------------------

void ImageSlice::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getCtrlImage();

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    if (imageIsValid)
    {
        this->buildPipeline();
        this->updateImage(image);
        this->updateImageSliceIndex(image);
        this->updateOutline();
    }
}

//-----------------------------------------------------------------------------

void ImageSlice::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = m_ctrlImage.lock();
    this->updateImageSliceIndex(image);
    this->updateOutline();
}

//-----------------------------------------------------------------------------

void ImageSlice::updateSliceType(int from, int to)
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

void ImageSlice::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");
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
    }
    if(m_configuration->hasAttribute("ctrlimage") )
    {
        this->setCtrlImageId( m_configuration->getAttributeValue("ctrlimage") );
    }
    if(m_configuration->hasAttribute("vtkimagesource") )
    {
        this->setVtkImageSourceId( m_configuration->getAttributeValue("vtkimagesource") );
    }
    if (m_configuration->hasAttribute("interpolation"))
    {
        this->setInterpolation(!(m_configuration->getAttributeValue("interpolation") == "off"));
    }
    if(m_configuration->hasAttribute("actorOpacity") )
    {
        this->setActorOpacity(::boost::lexical_cast<double>(m_configuration->getAttributeValue("actorOpacity")));
    }
}

//------------------------------------------------------------------------------

void ImageSlice::updateImage( ::fwData::Image::sptr image  )
{
    SLM_ASSERT("Null control image", !m_ctrlImage.expired());
    this->updateImageInfos(image);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageSlice::updateImageSliceIndex( ::fwData::Image::sptr image )
{
    unsigned int axialIndex    = m_axialIndex->value();
    unsigned int frontalIndex  = m_frontalIndex->value();
    unsigned int sagittalIndex = m_sagittalIndex->value();

    const unsigned int pos[3] = { sagittalIndex, frontalIndex, axialIndex  };
    //int pos[3];
    //pos[2]= axialIndex;
    //pos[1]= frontalIndex;
    //pos[0]= sagittalIndex;

    this->setSlice( pos[ m_orientation], image );
}

//------------------------------------------------------------------------------

void ImageSlice::setSlice( int slice, ::fwData::Image::sptr image  )
{
    SLM_TRACE_FUNC();
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

void ImageSlice::buildPipeline( )
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

void ImageSlice::buildOutline()
{
    vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
    points->SetNumberOfPoints(4);
    int i;
    for (i = 0; i < 4; i++)
    {
        points->SetPoint(i,0.0,0.0,0.0);
    }

    vtkCellArray* cells = vtkCellArray::New();
    cells->Allocate(cells->EstimateSize(4,2));
    vtkIdType pts[2];
    pts[0] = 3; pts[1] = 2;       // top edge
    cells->InsertNextCell(2,pts);
    pts[0] = 0; pts[1] = 1;       // bottom edge
    cells->InsertNextCell(2,pts);
    pts[0] = 0; pts[1] = 3;       // left edge
    cells->InsertNextCell(2,pts);
    pts[0] = 1; pts[1] = 2;       // right edge
    cells->InsertNextCell(2,pts);

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

void ImageSlice::updateOutline()
{
    SLM_TRACE_FUNC();
    static const int indexZ[12]   = { 0,2,4, 1,2,4,  1,3,4,0,3,4 };
    static const int indexY[12]   = { 0,2,4, 1,2,4,  1,2,5,0,2,5 };
    static const int indexX[12]   = { 0,2,4, 0,2,5,  0,3,5,0,3,4 };
    static const int* indexSet[3] = { indexX, indexY, indexZ  };
    static double colors[3][3]    = { {0.,0.,1.}, {0.,1.,0.}, {1.,0.,0.} };

    double* extent    = m_imageActor->GetBounds();
    vtkPoints* points = m_planeOutlinePolyData->GetPoints();


    const int* index = indexSet[ m_orientation ];
    for ( int i = 0; i < 4; ++i)
    {
        double pt[3];
        pt[0] = extent[ *(index++) ];
        pt[1] = extent[ *(index++) ];
        pt[2] = extent[ *(index++) ];
        points->SetPoint(i,pt);
    }

    points->GetData()->Modified();
    m_planeOutlinePolyData->Modified();

    m_planeOutlineActor->GetProperty()->SetColor( colors[m_orientation]);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageSlice::checkCtrlImage()
{
    if (m_ctrlImage.expired() || m_ctrlImage.lock() != this->getCtrlImage())
    {
        m_connections.disconnect();
        m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);
        m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLICE_INDEX_SLOT);
        m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLICE_TYPE_SLOT);
        m_connections.connect(this->getCtrlImage(), ::fwData::Image::s_BUFFER_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);

        this->doUpdate();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImageSlice::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_CHECK_CTRL_IMAGE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHECK_CTRL_IMAGE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_CHECK_CTRL_IMAGE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
