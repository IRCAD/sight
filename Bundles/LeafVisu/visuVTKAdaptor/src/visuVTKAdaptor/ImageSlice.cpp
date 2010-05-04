/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/helpers.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageActor.h>
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include "visuVTKAdaptor/ImageSlice.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageSlice, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

ImageSlice::ImageSlice() throw()
{
    SLM_TRACE_FUNC();
    m_imageActor = vtkImageActor::New();

    //m_planeSource = vtkPlaneSource::New();
    m_planeOutlinePolyData = vtkPolyData::New();
    m_planeOutlineMapper   = vtkPolyDataMapper::New();
    m_planeOutlineActor    = vtkActor::New();

    m_interpolation = true;

    // Manage events
    this->addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX         );
    this->addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE   );
    this->addNewHandledEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS );
}

//------------------------------------------------------------------------------

ImageSlice::~ImageSlice() throw()
{
    SLM_TRACE_FUNC();
    m_imageActor->Delete();
    m_imageActor = NULL;

    m_planeOutlineActor->Delete();
    m_planeOutlineActor = NULL;

    m_planeOutlineMapper->Delete();
    m_planeOutlineMapper = NULL;

    m_planeOutlinePolyData->Delete();
    m_planeOutlinePolyData = NULL;
}

//------------------------------------------------------------------------------

void ImageSlice::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->addToRenderer(m_imageActor);
    this->addToRenderer(m_planeOutlineActor);
    this->addToPicker(m_imageActor);
}

//------------------------------------------------------------------------------

void ImageSlice::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeFromPicker(m_imageActor);
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void ImageSlice::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageSlice::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    if (!m_ctrlImageId.empty())
    {
        m_ctrlImage.reset();
        ::fwData::Composite::iterator it = (*composite).find(m_ctrlImageId);
        if (it != (*composite).end())
        {
            m_ctrlImage = ::fwData::Image::dynamicCast((*it).second);
        }
    }
    SLM_ASSERT("Null control image", !m_ctrlImage.expired());



    ::fwData::Image::sptr image = m_ctrlImage.lock();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (!m_imageComChannel.expired())
    {
        ::fwServices::OSR::unregisterService(m_imageComChannel.lock());
    }
    m_imageComChannel = ::fwServices::registerCommunicationChannel(image, this->getSptr());
    m_imageComChannel.lock()->start();

    if (imageIsValid)
    {
        this->buildPipeline();
        this->updateImage(image);
        this->updateSliceIndex(image);
        this->updateOutline();
    }
}

//------------------------------------------------------------------------------

void ImageSlice::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = m_ctrlImage.lock();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( msg->hasEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS ) )
    {
        SLM_TRACE("Has event MODIFIED_FIELDS");
        doUpdate();
    }


    if (imageIsValid)
    {

        if ( msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) )
        {
            ::fwComEd::ImageMsg::dynamicConstCast(msg)->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
            this->updateSliceIndex(image);
            this->updateOutline();
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE ) && imageIsValid)
        {
            ::fwData::Object::csptr cObjInfo = msg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
            ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
            ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

            int fromSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["fromSliceType"] )->value();
            int toSliceType =   ::fwData::Integer::dynamicCast( info->getRefMap()["toSliceType"] )->value();

            if( toSliceType == static_cast<int>(m_orientation) )
            {
                setOrientation( static_cast< Orientation >( fromSliceType ));
                this->doUpdate();
            }
            else if(fromSliceType == static_cast<int>(m_orientation))
            {
                setOrientation( static_cast< Orientation >( toSliceType ));
                this->doUpdate();
            }
        }
    }
}

//------------------------------------------------------------------------------

