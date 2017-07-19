/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opVTKMesh/SVTKMesher.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkDecimatePro.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>


namespace opVTKMesh
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::opVTKMesh::IMesher, ::opVTKMesh::SVTKMesher, ::fwData::Composite );

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

    SLM_ASSERT("You must have one <config/> element.", srvConfig.get_child("service").count("config") == 1 );

    const ::fwServices::IService::ConfigType& config = srvConfig.get_child("service.config");

    SLM_ASSERT("You must have one <percentReduction/> element.", config.count("percentReduction") == 1);
    const ::fwServices::IService::ConfigType& reductionCfg = config.get_child("percentReduction");
    m_reduction = reductionCfg.get_value<unsigned int>();

    if(!this->isVersion2())
    {
        SLM_ASSERT("You must have one <image/> element.", config.count("image") == 1);
        SLM_ASSERT("You must have one <modelSeries/> element.", config.count("modelSeries") == 1);
        const ::fwServices::IService::ConfigType& imageCfg       = config.get_child("image");
        const ::fwServices::IService::ConfigType& modelSeriesCfg = config.get_child("modelSeries");
        m_imageKey       = imageCfg.get_value<std::string>();
        m_modelSeriesKey = modelSeriesCfg.get_value<std::string>();
    }
}

//-----------------------------------------------------------------------------

void SVTKMesher::updating()
{
    ::fwData::Image::csptr image;
    ::fwMedData::ModelSeries::sptr modelSeries;

    if(this->isVersion2())
    {
        image       = this->getInput< ::fwData::Image >("image");
        modelSeries = this->getInOut< ::fwMedData::ModelSeries >("modelSeries");
    }
    else
    {
        ::fwData::Composite::sptr composite           = this->getObject< ::fwData::Composite >();
        ::fwData::Composite::iterator iterImg         = composite->find(m_imageKey);
        ::fwData::Composite::iterator iterModelSeries = composite->find(m_modelSeriesKey);

        SLM_ASSERT("Key '"+m_imageKey+"' not found in composite.", iterImg != composite->end());
        SLM_ASSERT("Key '"+m_modelSeriesKey+"' not found in composite.", iterModelSeries != composite->end());

        image = ::fwData::Image::dynamicCast(iterImg->second);
        SLM_ASSERT("Image '"+m_imageKey+"' is not valid.", image);

        modelSeries = ::fwMedData::ModelSeries::dynamicCast(iterModelSeries->second);
        SLM_ASSERT("ModelSeries '"+m_modelSeriesKey+"' is not valid.", modelSeries);
    }

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    // vtk img
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( image, vtkImage );

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
        decimate->SetInputConnection( smoothFilter->GetOutputPort() );
        decimate->SetTargetReduction( reduction/100.0 );
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

    // Notification
    ::fwMedData::ModelSeries::ReconstructionVectorType addedRecs;
    addedRecs.push_back(reconstruction);
    auto sig = modelSeries->signal< ::fwMedData::ModelSeries::ReconstructionsAddedSignalType >(
        ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG);
    sig->asyncEmit(addedRecs);
}

//-----------------------------------------------------------------------------

}
