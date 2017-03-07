/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Reconstruction.hpp"

#include "visuVTKAdaptor/Mesh.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwTools/fwID.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkPicker.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Reconstruction,
                         ::fwData::Reconstruction );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType Reconstruction::s_UPDATE_MESH_SLOT        = "updateMesh";
const ::fwCom::Slots::SlotKeyType Reconstruction::s_UPDATE_VISIBILITY_SLOT  = "updateVisibility";
const ::fwCom::Slots::SlotKeyType Reconstruction::s_UPDATE_NORMAL_MODE_SLOT = "updateNormalMode";

//------------------------------------------------------------------------------
Reconstruction::Reconstruction() throw() :
    m_sharpEdgeAngle(180.),
    m_autoResetCamera(true)
{
    newSlot(s_UPDATE_MESH_SLOT, &Reconstruction::updateMesh, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &Reconstruction::updateVisibility, this);
    newSlot(s_UPDATE_NORMAL_MODE_SLOT, &Reconstruction::updateNormalMode, this);
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction() throw()
{
}

//------------------------------------------------------------------------------

void Reconstruction::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doStart() throw(fwTools::Failed)
{
    this->createMeshService();
}

//------------------------------------------------------------------------------

void Reconstruction::createMeshService()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();

    ::fwData::Mesh::sptr mesh = reconstruction->getMesh();

    SLM_TRACE_IF("Mesh is null", !mesh);
    if (mesh)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService;
        meshService = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
            mesh,
            "::visuVTKAdaptor::Mesh" );
        SLM_ASSERT("meshService not instanced", meshService);
        ::visuVTKAdaptor::Mesh::sptr meshAdaptor = Mesh::dynamicCast(meshService);

        meshService->setRenderService( this->getRenderService() );
        meshService->setRenderId( this->getRenderId()      );
        meshService->setPickerId( this->getPickerId()      );
        meshService->setTransformId( this->getTransformId() );
        meshService->setAutoRender( this->getAutoRender() );

        meshAdaptor->setClippingPlanesId( m_clippingPlanesId             );
        //meshAdaptor->setSharpEdgeAngle  ( m_sharpEdgeAngle               );
        meshAdaptor->setShowClippedPart( true );
        meshAdaptor->setMaterial( reconstruction->getMaterial()  );
        meshAdaptor->setAutoResetCamera( m_autoResetCamera );
        meshService->start();
        meshAdaptor->updateVisibility( reconstruction->getIsVisible() );
        meshAdaptor->update();

        m_meshService = meshService;
        this->registerService(meshService);
        OSLM_TRACE("Mesh is visible : "<< reconstruction->getIsVisible());
        OSLM_TRACE("Mesh nb points : "<< mesh->getNumberOfPoints());
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Reconstruction::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService = m_meshService.lock();

        ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();
        ::visuVTKAdaptor::Mesh::sptr meshAdaptor      = Mesh::dynamicCast(meshService);
        //meshAdaptor->setSharpEdgeAngle( m_sharpEdgeAngle );

        meshAdaptor->setMaterial( reconstruction->getMaterial());
        meshAdaptor->swap( reconstruction->getMesh() );
        meshAdaptor->updateVisibility( reconstruction->getIsVisible());

    }
    else
    {
        this->createMeshService();
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeAllPropFromRenderer();

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Reconstruction::setForceHide(bool hide)
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();
    this->updateVisibility((hide ? false : reconstruction->getIsVisible()));
}

//------------------------------------------------------------------------------

void Reconstruction::updateVisibility(bool visible)
{
    SLM_TRACE_FUNC();
    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService = m_meshService.lock();
        ::visuVTKAdaptor::Mesh::sptr meshAdaptor
            = Mesh::dynamicCast(meshService);

        if (meshAdaptor)
        {
            meshAdaptor->updateVisibility( visible );
        }
        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void Reconstruction::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

void Reconstruction::updateMesh(SPTR(::fwData::Mesh))
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Reconstruction::updateNormalMode(std::uint8_t mode)
{
    if (!m_meshService.expired())
    {
        ::visuVTKAdaptor::Mesh::sptr meshAdaptor = ::visuVTKAdaptor::Mesh::dynamicCast(m_meshService.lock());
        if (meshAdaptor)
        {
            meshAdaptor->updateNormalMode(mode);
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Reconstruction::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Reconstruction::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Reconstruction::s_MESH_CHANGED_SIG, s_UPDATE_MESH_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG,
                                           s_UPDATE_VISIBILITY_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
