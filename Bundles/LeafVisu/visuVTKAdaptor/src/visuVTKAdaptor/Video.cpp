/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Video.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/VideoMsg.hpp>


#include <vtkPlaneSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkgl.h>

#include "visuVTKAdaptor/Video.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Video, ::fwData::Video ) ;


namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Video::Video() throw()
{
    m_imageData   = vtkImageData::New();
    m_array = vtkUnsignedCharArray::New();
    m_texture = vtkTexture::New();
    bText_init  = false;

    addNewHandledEvent( ::fwComEd::VideoMsg::VIDEO_IS_REFRESHED );
}

//------------------------------------------------------------------------------

Video::~Video() throw()
{}

//------------------------------------------------------------------------------

void Video::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void Video::doStart() throw(fwTools::Failed)
{
    vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
    vtkPlaneSource* plan = vtkPlaneSource ::New();
    mapper->SetInput(plan->GetOutput());
    vtkActor* actor = this->getActor();
    actor->SetMapper(mapper);
    this->setVtkPipelineModified();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Video::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Video::sptr video = this->getObject< ::fwData::Video >();
    if (!video->dataAvailable())
        return;

    const ::boost::uint8_t * _pImageBuffer = video->getImageBuffer();

    int size[3];
    size[0] = video->getXSize() ;
    size[1] = video->getYSize();
    size[2] = 1;

    m_array->SetNumberOfComponents(video->m_ui8BPP );
    m_array->SetVoidArray( (unsigned char*)_pImageBuffer, video->getXSize() * video->getYSize() ,1);
    m_imageData->GetPointData()->SetScalars(m_array) ;
    m_imageData->SetDimensions(size) ;
    m_imageData->SetScalarType(VTK_UNSIGNED_CHAR) ;
    m_imageData->SetSpacing(1.0, 1.0, 1.0 );
    m_imageData->SetOrigin(0.0, 0.0, 0.0 );

    if(!bText_init)
    {
        m_texture->SetInput(m_imageData ) ;
        this->getActor()->SetScale(size[0], size[1], 1.0);
        this->getRenderer()->InteractiveOff();
        this->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
        this->getRenderer()->ResetCamera();

         //fit the image to the render window
        this->getRenderer()->GetActiveCamera()->SetParallelScale(size[1] / 2.0);

        this->addToRenderer(this->getActor());

        ::fwData::TransformationMatrix3D::sptr intrinsic = video->getCamera()->getIntrinsicCameraTransformation();
        double cx = intrinsic->getCoefficient(0, 2);
        double cy = intrinsic->getCoefficient(1, 2);
        double* pos = this->getActor()->GetPosition();
        pos[0]-= size[0]/2.0 - cx;
        pos[1]-= size[1]/2.0 - cy;
        this->getActor()->SetPosition(pos);

        bText_init  = true;

        vtkProperty *property = this->getActor()->GetProperty();
        ::boost::filesystem::path pathShader ( "Bundles/visuVTKAdaptor_" + std::string(VISUVTKADAPTOR_VER) + "/video.xml" );
        property->LoadMaterial(pathShader.string().c_str());
        property->SetTexture("texture", m_texture);
        property->ShadingOn();
    }
    m_imageData->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Video::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Video::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Video::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwData::Video::sptr video = this->getObject< ::fwData::Video >();

    ::fwComEd::VideoMsg::csptr videoMsg = ::fwComEd::VideoMsg::dynamicConstCast( msg );
    if ( videoMsg &&
            ( videoMsg->hasEvent( ::fwComEd::VideoMsg::VIDEO_IS_REFRESHED ) /* ||  videoMsg->hasEvent( ::fwComEd::VideoMsg::VIDEO_IS_INITIALIZED ) */ ) )
    {
        doUpdate();
    }
}



} //namespace visuVTKAdaptor
