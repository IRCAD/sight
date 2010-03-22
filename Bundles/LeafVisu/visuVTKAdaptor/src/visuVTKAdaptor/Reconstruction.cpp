/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
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
    addNewHandledEvent( ::fwComEd::ReconstructionMsg::MESH );
    addNewHandledEvent( ::fwComEd::ReconstructionMsg::VISIBILITY );
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction() throw()
{
}

//------------------------------------------------------------------------------

void Reconstruction::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId   ( m_configuration->getAttributeValue( "picker"    ) ) ;
    this->setRenderId   ( m_configuration->getAttributeValue( "renderer"  ) ) ;
    this->setTransformId( m_configuration->getAttributeValue( "transform" ) ) ;
}

//------------------------------------------------------------------------------

void Reconstruction::doStart() throw(fwTools::Failed)
{
    createMeshService();
}

void Reconstruction::createMeshService()
{
    SLM_TRACE_FUNC();


    ::fwData::Reconstruction::sptr reconstruction
        = this->getObject < ::fwData::Reconstruction >();


    ::fwRenderVTK::IVtkAdaptorService::sptr meshService;
    meshService = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
            reconstruction->getTriangularMesh(),
            "::visuVTKAdaptor::TriangularMesh" );
    assert(meshService);
    ::visuVTKAdaptor::TriangularMesh::sptr meshAdaptor
        = TriangularMesh::dynamicCast(meshService);

    meshService->setRenderService( this->getRenderService() );
    meshService->setRenderId     ( this->getRenderId()      );
    meshService->setPickerId     ( this->getPickerId()      );
    meshService->setTransformId  ( this->getTransformId()   );

    meshAdaptor->setClippingPlanesId( m_clippingPlanesId             );
    meshAdaptor->setSharpEdgeAngle  ( m_sharpEdgeAngle               );
    meshAdaptor->setShowClippedPart ( true );                        
    meshAdaptor->setMaterial        ( reconstruction->getMaterial()  );
    meshService->start();
    meshAdaptor ->updateVisibility  ( reconstruction->getIsVisible() );

    m_meshService = meshService;
    this->registerService(meshService);
}

//------------------------------------------------------------------------------

void Reconstruction::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING Reconstruction");
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Reconstruction::doUpdate() throw(fwTools::Failed)
{

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

    if (!m_meshService.expired())
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr meshService = m_meshService.lock();

        ::fwComEd::ReconstructionMsg::csptr msg = ::fwComEd::ReconstructionMsg::dynamicConstCast(_msg);

        assert(msg->getSubject().lock() == this->getObject());

        ::fwData::Reconstruction::sptr reconstruction = this->getObject < ::fwData::Reconstruction >();
        assert(reconstruction);

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

} //namespace visuVTKAdaptor