void ImageSlice::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    if(m_configuration->hasAttribute("sliceIndex"))
    {
         std::string  orientation = m_configuration->getAttributeValue("sliceIndex");
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
    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
    if(m_configuration->hasAttribute("ctrlimage") )
    {
        this->setCtrlImageId( m_configuration->getAttributeValue("ctrlimage") );
    }
    if(m_configuration->hasAttribute("vtkimagesource") )
    {
        this->setVtkImageSourceId( m_configuration->getAttributeValue("vtkimagesource") );
    }
    if(m_configuration->getAttributeValue("interpolation") == std::string("off") )
    {
        m_interpolation = false;
    }
}


//------------------------------------------------------------------------------


void ImageSlice::updateImage( ::fwData::Image::sptr image  )
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Null control image", !m_ctrlImage.expired());
    this->updateImageInfos(m_ctrlImage.lock());

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageSlice::updateSliceIndex( ::fwData::Image::sptr image )
{
    SLM_TRACE_FUNC();
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
    extent[1] = image->getSize()[0]-1;
    extent[3] = image->getSize()[1]-1;
    extent[5] = image->getSize()[2]-1;
    extent[2*m_orientation]=slice;
    extent[2*m_orientation+1]=slice;

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
    SLM_TRACE_FUNC();

    if (!m_imageSourceId.empty())
    {
        m_imageSource = this->getVtkObject(m_imageSourceId);
    }

    vtkImageAlgorithm *algorithm  = vtkImageAlgorithm::SafeDownCast(m_imageSource);
    vtkImageData      *imageData  = vtkImageData::SafeDownCast(m_imageSource);
    //vtkImageBlend     *imageBlend = vtkImageBlend::SafeDownCast(m_imageSource);

    SLM_ASSERT("Invalid vtk image source", algorithm||imageData )
    if (algorithm)
    {
        SLM_TRACE("Input is a vtkImageAlgorithm");
        m_imageActor->SetInput(algorithm->GetOutput());
        //if (imageBlend)
        //{
            //imageBlend->SetBlendModeToCompound();
            //imageBlend->SetCompoundThreshold(0);
        //}
    }
    else if (imageData)
    {
        SLM_TRACE("Input is a vtkImageData");
        m_imageActor->SetInput(imageData);
    }

    if(!this->getTransformId().empty())
    {
        m_imageActor->SetUserTransform(this->getTransform());
    }

    m_imageActor->SetInterpolate(m_interpolation);

    this->buildOutline();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------
void ImageSlice::buildOutline()
{
    SLM_TRACE_FUNC();
    vtkPoints* points   = vtkPoints::New(VTK_DOUBLE);
    points->SetNumberOfPoints(4);
    int i;
    for (i = 0; i < 4; i++)
    {
        points->SetPoint(i,0.0,0.0,0.0);
    }

    vtkCellArray *cells = vtkCellArray::New();
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
    points = NULL;
    m_planeOutlinePolyData->SetLines(cells);
    cells->Delete();
    cells = NULL;

    m_planeOutlineMapper = vtkPolyDataMapper::New();
    m_planeOutlineMapper->SetInput( m_planeOutlinePolyData );
    m_planeOutlineMapper->SetResolveCoincidentTopologyToPolygonOffset();
    m_planeOutlineActor->SetMapper(m_planeOutlineMapper);
    m_planeOutlineActor->PickableOff();
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
    static const int indexZ[12] = { 0,2,4, 1,2,4,  1,3,4 ,0,3,4 };
    static const int indexY[12] = { 0,2,4, 1,2,4,  1,2,5 ,0,2,5 };
    static const int indexX[12] = { 0,2,4, 0,2,5,  0,3,5 ,0,3,4 };
    static const int *indexSet[3] = { indexX, indexY, indexZ  };
    static double colors[3][3] = { {0.,0.,1.} , {0.,1.,0.}, {1.,0.,0.} };

    double *extent = m_imageActor->GetBounds();
    vtkPoints* points = m_planeOutlinePolyData->GetPoints();


    const int *index = indexSet[ m_orientation ];
    for ( int i=0; i < 4; ++i)
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


} //namespace visuVTKAdaptor
