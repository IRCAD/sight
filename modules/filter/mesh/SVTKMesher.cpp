/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/filter/mesh/SVTKMesher.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/fwID.hpp>

#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <services/macros.hpp>

#include <io/vtk/helper/Mesh.hpp>
#include <io/vtk/vtk.hpp>

#include <vtkDecimatePro.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace sight::modules::filter::mesh
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::services::IOperator, modules::filter::mesh::SVTKMesher )

static const std::string s_IMAGE_INPUT = "imageSeries";
static const std::string s_MODEL_OUTPUT = "modelSeries";

//-----------------------------------------------------------------------------

SVTKMesher::SVTKMesher() noexcept :
    m_reduction(0)
{
    this->registerObject(s_IMAGE_INPUT, services::IService::AccessType::INPUT, false);
    this->registerObject(s_MODEL_OUTPUT, services::IService::AccessType::OUTPUT, false, true);
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
    const services::IService::ConfigType& srvConfig = this->getConfigTree();

    SLM_ASSERT("You must have one <config/> element.", srvConfig.count("config") == 1 );

    const services::IService::ConfigType& config = srvConfig.get_child("config");

    SLM_ASSERT("You must have one <percentReduction/> element.", config.count("percentReduction") == 1);
    const services::IService::ConfigType& reductionCfg = config.get_child("percentReduction");
    m_reduction = reductionCfg.get_value<unsigned int>();
}

//-----------------------------------------------------------------------------

void SVTKMesher::updating()
{
    data::ImageSeries::csptr imageSeries = this->getInput< data::ImageSeries >(s_IMAGE_INPUT);
    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();

    data::Object::DeepCopyCacheType cache;
    modelSeries->data::Series::cachedDeepCopy(imageSeries, cache);

    // vtk img
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    io::vtk::toVTKImage( imageSeries->getImage(), vtkImage );

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
    data::Mesh::sptr mesh = data::Mesh::New();

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
        io::vtk::helper::Mesh::fromVTKMesh( polyData, mesh);
    }
    else
    {
        polyData = smoothFilter->GetOutput();
        io::vtk::helper::Mesh::fromVTKMesh( polyData, mesh);
    }

    data::Reconstruction::sptr reconstruction = data::Reconstruction::New();

    static unsigned int organNumber = 0;
    ++organNumber;
    reconstruction->setOrganName("OrganMesher_VTK_" + std::to_string(organNumber));
    reconstruction->setStructureType("OrganType");
    reconstruction->setIsVisible(true);
    // Set Mesh
    reconstruction->setMesh(mesh);

    data::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
    recs.push_back(reconstruction);
    modelSeries->setReconstructionDB(recs);
    modelSeries->setDicomReference(imageSeries->getDicomReference());

    this->setOutput(s_MODEL_OUTPUT, modelSeries);
}

//-----------------------------------------------------------------------------

}
