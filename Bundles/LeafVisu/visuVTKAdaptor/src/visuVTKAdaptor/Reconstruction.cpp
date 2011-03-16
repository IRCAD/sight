/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwComEd/ReconstructionMsg.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkProperty.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataNormals.h>

#include "visuVTKAdaptor/TriangularMesh.hpp"
#include "visuVTKAdaptor/Reconstruction.hpp"

//VAG DEBUG
//#include <fwMath/MeshFunctions.hpp>
//#include <vtkIO/vtk.hpp>
//#include <vtkIO/MeshWriter.hpp>
//#include <boost/filesystem.hpp>



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Reconstruction, ::fwData::Reconstruction ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------
Reconstruction::Reconstruction() throw()
{
    m_clippingPlanesId = "";
    m_sharpEdgeAngle = 180;
    m_autoResetCamera = true;
    addNewHandledEvent( ::fwComEd::ReconstructionMsg::MESH );
    addNewHandledEvent( ::fwComEd::ReconstructionMsg::VISIBILITY );
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction() throw()
{}

//------------------------------------------------------------------------------

void Reconstruction::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId   ( m_configuration->getAttributeValue( "picker"    ) ) ;
    this->setRenderId   ( m_configuration->getAttributeValue( "renderer"  ) ) ;
    this->setTransformId( m_configuration->getAttributeValue( "transform" ) ) ;

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    createMeshService();
}

//------------------------------------------------------------------------------

void Reconstruction::createMeshService()
{
    SLM_TRACE_FUNC();
    ::fwData::Reconstruction::sptr reconstruction
        = this->getObject < ::fwData::Reconstruction >();

    ::fwData::TriangularMesh::sptr mesh = reconstruction->getTriangularMesh();

    SLM_TRACE_IF("Mesh is null", !mesh);
    if (mesh)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService;
        meshService = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
                mesh,
                "::visuVTKAdaptor::TriangularMesh" );
        SLM_ASSERT("meshService not instanced", meshService);
        ::visuVTKAdaptor::TriangularMesh::sptr meshAdaptor
            = TriangularMesh::dynamicCast(meshService);

        meshService->setRenderService( this->getRenderService() );
        meshService->setRenderId     ( this->getRenderId()      );
        meshService->setPickerId     ( this->getPickerId()      );
        meshService->setTransformId  ( this->getTransformId() );

        meshAdaptor->setClippingPlanesId( m_clippingPlanesId             );
        meshAdaptor->setSharpEdgeAngle  ( m_sharpEdgeAngle               );
        meshAdaptor->setShowClippedPart ( true );
        meshAdaptor->setMaterial        ( reconstruction->getMaterial()  );
        meshAdaptor->setAutoResetCamera ( m_autoResetCamera );
        meshService->start();
        meshAdaptor->updateVisibility  ( reconstruction->getIsVisible() );
        meshAdaptor->update();

        m_meshService = meshService;
        this->registerService(meshService);
        OSLM_TRACE("Mesh is visible : "<< reconstruction->getIsVisible());
        OSLM_TRACE("Mesh nb points : "<< mesh->getNumPoints());
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

        ::fwData::Reconstruction::sptr reconstruction
            = this->getObject < ::fwData::Reconstruction >();

        ::visuVTKAdaptor::TriangularMesh::sptr meshAdaptor
            = TriangularMesh::dynamicCast(meshService);

        /*
           OSLM_INFO( "Volume of " << reconstruction->getOrganName()
           << " : " << ::vtkIO::computeVolume(reconstruction->getTriangularMesh()) << " mm^3"
           << " isClosed " << ::fwMath::isBorderlessSurface(  reconstruction->getTriangularMesh()->cells() )
           << " nbHole " << contours.size()
           );
           for (int i=0; i<contours.size(); ++i )
           {
           OSLM_INFO("contour num=" << i << "contains " << contours[i].size()<< " edges ");
           }

           OSLM_INFO("CLOSESURFACE")
           ::fwMath::closeSurface(  reconstruction->getTriangularMesh()->points(), reconstruction->getTriangularMesh()->cells() );
           OSLM_INFO( "Volume of (closed) " << reconstruction->getOrganName()
           << " : " << ::vtkIO::computeVolume(reconstruction->getTriangularMesh()) << " mm^3"
           << " isClosed " << ::fwMath::isBorderlessSurface(  reconstruction->getTriangularMesh()->cells() )
           );

           ::vtkIO::MeshWriter writer;
           boost::filesystem::path dst("/tmp/" + reconstruction->getOrganName() + ".vtk");
           writer.setFile( dst );
           writer.setObject(  reconstruction->getTriangularMesh() );
           writer.write();
           */

        meshAdaptor->setSharpEdgeAngle( m_sharpEdgeAngle );

        meshAdaptor->setMaterial     ( reconstruction->getMaterial()       );
        meshAdaptor->swap            ( reconstruction->getTriangularMesh() );
        meshAdaptor->updateVisibility( reconstruction->getIsVisible()      );

    }
    else
    {
        this->createMeshService();
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService = m_meshService.lock();

        ::fwComEd::ReconstructionMsg::csptr msg = ::fwComEd::ReconstructionMsg::dynamicConstCast(_msg);

        assert(msg->getSubject().lock() == this->getObject());

        ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();
        SLM_ASSERT("reconstruction not instanced", reconstruction);

        if (msg && msg->hasEvent(::fwComEd::ReconstructionMsg::MESH))
        {
            //updateTriangulaMesh( reconstruction->getTriangularMesh() );
        }
        else if (msg && msg->hasEvent(::fwComEd::ReconstructionMsg::VISIBILITY))
        {

            this->setForceHide(!reconstruction->getIsVisible());
        }
    }
}

//------------------------------------------------------------------------------

void Reconstruction::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeAllPropFromRenderer();

    //if (this->getPicker())
    //{
        //this->removeFromPicker(this->getActor());
    //}

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Reconstruction::setForceHide(bool hide)
{
    SLM_TRACE_FUNC();
    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService = m_meshService.lock();
        ::visuVTKAdaptor::TriangularMesh::sptr meshAdaptor
            = TriangularMesh::dynamicCast(meshService);

        if (meshAdaptor)
        {
            ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();
            meshAdaptor->updateVisibility( hide ? false : reconstruction->getIsVisible() );
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

} //namespace visuVTKAdaptor
