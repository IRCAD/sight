/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SVideoAdapter.hpp"

#include <arData/Camera.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKARAdaptor::SVideoAdapter, ::fwData::Image );


namespace visuVTKARAdaptor
{
//------------------------------------------------------------------------------

SVideoAdapter::SVideoAdapter() throw() :
    m_actor(vtkActor::New()),
    m_isTextureInit(false),
    m_reverse(true)

{
    m_imageData = vtkImageData::New();
    m_texture   = vtkTexture::New();
}

//------------------------------------------------------------------------------

SVideoAdapter::~SVideoAdapter() throw()
{
    m_actor->Delete();
    m_actor = 0;
}

//------------------------------------------------------------------------------

void SVideoAdapter::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    m_cameraUID = m_configuration->getAttributeValue("cameraUID");

    std::string reverse = m_configuration->getAttributeValue("reverse");
    if (!reverse.empty() && reverse == "false")
    {
        m_reverse = false;
    }
}

//------------------------------------------------------------------------------


void SVideoAdapter::doStart() throw(fwTools::Failed)
{
    vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
    vtkPlaneSource* plan      = vtkPlaneSource ::New();
    mapper->SetInputConnection(plan->GetOutputPort());
    m_actor->SetMapper(mapper);
    if (m_reverse)
    {
        m_actor->RotateZ(180);
        m_actor->RotateY(180);
    }
    this->setVtkPipelineModified();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SVideoAdapter::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (!imageIsValid)
    {
        return;
    }

    ::fwVtkIO::toVTKImage(image, m_imageData);

    if(!m_isTextureInit)
    {
        this->removeAllPropFromRenderer();

        // Reset the transform matrix first
        m_actor->PokeMatrix(vtkMatrix4x4::New());
        if (m_reverse)
        {

            m_actor->RotateZ(180);
            m_actor->RotateY(180);
        }

        ::fwData::Image::SizeType size       = image->getSize();
        ::fwData::Image::SpacingType spacing = image->getSpacing();

        m_texture->SetInputData( m_imageData );
        m_actor->SetScale(size[0]*spacing[0], size[1]*spacing[1], 1.);
        this->addToRenderer(m_actor);

        m_isTextureInit = true;

        vtkProperty *property = m_actor->GetProperty();
        property->RemoveTexture("texture");
        property->SetTexture("texture", m_texture);

        this->getRenderer()->InteractiveOff();
        this->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
        this->getRenderer()->ResetCamera();
        this->getRenderer()->GetActiveCamera()->SetParallelScale(size[1] / 2.0);

        if (!m_cameraUID.empty())
        {
            ::fwTools::Object::sptr obj   = ::fwTools::fwID::getObject(m_cameraUID);
            ::arData::Camera::sptr camera = ::arData::Camera::dynamicCast(obj);
            SLM_ASSERT("Missing camera", camera);

            double shiftX = size[0] / 2. - camera->getCx();
            double shiftY = size[1] / 2. - camera->getCy();

            if (m_reverse)
            {
                m_actor->SetPosition(shiftX, -shiftY, 0);
            }
            else
            {
                m_actor->SetPosition(-shiftX, shiftY, 0);
            }
        }
    }

    m_imageData->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SVideoAdapter::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SVideoAdapter::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SVideoAdapter::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imMsg = ::fwComEd::ImageMsg::dynamicConstCast( msg );

    if ( imMsg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )
         || imMsg->hasEvent( ::fwComEd::ImageMsg::MODIFIED )
         )
    {
        // If the image has changed, we need to modify the texture
        m_isTextureInit = false;
    }

    if ( imMsg->hasEvent( ::fwComEd::ImageMsg::BUFFER )
         || imMsg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )
         || imMsg->hasEvent( ::fwComEd::ImageMsg::MODIFIED )
         )
    {
        this->doUpdate();
    }

    if ( imMsg->hasEvent( ::fwComEd::ImageMsg::TRANSPARENCY ) || imMsg->hasEvent( ::fwComEd::ImageMsg::VISIBILITY ) )
    {
        this->updateImageOpacity();
    }
}

//------------------------------------------------------------------------------

void SVideoAdapter::updateImageOpacity()
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
    if(img->getField( "TRANSPARENCY" ) )
    {
        ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
        double imageOpacity = (100 - (*transparency) ) / 100.0;
        m_actor->GetProperty()->SetOpacity(imageOpacity);
    }
    if(img->getField( "VISIBILITY" ) )
    {
        ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
        m_actor->SetVisibility(*visible);
    }

    this->setVtkPipelineModified();
}

} //namespace visuVTKARAdaptor
