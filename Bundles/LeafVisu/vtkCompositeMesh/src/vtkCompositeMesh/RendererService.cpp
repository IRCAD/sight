/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataNormals.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkTransform.h>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Material.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwComEd/CameraMsg.hpp>
#include <fwComEd/MeshMsg.hpp>


#include <vtkIO/helper/Mesh.hpp>
#include <vtkIO/vtk.hpp>

#include "vtkCompositeMesh/RendererService.hpp"


fwServicesRegisterMacro( ::fwRender::IRender , ::vtkCompositeMesh::RendererService , ::fwData::Composite );



namespace vtkCompositeMesh
{

const ::fwCom::Slots::SlotKeyType RendererService::s_UPDATE_CAM_POSITION_SLOT = "updateCamPosition";
const ::fwCom::Signals::SignalKeyType RendererService::s_CAM_UPDATED_SIG = "camUpdated";

class vtkLocalCommand : public vtkCommand
{
public:

    vtkLocalCommand(::vtkCompositeMesh::RendererService* _service)
    {
        m_service = _service;
        this->m_isMousePressed = false;
    }
    void Execute(vtkObject* _caller, unsigned long _event, void* _obj)
    {
        //OSLM_INFO("ail like to LEFT : _event " << _event );

        if (_event == vtkCommand::StartInteractionEvent )
        {
            //SLM_INFO(" ________________START___________________________");
            this->m_isMousePressed = true;
        }
        else if (_event == vtkCommand::EndInteractionEvent )
        {
            //SLM_INFO(" ________________END___________________________");
            this->m_isMousePressed = false;
        }
        else if ( (_event == vtkCommand::ModifiedEvent && this->m_isMousePressed)
                || _event == vtkCommand::MouseWheelBackwardEvent || _event == vtkCommand::MouseWheelForwardEvent)
        {
            //SLM_INFO(" ______________________________________________");
            m_service->notifyCamPositionUpdated();
        }
    }
private:
    ::vtkCompositeMesh::RendererService* m_service;
    bool m_isMousePressed;
};

RendererService::RendererService() throw()
                                    : m_render( 0 ), m_bPipelineIsInit(false)
{
//    this->IService::addNewHandledEvent( ::fwComEd::MeshMsg::NEW_MESH );
//    this->IService::addNewHandledEvent( ::fwComEd::CameraMsg::CAMERA_MOVING );

    m_slotUpdateCamPosition   = ::fwCom::newSlot( &RendererService::updateCamPosition, this ) ;
    ::fwCom::HasSlots::m_slots( s_UPDATE_CAM_POSITION_SLOT   , m_slotUpdateCamPosition );

    m_sigCamUpdated = CamUpdatedSignalType::New();
#ifdef COM_LOG
    m_sigCamUpdated->setID( s_CAM_UPDATED_SIG );
#endif
    // Register
    ::fwCom::HasSignals::m_signals( s_CAM_UPDATED_SIG,  m_sigCamUpdated);

    this->setWorker( ::fwServices::registry::ActiveWorkers::getDefault()->
                                 getWorker( ::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER ) );

}

//-----------------------------------------------------------------------------

RendererService::~RendererService() throw()
{}

//-----------------------------------------------------------------------------

void RendererService::starting() throw(fwTools::Failed)
{
    this->IGuiContainerSrv::create();

    m_bPipelineIsInit = false;

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);
}

//-----------------------------------------------------------------------------

void RendererService::configuring() throw(::fwTools::Failed)
{
    this->IGuiContainerSrv::initialize();
}

//-----------------------------------------------------------------------------

void RendererService::stopping() throw(fwTools::Failed)
{
    if( m_render == 0 ) return;

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

void RendererService::updating() throw(fwTools::Failed)
{
    m_interactorManager->getInteractor()->Render();
}

//-----------------------------------------------------------------------------

void RendererService::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed)
{
    ::fwComEd::MeshMsg::csptr meshMsg = ::fwComEd::MeshMsg::dynamicConstCast(_msg);
    if ( meshMsg && meshMsg->hasEvent( ::fwComEd::MeshMsg::NEW_MESH ) )
    {
        if(!m_bPipelineIsInit)
        {
            initVTKPipeline();
            m_bPipelineIsInit = true;
        }
        else
        {
            updateVTKPipeline();
        }
        m_interactorManager->getInteractor()->Render();
    }
}

