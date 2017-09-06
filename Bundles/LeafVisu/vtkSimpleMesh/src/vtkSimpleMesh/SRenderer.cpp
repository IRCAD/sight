/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "vtkSimpleMesh/SRenderer.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/HiResTimer.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkCamera.h>
#include <vtkCommand.h>
#ifndef ANDROID
#include <vtkInteractorStyleTrackballCamera.h>
#else
#include <vtkInteractorStyleMultiTouchCamera.h>
#endif
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRender::IRender, ::vtkSimpleMesh::SRenderer, ::fwData::Mesh );

namespace vtkSimpleMesh
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SRenderer::s_UPDATE_CAM_POSITION_SLOT = "updateCamPosition";
const ::fwCom::Slots::SlotKeyType SRenderer::s_UPDATE_PIPELINE_SLOT     = "updatePipeline";
const ::fwCom::Slots::SlotKeyType SRenderer::s_INIT_PIPELINE_SLOT       = "initPipeline";
const ::fwCom::Signals::SignalKeyType SRenderer::s_CAM_UPDATED_SIG      = "camUpdated";

static const std::string s_MESH_KEY = "mesh";

//-----------------------------------------------------------------------------

// vtkCommand used to catch the user interactions and notify the new camera position
class vtkLocalCommand : public vtkCommand
{
public:

    vtkLocalCommand(::vtkSimpleMesh::SRenderer* _service)
    {
        m_service              = _service;
        this->m_isMousePressed = false;
    }
    void Execute(vtkObject* _caller, unsigned long _event, void* _obj)
    {
        if (_event == vtkCommand::StartInteractionEvent )
        {
            this->m_isMousePressed = true;
        }
        else if (_event == vtkCommand::EndInteractionEvent )
        {
            this->m_isMousePressed = false;
        }
        else if ( (_event == vtkCommand::ModifiedEvent && this->m_isMousePressed)
                  || _event == vtkCommand::MouseWheelBackwardEvent || _event == vtkCommand::MouseWheelForwardEvent)
        {
            m_service->notifyCamPositionUpdated();
            ::fwThread::Worker::sptr worker = m_service->getWorker();
            worker->processTasks();
        }
    }
private:
    ::vtkSimpleMesh::SRenderer* m_service;
    bool m_isMousePressed;
};

//-----------------------------------------------------------------------------

SRenderer::SRenderer() noexcept :
    m_render( 0 ),
    m_bPipelineIsInit(false)
{
    newSlot(s_UPDATE_CAM_POSITION_SLOT, &SRenderer::updateCamPosition, this);
    newSlot(s_UPDATE_PIPELINE_SLOT, &SRenderer::updatePipeline, this);
    newSlot(s_INIT_PIPELINE_SLOT, &SRenderer::initPipeline, this);

    m_sigCamUpdated = newSignal<CamUpdatedSignalType>(s_CAM_UPDATED_SIG);
}

//-----------------------------------------------------------------------------

SRenderer::~SRenderer() noexcept
{
}

//-----------------------------------------------------------------------------

void SRenderer::starting()
{
    this->create();

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();

    m_interactorManager->installInteractor( this->getContainer() );

    m_bPipelineIsInit = false;

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);

    bool meshIsLoaded;
    {
        ::fwData::Mesh::csptr mesh = this->getInput< ::fwData::Mesh >(s_MESH_KEY);
        SLM_ASSERT("'" + s_MESH_KEY + "' key not found", mesh);
        ::fwData::mt::ObjectReadLock lock(mesh);
        meshIsLoaded = mesh->getNumberOfPoints() > 0;
    }

    if ( meshIsLoaded )
    {
        this->initVTKPipeline();
        m_bPipelineIsInit = true;
    }
}

//-----------------------------------------------------------------------------

void SRenderer::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SRenderer::stopping()
{
    if( m_render == 0 )
    {
        return;
    }

    m_interactorManager->getInteractor()->RemoveObserver(m_loc);

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();

    SLM_ASSERT("m_render not instanced", m_render);
    m_render->Delete();
    m_render = 0;

    this->destroy();
}

//-----------------------------------------------------------------------------

