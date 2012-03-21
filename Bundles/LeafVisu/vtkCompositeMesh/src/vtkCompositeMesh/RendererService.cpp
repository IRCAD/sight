/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

#include <fwData/Mesh.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Material.hpp>

#include <fwComEd/CameraMsg.hpp>
#include <fwComEd/MeshMsg.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <vtkIO/helper/Mesh.hpp>
#include <vtkIO/vtk.hpp>

#include "vtkCompositeMesh/RendererService.hpp"


REGISTER_SERVICE( ::fwRender::IRender , ::vtkCompositeMesh::RendererService , ::fwData::Composite );



namespace vtkCompositeMesh
{
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
            m_service->updateCamPosition();
        }
    }
private:
    ::vtkCompositeMesh::RendererService* m_service;
    bool m_isMousePressed;
};

RendererService::RendererService() throw()
                                    : m_render( 0 ), m_bPipelineIsInit(false), m_isCamMaster(false)
{
    this->IService::addNewHandledEvent( ::fwComEd::MeshMsg::NEW_MESH );
    this->IService::addNewHandledEvent( ::fwComEd::CameraMsg::CAMERA_MOVING );
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
    if( m_configuration->findConfigurationElement("masterSlaveRelation") )
    {
        m_isCamMaster = ( m_configuration->findConfigurationElement("masterSlaveRelation")->getValue() == "master" );
    }
}

//-----------------------------------------------------------------------------

void RendererService::stopping() throw(fwTools::Failed)
{
    if( m_render == 0 ) return;

    assert( m_interactorManager->getInteractor() );
    if ( m_isCamMaster )
    {
        m_interactorManager->getInteractor()->RemoveObserver(m_loc);
    }

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

void RendererService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed)
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
    }
    else
    {
        if ( !m_isCamMaster )
        {
            ::fwComEd::CameraMsg::csptr camMsg = ::fwComEd::CameraMsg::dynamicConstCast(_msg);
            if( camMsg && camMsg->hasEvent( ::fwComEd::CameraMsg::CAMERA_MOVING ) )
            {
                vtkCamera* camera = m_render->GetActiveCamera();

                camera->SetPosition(camMsg->getPositionCamera());
                camera->SetFocalPoint(camMsg->getFocalCamera());
                camera->SetViewUp(camMsg->getViewUpCamera());
                camera->SetClippingRange(0.1, 1000000);
            }
        }
    }
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
    for(::fwData::Composite::Container::const_iterator it = myComposite->begin(); it != myComposite->end(); ++it)
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
            if (myMesh->getField( "MaterialMesh" ))
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
        if ( m_isCamMaster )
        {
            m_interactorManager->getInteractor()->AddObserver(vtkCommand::AnyEvent, m_loc);
        }

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

void RendererService::updateCamPosition()
{
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();

    vtkCamera* camera = m_render->GetActiveCamera();

    // Prepare message to be fired according to position modification
    ::fwComEd::CameraMsg::NewSptr camMsg;
    camMsg->addEvent( ::fwComEd::CameraMsg::CAMERA_MOVING );
    camMsg->setPositionCamera(camera->GetPosition());
    camMsg->setFocalCamera(camera->GetFocalPoint());
    camMsg->setViewUpCamera(camera->GetViewUp());

    ::fwServices::IEditionService::notify(this->getSptr(), mesh, camMsg);
}

}
