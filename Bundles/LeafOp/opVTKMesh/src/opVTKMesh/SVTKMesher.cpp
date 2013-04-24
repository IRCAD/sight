/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ModelSeriesMsg.hpp>

#include <vtkIO/helper/Mesh.hpp>
#include <vtkIO/vtk.hpp>

#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkDecimatePro.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "opVTKMesh/SVTKMesher.hpp"

namespace opVTKMesh
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::opVTKMesh::IMesher , ::opVTKMesh::SVTKMesher , ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SVTKMesher::SVTKMesher() throw() :
    m_reduction(0)
{}

//-----------------------------------------------------------------------------

SVTKMesher::~SVTKMesher() throw()
{}

//-----------------------------------------------------------------------------

void SVTKMesher::starting() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SVTKMesher::stopping() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SVTKMesher::receiving( ::fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SVTKMesher::configuring() throw ( ::fwTools::Failed )
{
    const ::fwServices::IService::ConfigType& config = this->getConfigTree();

    SLM_ASSERT("You must have one <config/> element.", config.get_child("service").count("config") == 1 );

    SLM_ASSERT("You must have one <percentReduction/> element.", config.count("percentReduction") == 1);
    SLM_ASSERT("You must have one <image/> element.", config.count("image") == 1);
    SLM_ASSERT("You must have one <modelSeries/> element.", config.count("modelSeries") == 1);

    const ::fwServices::IService::ConfigType& reductionCfg = config.get_child("percentReduction");
    const ::fwServices::IService::ConfigType& imageCfg = config.get_child("image");
    const ::fwServices::IService::ConfigType& modelSeriesCfg = config.get_child("modelSeries");

    m_reduction = reductionCfg.get_value<unsigned int>();
    m_imageKey = imageCfg.get_value<std::string>();
    m_modelSeriesKey = modelSeriesCfg.get_value<std::string>();
}

//-----------------------------------------------------------------------------

void SVTKMesher::updating() throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwData::Composite::iterator iterImg = composite->find(m_imageKey);
    ::fwData::Composite::iterator iterModelSeries = composite->find(m_modelSeriesKey);

    SLM_ASSERT("Key '"+m_imageKey+"' not found in composite.", iterImg != composite->end());
    SLM_ASSERT("Key '"+m_modelSeriesKey+"' not found in composite.", iterModelSeries != composite->end());

    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(iterImg->second);
    SLM_ASSERT("Image '"+m_imageKey+"' is not valid.", image);

    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(iterModelSeries->second);
    SLM_ASSERT("ModelSeries '"+m_modelSeriesKey+"' is not valid.", modelSeries);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    // vtk img
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( image, vtkImage );

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
    vtkSmartPointer< vtkPolyData > polyData;

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
        ::vtkIO::helper::Mesh::fromVTKMesh( polyData, mesh);
    }
    else
    {
        polyData = smoothFilter->GetOutput();
        OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
        ::vtkIO::helper::Mesh::fromVTKMesh( polyData, mesh);
    }



    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

    reconstruction->setCRefOrganName("OrganMesher_VTK_" + reconstruction->getID());
    reconstruction->setCRefStructureType("OrganType");
    reconstruction->setIsVisible(true);
    reconstruction->setGenerated3D(true);
    reconstruction->setMaskGenerated(true);
    reconstruction->setIsAutomatic(true);
    reconstruction->setAvgVolume(-1);
    // Set Triangular Mesh
    reconstruction->setMesh(mesh);

    ::fwMedData::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
    recs.push_back(reconstruction);
    modelSeries->setReconstructionDB(recs);

    /// Notification
    ::fwComEd::ModelSeriesMsg::sptr msg;
    msg->addEvent( ::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION ) ;
    ::fwServices::IEditionService::notify( this->getSptr(), modelSeries, msg );
}

//-----------------------------------------------------------------------------

void SVTKMesher::info ( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
