/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/com/Slots.hxx>
#include <core/tools/fwID.hpp>

#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <io/vtk/helper/Mesh.hpp>
#include <io/vtk/vtk.hpp>

#include <service/macros.hpp>

#include <vtkDecimatePro.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace sight::module::filter::mesh
{

//-----------------------------------------------------------------------------
static const sight::core::com::Slots::SlotKeyType s_UPDATE_THRESHOLD_SLOT = "updateThreshold";

//-----------------------------------------------------------------------------

SVTKMesher::SVTKMesher() noexcept
{
    newSlot(s_UPDATE_THRESHOLD_SLOT, &SVTKMesher::updateThreshold, this);
}

//-----------------------------------------------------------------------------

void SVTKMesher::starting()
{
}

//-----------------------------------------------------------------------------

void SVTKMesher::stopping()
{
    m_model = nullptr;
}

//-----------------------------------------------------------------------------

void SVTKMesher::configuring()
{
    const service::IService::ConfigType& srvConfig = this->getConfiguration();
    SIGHT_ASSERT("You must have one <config/> element.", srvConfig.count("config") == 1);
    const service::IService::ConfigType& config = srvConfig.get_child("config");
    m_threshold = config.get<unsigned int>("<xmlattr>.threshold");
    m_reduction = config.get<unsigned int>("<xmlattr>.percentReduction");
}

//-----------------------------------------------------------------------------

void SVTKMesher::updating()
{
    auto imageSeries = m_image.lock();

    auto modelSeries = data::ModelSeries::New();

    modelSeries->Series::deepCopy(imageSeries.get_shared());

    // vtk img
    auto vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(imageSeries.get_shared(), vtkImage);

    // contour filter
    auto contourFilter = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
    contourFilter->SetInputData(vtkImage);
    contourFilter->SetValue(0, m_threshold);
    contourFilter->ComputeScalarsOn();
    contourFilter->ComputeNormalsOn();
    contourFilter->Update();

    // smooth filter
    auto smoothFilter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    smoothFilter->SetInputConnection(contourFilter->GetOutputPort());
    smoothFilter->SetNumberOfIterations(50);
    smoothFilter->BoundarySmoothingOn();
    smoothFilter->SetPassBand(0.1);
    smoothFilter->SetFeatureAngle(120.0);
    smoothFilter->SetEdgeAngle(90);
    smoothFilter->FeatureEdgeSmoothingOn();
    smoothFilter->Update();

    // Get polyData
    vtkSmartPointer<vtkPolyData> polyData;
    auto mesh = data::Mesh::New();

    // decimate filter
    if(m_reduction > 0)
    {
        auto decimate = vtkSmartPointer<vtkDecimatePro>::New();
        decimate->SetInputConnection(smoothFilter->GetOutputPort());
        decimate->SetTargetReduction(m_reduction / 100.0);
        decimate->PreserveTopologyOff();
        decimate->SplittingOn();
        decimate->BoundaryVertexDeletionOn();
        decimate->SetSplitAngle(120);
        decimate->Update();
        polyData = decimate->GetOutput();
        io::vtk::helper::Mesh::fromVTKMesh(polyData, mesh);
    }
    else
    {
        polyData = smoothFilter->GetOutput();
        io::vtk::helper::Mesh::fromVTKMesh(polyData, mesh);
    }

    auto reconstruction = data::Reconstruction::New();

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

    m_model = modelSeries;
}

//------------------------------------------------------------------------------

void SVTKMesher::updateThreshold(int threshold)
{
    m_threshold = (threshold >= 0) ? static_cast<unsigned int>(threshold) : 0;
    this->update();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::mesh
