/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Acquisition.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/AcquisitionMsg.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkThreshold.h>
#include <vtkPolyDataMapper.h>
#include <vtkDecimatePro.h>
#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "opVTKMesh/VTKMesherService.hpp"

namespace opVTKMesh
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::opVTKMesh::IMesher , ::opVTKMesh::VTKMesherService , ::fwData::Acquisition ) ;

//-----------------------------------------------------------------------------

VTKMesherService::VTKMesherService() throw() :
    m_reduction(0)
{}

//-----------------------------------------------------------------------------

VTKMesherService::~VTKMesherService() throw()
{}

//-----------------------------------------------------------------------------

void VTKMesherService::starting() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void VTKMesherService::stopping() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void VTKMesherService::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void VTKMesherService::configuring() throw ( ::fwTools::Failed )
{
    if (m_configuration->findConfigurationElement("percentReduction") &&
            m_configuration->findConfigurationElement("percentReduction")->hasAttribute("value"))
    {
        std::string reduce = m_configuration->findConfigurationElement("percentReduction")->getExistingAttributeValue("value");
        m_reduction = ::boost::lexical_cast<unsigned int>(reduce);
    }

    OSLM_INFO( "Reduction value = " << m_reduction);
}

//-----------------------------------------------------------------------------

void VTKMesherService::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    ::fwData::Image::sptr pImage = acq->getImage();

    ::fwData::TriangularMesh::NewSptr pMesh;

    // vtk img
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( pImage, vtkImage );

    // contour filter
    vtkSmartPointer< vtkDiscreteMarchingCubes > contourFilter = vtkSmartPointer< vtkDiscreteMarchingCubes >::New();
    contourFilter->SetInput(vtkImage);
    contourFilter->SetValue(0, 255);
    contourFilter->ComputeScalarsOn();
    contourFilter->ComputeNormalsOn();
    contourFilter->Update();

    // smooth filter
    vtkSmartPointer< vtkWindowedSincPolyDataFilter > smoothFilter = vtkSmartPointer< vtkWindowedSincPolyDataFilter >::New();
    smoothFilter->SetInput(contourFilter->GetOutput());
    smoothFilter->SetNumberOfIterations( 50 );
    smoothFilter->BoundarySmoothingOn();
    smoothFilter->SetPassBand ( 0.1 );
    smoothFilter->SetFeatureAngle(120.0);
    smoothFilter->SetEdgeAngle(90);
    smoothFilter->FeatureEdgeSmoothingOn();
    smoothFilter->Update();


    // Get polyData
      vtkPolyData * polyData;

      // decimate filter
      unsigned int reduction = m_reduction;
      if( reduction > 0 )
      {
          vtkSmartPointer< vtkDecimatePro > decimate = vtkSmartPointer< vtkDecimatePro >::New();
          decimate->SetInput( smoothFilter->GetOutput() );
          decimate->SetTargetReduction( reduction/100.0 );
          decimate->PreserveTopologyOff();
          decimate->SplittingOn();
          decimate->BoundaryVertexDeletionOn();
          decimate->SetSplitAngle( 120 );
          decimate->Update();
          polyData = decimate->GetOutput();
          OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
          bool res = ::vtkIO::fromVTKMesh( polyData, pMesh);
      }
      else
      {
          polyData = smoothFilter->GetOutput();
          OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
          bool res = ::vtkIO::fromVTKMesh( polyData, pMesh);
      }



    ::fwData::Reconstruction::NewSptr pReconstruction;

    pReconstruction->setCRefOrganName("OrganMesher_VTK_" + pReconstruction->getID());
    pReconstruction->setCRefStructureType("OrganType");
    pReconstruction->setIsVisible(true);
    pReconstruction->setGenerated3D(true);
    pReconstruction->setMaskGenerated(true);
    pReconstruction->setIsAutomatic(true);
    pReconstruction->setAvgVolume(-1);
    // Set Triangular Mesh
    pReconstruction->setTriangularMesh(pMesh);

    acq->addReconstruction(pReconstruction);

    /// Notification
    ::fwComEd::AcquisitionMsg::NewSptr msg;;
    msg->addEvent( ::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION ) ;
    ::fwServices::IEditionService::notify( this->getSptr(), acq, msg );
}

//-----------------------------------------------------------------------------

void VTKMesherService::info ( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
