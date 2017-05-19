/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SVideoAdapter.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkCamera.h>
#include <vtkImageActor.h>
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
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT                 = "show";
static const  ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT           = "calibrate";

//------------------------------------------------------------------------------

SVideoAdapter::SVideoAdapter() throw() :
    m_imageData(vtkImageData::New()),
    m_actor(vtkImageActor::New()),
    m_isTextureInit(false),
    m_reverse(true)
{
    newSlot(s_UPDATE_IMAGE_SLOT, &SVideoAdapter::updateImage, this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &SVideoAdapter::updateImageOpacity, this);
    newSlot(s_SHOW_SLOT, &SVideoAdapter::show, this);
    newSlot(s_CALIBRATE_SLOT, &SVideoAdapter::offsetOpticalCenter, this);
}

//------------------------------------------------------------------------------

SVideoAdapter::~SVideoAdapter() throw()
{
    m_actor->Delete();
    m_actor = nullptr;
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

void SVideoAdapter::doConfigure() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    m_cameraUID = m_configuration->getAttributeValue("cameraUID");

    std::string reverse = m_configuration->getAttributeValue("reverse");
    if (!reverse.empty() && reverse == "false")
    {
        m_reverse = false;
    }
    this->setPickerId(m_configuration->getAttributeValue("picker"));
}

//------------------------------------------------------------------------------

void SVideoAdapter::doStart() throw(fwTools::Failed)
{
    if (m_reverse)
    {
        m_actor->RotateZ(180);
        m_actor->RotateY(180);
    }

    // Set camera pointer, it will be used if present in doUpdate()
    if (!m_cameraUID.empty())
    {
        m_camera = this->getSafeInput< ::arData::Camera>(m_cameraUID);
        SLM_ASSERT("Missing camera", m_camera);

        m_connections.connect(m_camera, ::arData::Camera::s_MODIFIED_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);
        m_connections.connect(m_camera, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);
    }

    this->doUpdate();

    if (this->getPicker())
    {
        this->addToPicker(m_actor);
    }
}

//------------------------------------------------------------------------------

void SVideoAdapter::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

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

        const ::fwData::Image::SizeType size = image->getSize();

        m_actor->SetInputData(m_imageData);
        this->addToRenderer(m_actor);

        m_isTextureInit = true;

        this->getRenderer()->InteractiveOff();
        this->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
        this->getRenderer()->ResetCamera();
        this->getRenderer()->GetActiveCamera()->SetParallelScale(size[1] / 2.0);
        this->offsetOpticalCenter();
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
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
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
        m_actor->SetOpacity(imageOpacity);
    }
    if(img->getField( "VISIBILITY" ) )
    {
        ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
        m_actor->SetVisibility(*visible);
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideoAdapter::updateImage()
{
    m_isTextureInit = false;
    this->updating();
}

//------------------------------------------------------------------------------

void SVideoAdapter::show(bool visible)
{
    m_actor->SetVisibility(visible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideoAdapter::offsetOpticalCenter()
{
    if (m_camera)
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

        const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        if (!imageIsValid)
        {
            return;
        }

        const ::fwData::Image::SizeType size = image->getSize();

        const double shiftX = size[0] / 2. - m_camera->getCx();
        const double shiftY = size[1] / 2. - m_camera->getCy();

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

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
