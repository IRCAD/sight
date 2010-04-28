/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/helpers.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/macros.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkCubeSource.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageActor.h>
#include <vtkCellArray.h>
#include <vtkTransform.h>

#include "visuVTKAdaptor/NegatoOneSlice.hpp"


void convertTF2vtkTF(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst,
        bool allow_transparency = false
        )
{
    SLM_TRACE_FUNC();
    //vtkWindowLevelLookupTable * lookupTable = vtkWindowLevelLookupTable::New();

    // Compute center and width
    std::pair< double, ::boost::int32_t > centerAndWidth = _pTransfertFunctionSrc->getCenterWidth();
    double width = centerAndWidth.second;

    // Compute min and max
    typedef ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPCIterator;
    std::pair< TFPCIterator, TFPCIterator > range = _pTransfertFunctionSrc->getTransfertFunctionPoints();
    int min = (*range.first)->getValue();


    // Convert tf points
    //-------------------

    // Init iterator
    TFPCIterator iterTF = range.first;
    TFPCIterator iterTFNext = range.first + 1;
    TFPCIterator end = range.second;

    // Must have point in data tf
    assert( iterTF != end );

    // Init parameters
    int i = 0;
    double r, g, b;
    double alpha = 1.0;
    int value = 0;

    // Set first point
    value = ((*iterTF)->getValue() - min) * 255.0 / width;
    const ::fwData::Color::ColorArray & vRGBA0 = (*iterTF)->getColor()->getCRefRGBA();

    if(allow_transparency)
    {
        alpha = vRGBA0[3];
    }
    lookupTableDst->SetTableValue(i, vRGBA0[0], vRGBA0[1], vRGBA0[2], alpha);

    i++;
    while ( iterTFNext != end )
    {
        // First point
        value = ((*iterTF)->getValue() - min) * 255.0 / width;
        const ::fwData::Color::ColorArray & vRGBA = (*iterTF)->getColor()->getCRefRGBA();

        // Second point
        int valueNext = ((*iterTFNext)->getValue() - min) * 255.0 / width;
        const ::fwData::Color::ColorArray & vRGBANext = (*iterTFNext)->getColor()->getCRefRGBA();

        // Interpolation
        while (i <= valueNext)
        {
            double x = (double)(i - value) / (double)(valueNext - value);
            r = ( vRGBA[0] + ( vRGBANext[0] - vRGBA[0] ) * x ) ;
            g = ( vRGBA[1] + ( vRGBANext[1] - vRGBA[1] ) * x ) ;
            b = ( vRGBA[2] + ( vRGBANext[2] - vRGBA[2] ) * x ) ;
            if(allow_transparency)
            {
                alpha = ( vRGBA[3] + ( vRGBANext[3] - vRGBA[3] ) * x ) ;
            }

            lookupTableDst->SetTableValue( i, r, g, b , alpha );
            i++;
        }

        iterTF++;
        iterTFNext++;
    }

    lookupTableDst->SetTableRange( min, min + width );

    lookupTableDst->Build();

}


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoOneSlice, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

NegatoOneSlice::NegatoOneSlice() throw()
{
    m_lut = vtkLookupTable::New();
    m_imageActor = vtkImageActor::New();
    m_map2colors = vtkImageMapToColors::New();
    m_imageData = vtkImageData::New();

    //m_planeSource = vtkPlaneSource::New();
    m_planeOutlinePolyData = vtkPolyData::New();
    m_planeOutlineMapper   = vtkPolyDataMapper::New();
    m_planeOutlineActor    = vtkActor::New();

    // Manage events
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
    addNewHandledEvent( ::fwComEd::ImageMsg::MODIFIED );
    addNewHandledEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION );
    addNewHandledEvent( ::fwComEd::ImageMsg::WINDOWING );
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
}

//------------------------------------------------------------------------------

