/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SReconstruction.hpp"

#include "visuVTKAdaptor/SMesh.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SReconstruction);

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType SReconstruction::s_UPDATE_VISIBILITY_SLOT  = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SReconstruction::s_UPDATE_NORMAL_MODE_SLOT = "updateNormalMode";

const ::fwServices::IService::KeyType SReconstruction::s_RECONSTRUCTION_INPUT = "reconstruction";

//------------------------------------------------------------------------------
SReconstruction::SReconstruction() noexcept :
    m_sharpEdgeAngle(180.),
    m_autoResetCamera(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SReconstruction::updateVisibility, this);
    newSlot(s_UPDATE_NORMAL_MODE_SLOT, &SReconstruction::updateNormalMode, this);
}

//------------------------------------------------------------------------------

SReconstruction::~SReconstruction() noexcept
{
}

//------------------------------------------------------------------------------

void SReconstruction::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    const std::string autoresetcamera = config.get<std::string>("autoresetcamera", "yes");
    SLM_ASSERT("'autoresetcamera' value must be 'yes' or 'no', actual: " + autoresetcamera,
               autoresetcamera == "yes" || autoresetcamera == "no");
    m_autoResetCamera = (autoresetcamera == "yes");
}

//------------------------------------------------------------------------------

void SReconstruction::starting()
{
    this->initialize();
    this->createMeshService();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SReconstruction::createMeshService()
{
    ::fwData::Reconstruction::csptr reconstruction =
        this->getInput < ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
    SLM_ASSERT("Missing Reconstruction", reconstruction);

    ::fwData::Mesh::sptr mesh = reconstruction->getMesh();

    SLM_TRACE_IF("Mesh is null", !mesh);
    if (mesh)
    {
        // create the srv configuration for objects auto-connection
        IService::Config serviceConfig;
        ::fwRenderVTK::IAdaptor::sptr meshService = this->createSubAdaptor("::visuVTKAdaptor::SMesh", serviceConfig);
        this->registerServiceInput(mesh, SMesh::s_MESH_INPUT, meshService, true, serviceConfig);

        ::visuVTKAdaptor::SMesh::sptr meshAdaptor = SMesh::dynamicCast(meshService);

        meshService->setConfiguration(serviceConfig);
        meshService->setRenderService( this->getRenderService() );
        meshService->setRendererId( this->getRendererId() );
        meshService->setPickerId( this->getPickerId() );
        meshService->setTransformId( this->getTransformId() );
        meshService->setAutoRender( this->getAutoRender() );

        meshAdaptor->setClippingPlanesId( m_clippingPlanesId);
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

void SReconstruction::updating()
{
    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IAdaptor::sptr meshService = m_meshService.lock();

        ::fwData::Reconstruction::csptr reconstruction =
            this->getInput < ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
        SLM_ASSERT("Missing Reconstruction", reconstruction);
        ::visuVTKAdaptor::SMesh::sptr meshAdaptor = SMesh::dynamicCast(meshService);

        ::fwData::Mesh::csptr newMesh    = reconstruction->getMesh();
        ::fwData::Mesh::csptr oldMesh    = meshAdaptor->getInput< ::fwData::Mesh >(SMesh::s_MESH_INPUT);
        ::fwData::Material::csptr newMat = reconstruction->getMaterial();
        ::fwData::Material::csptr oldMat = meshAdaptor->getMaterial();

        // replace the mesh adaptor if the mesh or the material changed
        if (newMesh != oldMesh || newMat != oldMat)
        {
            this->unregisterService(meshAdaptor);
            this->createMeshService();
        }
        else
        {
            meshAdaptor->updateVisibility( reconstruction->getIsVisible());
        }
    }
    else
    {
        this->createMeshService();
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SReconstruction::stopping()
{
    this->removeAllPropFromRenderer();

    this->unregisterServices();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SReconstruction::setForceHide(bool hide)
{
    ::fwData::Reconstruction::csptr reconstruction =
        this->getInput < ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
    SLM_ASSERT("Missing Reconstruction", reconstruction);
    this->updateVisibility((hide ? false : reconstruction->getIsVisible()));
}

//------------------------------------------------------------------------------

void SReconstruction::updateVisibility(bool visible)
{
    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IAdaptor::sptr meshService = m_meshService.lock();
        ::visuVTKAdaptor::SMesh::sptr meshAdaptor
            = SMesh::dynamicCast(meshService);

        if (meshAdaptor)
        {
            meshAdaptor->updateVisibility( visible );
        }
    }
}

//------------------------------------------------------------------------------

void SReconstruction::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

void SReconstruction::updateNormalMode(std::uint8_t mode)
{
    if (!m_meshService.expired())
    {
        ::visuVTKAdaptor::SMesh::sptr meshAdaptor = ::visuVTKAdaptor::SMesh::dynamicCast(m_meshService.lock());
        if (meshAdaptor)
        {
            meshAdaptor->updateNormalMode(mode);
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SReconstruction::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION_INPUT, ::fwData::Reconstruction::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_RECONSTRUCTION_INPUT, ::fwData::Reconstruction::s_MESH_CHANGED_SIG, s_UPDATE_SLOT );
    connections.push(s_RECONSTRUCTION_INPUT, ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG,
                     s_UPDATE_VISIBILITY_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