void SRenderer::updating()
{
    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void SRenderer::initVTKPipeline()
{
    ::fwData::Mesh::csptr mesh = this->getInput< ::fwData::Mesh >(s_MESH_KEY);
    SLM_ASSERT("'" + s_MESH_KEY + "' key not found", mesh);
    m_vtkPolyData = vtkSmartPointer<vtkPolyData>::New();

    {
        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, m_vtkPolyData );
    }

    m_mapper = vtkPolyDataMapper::New();
    m_mapper->SetInputData(m_vtkPolyData);

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(m_mapper);

    // Add the actors
    m_render->AddActor( actor);
#ifndef ANDROID
    m_interactorManager->getInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
#else
    m_interactorManager->getInteractor()->SetInteractorStyle(vtkInteractorStyleMultiTouchCamera::New());
#endif
    m_loc = new vtkLocalCommand(this);

    m_interactorManager->getInteractor()->AddObserver(vtkCommand::AnyEvent, m_loc);


    // Repaint and resize window
    m_render->ResetCamera();
}

//-----------------------------------------------------------------------------

void SRenderer::updateVTKPipeline(bool resetCamera)
{
    ::fwData::Mesh::csptr mesh = this->getInput< ::fwData::Mesh >(s_MESH_KEY);
    SLM_ASSERT("'" + s_MESH_KEY + "' key not found", mesh);

    {
        ::fwData::mt::ObjectReadLock lock(mesh);
        ::fwVtkIO::helper::Mesh::updatePolyDataPoints(m_vtkPolyData, mesh);
        ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_vtkPolyData, mesh);
        ::fwVtkIO::helper::Mesh::updatePolyDataPointColor(m_vtkPolyData, mesh);
        ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(m_vtkPolyData, mesh);
    }

    if (resetCamera)
    {
        m_render->ResetCamera();
    }

}

//-----------------------------------------------------------------------------

void SRenderer::notifyCamPositionUpdated()
{
    vtkCamera* camera = m_render->GetActiveCamera();

    SharedArray position = SharedArray(new double[3]);
    SharedArray focal    = SharedArray(new double[3]);
    SharedArray viewUp   = SharedArray(new double[3]);

    std::copy(camera->GetPosition(), camera->GetPosition()+3, position.get());
    std::copy(camera->GetFocalPoint(), camera->GetFocalPoint()+3, focal.get());
    std::copy(camera->GetViewUp(), camera->GetViewUp()+3, viewUp.get());

    {
        ::fwCom::Connection::Blocker block(m_sigCamUpdated->getConnection(this->slot(s_UPDATE_CAM_POSITION_SLOT)));
        m_sigCamUpdated->asyncEmit (position, focal, viewUp);
    }
}

//-----------------------------------------------------------------------------

void SRenderer::updateCamPosition(SharedArray positionValue, SharedArray focalValue, SharedArray viewUpValue)
{
    vtkCamera* camera = m_render->GetActiveCamera();

    camera->SetPosition(positionValue.get());
    camera->SetFocalPoint(focalValue.get());
    camera->SetViewUp(viewUpValue.get());
    camera->SetClippingRange(0.1, 1000000);

    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void SRenderer::initPipeline()
{
    if(!m_bPipelineIsInit)
    {
        this->initVTKPipeline();
        m_bPipelineIsInit = true;
    }
    else
    {
        m_vtkPolyData              = vtkSmartPointer<vtkPolyData>::New();
        ::fwData::Mesh::csptr mesh = this->getInput< ::fwData::Mesh >(s_MESH_KEY);
        SLM_ASSERT("'" + s_MESH_KEY + "' key not found", mesh);
        {
            ::fwData::mt::ObjectReadLock lock(mesh);
            ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, m_vtkPolyData );
        }
        m_mapper->SetInputData(m_vtkPolyData);
    }
    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void SRenderer::updatePipeline()
{
    m_hiResTimer.reset();
    m_hiResTimer.start();
    this->updateVTKPipeline(false);
    m_hiResTimer.stop();
    OSLM_INFO("Vertex updating time (milli sec) = " << m_hiResTimer.getElapsedTimeInMilliSec());

    m_hiResTimer.reset();
    m_hiResTimer.start();
    m_interactorManager->getInteractor()->Render();
    m_hiResTimer.stop();
    OSLM_INFO("Render time (milli sec) = " << m_hiResTimer.getElapsedTimeInMilliSec());
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SRenderer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MESH_KEY, ::fwData::Object::s_MODIFIED_SIG, s_INIT_PIPELINE_SLOT );
    connections.push( s_MESH_KEY, ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_UPDATE_PIPELINE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------
}