NegatoOneSlice::~NegatoOneSlice() throw()
{
    m_lut->Delete();
    m_lut = NULL;

    m_imageActor->Delete();
    m_imageActor = NULL;

    m_map2colors->Delete();
    m_map2colors = NULL;

    m_imageData->Delete();
    m_imageData = NULL;

    m_planeOutlineActor->Delete();
    m_planeOutlineActor = NULL;

    m_planeOutlineMapper->Delete();
    m_planeOutlineMapper = NULL;

    m_planeOutlinePolyData->Delete();
    m_planeOutlinePolyData = NULL;
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStart() throw(fwTools::Failed)
{
    this->addToRenderer(m_imageActor);
    this->addToRenderer(m_planeOutlineActor);
    this->addToPicker(m_imageActor);
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
    this->removeFromPicker(m_imageActor);
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doSwap() throw(fwTools::Failed)
{
    doUpdate();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        buildPipeline();
        updateImage(image);
        updateSliceIndex(image);
        updateOutline();
        updateTransfertFunction(image);
        updateWindowing(image);
    }
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {

        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            doUpdate();
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::MODIFIED ) )
        {
            m_imageData->Modified();
            this->setVtkPipelineModified();
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION ) )
        {
            updateTransfertFunction(image);
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::WINDOWING ) )
        {
            ::fwComEd::ImageMsg::dynamicConstCast(msg)->getWindowMinMax( m_windowMin, m_windowMax);
            updateWindowing(image);
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) )
        {
            ::fwComEd::ImageMsg::dynamicConstCast(msg)->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
            updateSliceIndex(image);
            updateOutline();
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
                doUpdate();
            }
            else if(fromSliceType == static_cast<int>(m_orientation))
            {
                setOrientation( static_cast< Orientation >( toSliceType ));
                doUpdate();
            }
        }
    }
}

//------------------------------------------------------------------------------

void NegatoOneSlice::configuring() throw(fwTools::Failed)
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
}


//------------------------------------------------------------------------------


void NegatoOneSlice::updateImage( ::fwData::Image::sptr image  )
{
    ::vtkIO::toVTKImage(image,m_imageData);
    m_map2colors->SetInput(m_imageData);

    this->updateImageInfos(image);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::updateSliceIndex( ::fwData::Image::sptr image )
{
    unsigned int axialIndex    = m_axialIndex->value();
    unsigned int frontalIndex  = m_frontalIndex->value();
    unsigned int sagittalIndex = m_sagittalIndex->value();

    int pos[3];
    pos[2]= axialIndex;
    pos[1]= frontalIndex;
    pos[0]= sagittalIndex;

    this->setSlice( pos[ m_orientation] );
}

//------------------------------------------------------------------------------

void NegatoOneSlice::updateWindowing( ::fwData::Image::sptr image )
{
    //std::pair<bool,bool> fieldsAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( image );
    // Temp test because theses cases are not manage ( need to notify if there are modifications of Min/Max/TF )
    //assert( ! fieldsAreModified.first && ! fieldsAreModified.second );

    m_lut->SetTableRange( m_windowMin->value(), m_windowMax->value() );
    m_lut->Modified();
    setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::updateTransfertFunction( ::fwData::Image::sptr image )
{
    ::fwData::Composite::sptr tfComposite = m_transfertFunctions;
    std::string tfName = m_transfertFunctionId->value();
    ::fwData::TransfertFunction::sptr pTransfertFunction = ::fwData::TransfertFunction::dynamicCast(tfComposite->getRefMap()[tfName]);
    convertTF2vtkTF( pTransfertFunction, m_lut );
    setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::setSlice( int slice )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    assert ( imageIsValid );

    int extent[6];
    std::fill(  extent, extent+6, 0);
    extent[1] = image->getSize()[0]-1;
    extent[3] = image->getSize()[1]-1;
    extent[5] = image->getSize()[2]-1;
    extent[2*m_orientation]=slice;
    extent[2*m_orientation+1]=slice;

    m_imageActor->SetDisplayExtent( extent );
//  this->getRenderer()->ResetCameraClippingRange();
    setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::buildPipeline( )
{
    m_map2colors->SetLookupTable(m_lut);
    m_imageActor->SetInput(m_map2colors->GetOutput());
    if(!this->getTransformId().empty())
    {
        m_imageActor->SetUserTransform(this->getTransform());
    }
//  m_imageActor->InterpolateOff();
    buildOutline();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::buildOutline()
{
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

const int indexZ[12] = { 0,2,4, 1,2,4,  1,3,4 ,0,3,4 };
const int indexY[12] = { 0,2,4, 1,2,4,  1,2,5 ,0,2,5 };
const int indexX[12] = { 0,2,4, 0,2,5,  0,3,5 ,0,3,4 };
const int *indexSet[3] = { indexX, indexY, indexZ  };
double colors[3][3] = { {0.,0.,1.} , {0.,1.,0.}, {1.,0.,0.} };

void NegatoOneSlice::updateOutline()
{
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
    setVtkPipelineModified();
}



} //namespace visuVTKAdaptor
