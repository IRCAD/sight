/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modules/filter/mesh/vtk_mesher.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/helper/mesh.hpp>
#include <io/vtk/vtk.hpp>

#include <vtkDecimatePro.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace sight::module::filter::mesh
{

//-----------------------------------------------------------------------------

static const sight::core::com::slots::key_t UPDATE_THRESHOLD_SLOT = "update_threshold";

//-----------------------------------------------------------------------------

vtk_mesher::vtk_mesher() noexcept :
    filter(m_signals)
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::starting()
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::stopping()
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::configuring()
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::updating()
{
    auto image_series = m_image.lock();

    auto model_series = std::make_shared<data::model_series>();

    model_series->series::deep_copy(image_series.get_shared());

    // vtk img
    auto vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image_series.get_shared(), vtk_image);

    // contour filter
    auto contour_filter = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
    contour_filter->SetInputData(vtk_image);
    contour_filter->SetValue(0, std::max(static_cast<unsigned int>(*m_threshold), 0U));
    contour_filter->ComputeScalarsOn();
    contour_filter->ComputeNormalsOn();
    contour_filter->Update();

    // smooth filter
    auto smooth_filter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    smooth_filter->SetInputConnection(contour_filter->GetOutputPort());
    smooth_filter->SetNumberOfIterations(50);
    smooth_filter->BoundarySmoothingOn();
    smooth_filter->SetPassBand(0.1);
    smooth_filter->SetFeatureAngle(120.0);
    smooth_filter->SetEdgeAngle(90);
    smooth_filter->FeatureEdgeSmoothingOn();
    smooth_filter->Update();

    // Get polyData
    vtkSmartPointer<vtkPolyData> poly_data;
    auto mesh = std::make_shared<data::mesh>();

    // decimate filter
    if(*m_reduction > 0)
    {
        auto decimate = vtkSmartPointer<vtkDecimatePro>::New();
        decimate->SetInputConnection(smooth_filter->GetOutputPort());
        decimate->SetTargetReduction(*m_reduction / 100.0);
        decimate->PreserveTopologyOff();
        decimate->SplittingOn();
        decimate->BoundaryVertexDeletionOn();
        decimate->SetSplitAngle(120);
        decimate->Update();
        poly_data = decimate->GetOutput();
        io::vtk::helper::mesh::from_vtk_mesh(poly_data, mesh);
    }
    else
    {
        poly_data = smooth_filter->GetOutput();
        io::vtk::helper::mesh::from_vtk_mesh(poly_data, mesh);
    }

    auto reconstruction = std::make_shared<data::reconstruction>();

    static unsigned int organ_number = 0;
    ++organ_number;
    reconstruction->set_organ_name("OrganMesher_VTK_" + std::to_string(organ_number));
    reconstruction->set_structure_type("organ_t");
    reconstruction->set_is_visible(true);
    // Set Mesh
    reconstruction->set_mesh(mesh);

    data::model_series::reconstruction_vector_t recs = model_series->get_reconstruction_db();
    recs.push_back(reconstruction);
    model_series->set_reconstruction_db(recs);
    model_series->set_dicom_reference(image_series->get_dicom_reference());

    m_model = model_series;
}

//-------------------------------------------------------------------------

vtk_mesher::connections_t vtk_mesher::auto_connections() const
{
    return {
        {m_threshold, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_reduction, data::object::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::mesh
