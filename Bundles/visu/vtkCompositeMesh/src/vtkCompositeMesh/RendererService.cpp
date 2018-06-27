/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "vtkCompositeMesh/RendererService.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRender::IRender, ::vtkCompositeMesh::RendererService, ::fwData::Composite );

namespace vtkCompositeMesh
{

const ::fwCom::Slots::SlotKeyType RendererService::s_UPDATE_CAM_POSITION_SLOT = "updateCamPosition";
const ::fwCom::Slots::SlotKeyType RendererService::s_UPDATE_PIPELINE_SLOT     = "updatePipeline";
const ::fwCom::Signals::SignalKeyType RendererService::s_CAM_UPDATED_SIG      = "camUpdated";

class vtkLocalCommand : public vtkCommand
{
public:

    vtkLocalCommand(::vtkCompositeMesh::RendererService* _service)
    {
        m_service              = _service;
        this->m_isMousePressed = false;
    }
    //------------------------------------------------------------------------------

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
    ::vtkCompositeMesh::RendererService* m_service;
    bool m_isMousePressed;
};

RendererService::RendererService() noexcept :
    m_render( 0 ),
    m_bPipelineIsInit(false)
{
    m_slotUpdateCamPosition = newSlot( s_UPDATE_CAM_POSITION_SLOT, &RendererService::updateCamPosition, this );
    newSlot(s_UPDATE_PIPELINE_SLOT, &RendererService::updatePipeline, this);

    m_sigCamUpdated = CamUpdatedSignalType::New();

    // Register
    ::fwCom::HasSignals::m_signals( s_CAM_UPDATED_SIG,  m_sigCamUpdated);

}

//-----------------------------------------------------------------------------

RendererService::~RendererService() noexcept
{
}

//-----------------------------------------------------------------------------

void RendererService::starting()
{
    FW_DEPRECATED_MSG("This service is no longer supported.", "18.0");

    this->IGuiContainerSrv::create();

    m_bPipelineIsInit = false;

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);
}

//-----------------------------------------------------------------------------

void RendererService::configuring()
{
    this->IGuiContainerSrv::initialize();
}

//-----------------------------------------------------------------------------

void RendererService::stopping()
{
    if( m_render == 0 )
    {
        return;
    }

    assert( m_interactorManager->getInteractor() );
    m_interactorManager->getInteractor()->RemoveObserver(m_loc);

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();

    SLM_ASSERT("m_render not instanced", m_render);
    m_render->Delete();
    m_render = 0;

    this->IGuiContainerSrv::destroy();
}

//-----------------------------------------------------------------------------

void RendererService::updating()
{
    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void RendererService::updateCamPosition(SharedArray positionValue,
                                        SharedArray focalValue,
                                        SharedArray viewUpValue)
{
    vtkCamera* camera = m_render->GetActiveCamera();

    camera->SetPosition(positionValue.get());
    camera->SetFocalPoint(focalValue.get());
    camera->SetViewUp(viewUpValue.get());
    camera->SetClippingRange(0.1, 1000000);

    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void RendererService::initVTKPipeline()
{
    //Create the view:
    createAndAddActorToRender();
}

//-----------------------------------------------------------------------------
void RendererService::createAndAddActorToRender()
{
    //Check there is indeed a Composite object in this:
    assert(this->getObject< ::fwData::Composite >());

    //Create a pointer on this object:
    ::fwData::Composite::sptr myComposite = this->getObject< ::fwData::Composite >();

    OSLM_INFO( "VTK Pipeline ready TO UPDATE" << '\n' << "Object received:" << myComposite->getLeafClassname());

    // elementnumber increases for each mesh found:
    unsigned int elementNumber = 0;

    //Loop through the composite objects, if it's a mesh, then render it:
    for(::fwData::Composite::ContainerType::const_iterator it = myComposite->begin(); it != myComposite->end(); ++it)
    {
        OSLM_INFO("ObjectName: " << it->first);
        OSLM_INFO("ObjectPointer: " << it->second);
        OSLM_INFO("ObjectType: " << it->second->getClassname() << '\n');

        ::fwData::Mesh::sptr myMesh = ::fwData::Mesh::dynamicCast(it->second);

        // If it's a mesh, then put it in the pipeline:
        if( myMesh )
        {
            vtkSmartPointer<vtkPolyData> vtk_polyData = vtkSmartPointer<vtkPolyData>::New();
            ::fwVtkIO::helper::Mesh::toVTKMesh( myMesh, vtk_polyData);
            OSLM_INFO("Loaded: " << it->first);
            vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

            m_normals = vtkPolyDataNormals::New();
            m_normals->SetInputData(vtk_polyData);
            mapper->SetInputConnection(m_normals->GetOutputPort());

            vtkActor* actor = vtkActor::New();
            actor->SetMapper( mapper);

            // Add the actors
            m_render->AddActor( actor);

            OSLM_INFO("Mesh found: " << it->first);
            if (!myMesh->getField( "MaterialMesh" ))
            {
                // No Material data then default
                if(elementNumber == 0)
                {
                    actor->GetProperty()->EdgeVisibilityOn();
                    actor->GetProperty()->SetInterpolationToFlat();
                    actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
                    actor->GetProperty()->SetEdgeColor(1.0, 0.0, 0.0);
                }
                else
                {
                    actor->GetProperty()->SetRepresentationToWireframe();
                    actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
                }
            }
            else
            {
                // Material exists
                ::fwData::Material::sptr matObjPtr = myMesh->getField< ::fwData::Material >( "MaterialMesh" );
                actor->GetProperty()->SetColor(matObjPtr->diffuse()->red(),
                                               matObjPtr->diffuse()->green(), matObjPtr->diffuse()->blue());
            }
            mapper->Delete();
            elementNumber++;
            OSLM_INFO("displayed: " << it->first);
        }
    }
    m_interactorManager->getInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
    m_loc = new vtkLocalCommand(this);
    m_interactorManager->getInteractor()->AddObserver(vtkCommand::AnyEvent, m_loc);

    // Repaint and resize window
    m_render->ResetCamera();
}

//-----------------------------------------------------------------------------

void RendererService::updateVTKPipeline()
{
    //Clear the view:
    m_render->RemoveAllViewProps();
    //Recreate the view:
    createAndAddActorToRender();
}

//-----------------------------------------------------------------------------

void RendererService::updatePipeline()
{
    if(!m_bPipelineIsInit)
    {
        this->initVTKPipeline();
        m_bPipelineIsInit = true;
    }
    else
    {
        this->updateVTKPipeline();
    }
    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void RendererService::notifyCamPositionUpdated()
{
    vtkCamera* camera = m_render->GetActiveCamera();

    SharedArray position = SharedArray(new double[3]);
    SharedArray focal    = SharedArray(new double[3]);
    SharedArray viewUp   = SharedArray(new double[3]);

    std::copy(camera->GetPosition(), camera->GetPosition()+3, position.get());
    std::copy(camera->GetFocalPoint(), camera->GetFocalPoint()+3, focal.get());
    std::copy(camera->GetViewUp(), camera->GetViewUp()+3, viewUp.get());

    {
        ::fwCom::Connection::Blocker block(m_sigCamUpdated->getConnection(m_slotUpdateCamPosition));
        m_sigCamUpdated->asyncEmit(position, focal, viewUp);
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType RendererService::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_PIPELINE_SLOT ) );
    return connections;
}

}
