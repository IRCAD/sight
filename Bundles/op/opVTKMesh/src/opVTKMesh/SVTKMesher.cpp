/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opVTKMesh/SVTKMesher.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkDecimatePro.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace opVTKMesh
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::opVTKMesh::IMesher, ::opVTKMesh::SVTKMesher );

static const std::string s_IMAGE_INPUT  = "imageSeries";
static const std::string s_MODEL_OUTPUT = "modelSeries";

//-----------------------------------------------------------------------------

SVTKMesher::SVTKMesher() noexcept :
    m_reduction(0)
{
}

//-----------------------------------------------------------------------------

SVTKMesher::~SVTKMesher() noexcept
{
}

//-----------------------------------------------------------------------------

void SVTKMesher::starting()
{
}

//-----------------------------------------------------------------------------

void SVTKMesher::stopping()
{
}

//-----------------------------------------------------------------------------

void SVTKMesher::configuring()
{
    const ::fwServices::IService::ConfigType& srvConfig = this->getConfigTree();

    SLM_ASSERT("You must have one <config/> element.", srvConfig.count("config") == 1 );

    const ::fwServices::IService::ConfigType& config = srvConfig.get_child("config");

    SLM_ASSERT("You must have one <percentReduction/> element.", config.count("percentReduction") == 1);
    const ::fwServices::IService::ConfigType& reductionCfg = config.get_child("percentReduction");
    m_reduction = reductionCfg.get_value<unsigned int>();
}

//-----------------------------------------------------------------------------

void SVTKMesher::updating()
{
    ::fwMedData::ImageSeries::csptr imageSeries = this->getInput< ::fwMedData::ImageSeries >(s_IMAGE_INPUT);
    ::fwMedData::ModelSeries::sptr modelSeries  = ::fwMedData::ModelSeries::New();

    ::fwData::Object::DeepCopyCacheType cache;
    modelSeries->::fwMedData::Series::cachedDeepCopy(imageSeries, cache);

    // vtk img
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( imageSeries->getImage(), vtkImage );

    // contour filter
    vtkSmartPointer< vtkDiscreteMarchingCubes > contourFilter = vtkSmartPointer< vtkDiscreteMarchingCubes >::New();
    contourFilter->SetInputData(vtkImage);
    contourFilter->SetValue(0, 255);
    contourFilter->ComputeScalarsOn();
    contourFilter->ComputeNormalsOn();
    contourFilter->Update();

    // smooth filter
    vtkSmartPointer< vtkWindowedSincPolyDataFilter > smoothFilter =
        vtkSmartPointer< vtkWindowedSincPolyDataFilter >::New();
    smoothFilter->SetInputConnection(contourFilter->GetOutputPort());
    smoothFilter->SetNumberOfIterations( 50 );
    smoothFilter->BoundarySmoothingOn();
    smoothFilter->SetPassBand( 0.1 );
    smoothFilter->SetFeatureAngle(120.0);
    smoothFilter->SetEdgeAngle(90);
    smoothFilter->FeatureEdgeSmoothingOn();
    smoothFilter->Update();

    // Get polyData
    vtkSmartPointer< vtkPolyData > polyData;
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    // decimate filter
    if( m_reduction > 0 )
    {
        vtkSmartPointer< vtkDecimatePro > decimate = vtkSmartPointer< vtkDecimatePro >::New();
        decimate->SetInputConnection( smoothFilter->GetOutputPort() );
        decimate->SetTargetReduction( m_reduction/100.0 );
        decimate->PreserveTopologyOff();
        decimate->SplittingOn();
        decimate->BoundaryVertexDeletionOn();
        decimate->SetSplitAngle( 120 );
        decimate->Update();
        polyData = decimate->GetOutput();
        OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
        ::fwVtkIO::helper::Mesh::fromVTKMesh( polyData, mesh);
    }
    else
    {
        polyData = smoothFilter->GetOutput();
        OSLM_TRACE("final GetNumberOfCells = " << polyData->GetNumberOfCells());
        ::fwVtkIO::helper::Mesh::fromVTKMesh( polyData, mesh);
    }

    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

    static unsigned int organNumber = 0;
    ++organNumber;
    reconstruction->setOrganName("OrganMesher_VTK_" + std::to_string(organNumber));
    reconstruction->setStructureType("OrganType");
    reconstruction->setIsVisible(true);
    // Set Mesh
    reconstruction->setMesh(mesh);

    ::fwMedData::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
    recs.push_back(reconstruction);
    modelSeries->setReconstructionDB(recs);
    modelSeries->setDicomReference(imageSeries->getDicomReference());

    this->setOutput(s_MODEL_OUTPUT, modelSeries);
}

//-----------------------------------------------------------------------------

}
