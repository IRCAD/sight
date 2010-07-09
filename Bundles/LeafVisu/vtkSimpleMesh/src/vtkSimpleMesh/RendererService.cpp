/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/panel.h>
#include <wx/window.h>

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

#include <fwData/TriangularMesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwComEd/CameraMsg.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include <vtkIO/vtk.hpp>
#include <fwGuiWx/container/WxContainer.hpp>

#include "vtkSimpleMesh/RendererService.hpp"

REGISTER_SERVICE( ::fwRender::IRender , ::vtkSimpleMesh::RendererService , ::fwData::TriangularMesh );



namespace vtkSimpleMesh
{

class vtkLocalCommand : public vtkCommand
{
public:

    vtkLocalCommand(::vtkSimpleMesh::RendererService* _service)
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
    ::vtkSimpleMesh::RendererService* m_service;
    bool m_isMousePressed;
};

RendererService::RendererService() throw()
: m_render( 0 ), m_bPipelineIsInit(false), m_isCamMaster(false)
{

}

//-----------------------------------------------------------------------------

RendererService::~RendererService() throw()
{
}

//-----------------------------------------------------------------------------

void RendererService::starting() throw(fwTools::Failed)
{
    this->create();

    m_bPipelineIsInit = false;

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container );
    m_wxmanager = new wxAuiManager( container );
    // Create a VTK-compliant window and insert it
    m_interactor = new ::wxVTKRenderWindowInteractor( container, -1 );
    m_wxmanager->AddPane( m_interactor, wxAuiPaneInfo().CentrePane() );
    // Repaint and resize window
    m_wxmanager->Update();

    // Renderer
    m_render = vtkRenderer::New();
    m_interactor->GetRenderWindow()->AddRenderer(m_render);
    // Repaint and resize window
    m_wxmanager->Update();
}

//-----------------------------------------------------------------------------

void RendererService::configuring() throw(::fwTools::Failed)
{
    if( m_configuration->findConfigurationElement("masterSlaveRelation") )
    {
        m_isCamMaster = ( m_configuration->findConfigurationElement("masterSlaveRelation")->getValue() == "master" );
    }
}

//-----------------------------------------------------------------------------

void RendererService::stopping() throw(fwTools::Failed)
{
    if( m_render == 0 ) return;

    if( m_wxmanager == 0 ) return;

    assert( m_interactor );
    if ( m_isCamMaster )
    {
        m_interactor->RemoveObserver(m_loc);
    }
    m_interactor->Delete();
    m_interactor = 0;

    assert( m_wxmanager );
    m_wxmanager->UnInit();
    delete m_wxmanager;
    m_wxmanager = 0;

    assert( m_render );
    m_render->Delete();
    m_render = 0;

    this->destroy();
}

//-----------------------------------------------------------------------------

void RendererService::updating() throw(fwTools::Failed)
{
    m_interactor->Render();
}

//-----------------------------------------------------------------------------

void RendererService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed)
{
    ::fwComEd::TriangularMeshMsg::csptr TriangularMeshMsg = ::fwComEd::TriangularMeshMsg::dynamicConstCast(_msg);
    if ( TriangularMeshMsg && TriangularMeshMsg->hasEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH ) )
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
    m_interactor->Render();
}

//-----------------------------------------------------------------------------

void RendererService::initVTKPipeline()
{
    vtkPolyData* vtk_polyData = ::vtkIO::toVTKMesh( this->getObject< ::fwData::TriangularMesh >());

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

    m_normals = vtkPolyDataNormals::New();
    m_normals->SetInput(vtk_polyData);
    mapper->SetInputConnection(m_normals->GetOutputPort());

    vtkActor* actor =  vtkActor::New();
    actor->SetMapper( mapper);

    // Add the actors
    m_render->AddActor( actor);

    m_interactor->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
    m_loc = new vtkLocalCommand(this);
    if ( m_isCamMaster )
    {
        m_interactor->AddObserver(vtkCommand::AnyEvent, m_loc);
    }
    // Repaint and resize window
    m_wxmanager->Update();
    m_render->ResetCamera();

    mapper->Delete();
}

//-----------------------------------------------------------------------------

void RendererService::updateVTKPipeline()
{
    assert(this->getObject< ::fwData::TriangularMesh >());
    m_normals->SetInput( ::vtkIO::toVTKMesh( this->getObject< ::fwData::TriangularMesh >() ));

    m_render->ResetCamera();

}

//-----------------------------------------------------------------------------

void RendererService::updateCamPosition()
{
    ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >();

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
