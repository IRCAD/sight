/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SVideoAdapter.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

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

static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_OPACITY_SLOT = "updateImageOpacity";

//------------------------------------------------------------------------------

SVideoAdapter::SVideoAdapter() throw() :
    m_imageData(vtkImageData::New()),
    m_texture(vtkTexture::New()),
    m_actor(vtkActor::New()),
    m_isTextureInit(false),
    m_reverse(true)
{
    newSlot(s_UPDATE_IMAGE_SLOT, &SVideoAdapter::updateImage, this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &SVideoAdapter::updateImageOpacity, this);
}

//------------------------------------------------------------------------------

SVideoAdapter::~SVideoAdapter() throw()
{
    m_actor->Delete();
    m_actor = nullptr;
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


//------------------------------------------------------------------------------

void SVideoAdapter::updateImage()
{
    m_isTextureInit = false;
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SVideoAdapter::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair(::fwData::Image::s_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT));

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