//-----------------------------------------------------------------------------

void RendererService::updateCamPosition(const double positionValue[3], const double focalValue[3], const double viewUpValue[3] )
{
    vtkCamera* camera = m_render->GetActiveCamera();

    camera->SetPosition(positionValue);
    camera->SetFocalPoint(focalValue);
    camera->SetViewUp(viewUpValue);
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
    bool flagMaterialExist = false;
    //Check there is indeed a Composite object in this:
    assert(this->getObject< ::fwData::Composite >());

    //Create a pointer on this object:
    ::fwData::Composite::sptr myComposite = this->getObject< ::fwData::Composite >();

    OSLM_INFO( "VTK Pipeline ready TO UPDATE" << '\n' << "Object received:" << myComposite->getLeafClassname());

    // elementnumber increases for each mesh found:
    unsigned int elementNumber=0;

    //Loop through the composite objects, if it's a mesh, then render it:
    for(::fwData::Composite::ContainerType::const_iterator it = myComposite->begin(); it != myComposite->end(); ++it)
    {
        OSLM_INFO("ObjectName: " << it->first);
        OSLM_INFO("ObjectPointer: " << it->second);
        OSLM_INFO("ObjectType: " << it->second->getClassname () << '\n');

        ::fwData::Mesh::sptr myMesh =::fwData::Mesh::dynamicCast (it->second);

        // If it's a mesh, then put it in the pipeline:
        if( myMesh )
        {
            vtkSmartPointer<vtkPolyData> vtk_polyData = vtkSmartPointer<vtkPolyData>::New();
            ::vtkIO::helper::Mesh::toVTKMesh( myMesh, vtk_polyData);
            OSLM_INFO("Loaded: " << it->first);
            vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

            m_normals = vtkPolyDataNormals::New();
            m_normals->SetInput(vtk_polyData);
            mapper->SetInputConnection(m_normals->GetOutputPort());

            vtkActor* actor =  vtkActor::New();
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
                    actor->GetProperty()->SetColor (1.0, 0.0, 0.0);
                    actor->GetProperty()->SetEdgeColor (1.0, 0.0, 0.0);
                }
                else
                {
                    actor->GetProperty()->SetRepresentationToWireframe ();
                    actor->GetProperty()->SetColor (1.0, 1.0, 1.0);
                }
            }
            else
            {
                // Material exists
                ::fwData::Material::sptr matObjPtr = myMesh->getField< ::fwData::Material >( "MaterialMesh" );
                actor->GetProperty()->SetColor (matObjPtr->ambient()->red(), matObjPtr->ambient()->green(), matObjPtr->ambient()->blue());
            }
            mapper->Delete();
        }
        m_interactorManager->getInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
        m_loc = new vtkLocalCommand(this);
        m_interactorManager->getInteractor()->AddObserver(vtkCommand::AnyEvent, m_loc);

        // Repaint and resize window
        m_render->ResetCamera();
        OSLM_INFO("displayed: " << it->first);

        elementNumber++;
    }
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

void RendererService::notifyCamPositionUpdated()
{
    vtkCamera* camera = m_render->GetActiveCamera();

    const double * position = camera->GetPosition();
    const double * focal = camera->GetFocalPoint();
    const double * viewUp = camera->GetViewUp();

//    fwServicesBlockAndNotifyMacro( this->getLightID(), m_sigCamUpdated,
//                                   (position, focal, viewUp),
//                                   m_slotUpdateCamPosition );

    ::fwCom::Connection::Blocker block(m_sigCamUpdated->getConnection(m_slotUpdateCamPosition));
    m_sigCamUpdated->emit(position, focal, viewUp);
}

}
