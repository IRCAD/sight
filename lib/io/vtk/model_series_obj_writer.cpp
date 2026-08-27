/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "io/vtk/model_series_obj_writer.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/vtk.hpp"

#include <core/notification/observer.hpp>

#include <data/material.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <vtkActor.h>
#include <vtkOBJExporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <filesystem>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

static vtkSmartPointer<vtkActor> create_actor(const data::reconstruction::sptr& _p_reconstruction)
{
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    data::mesh::sptr mesh         = _p_reconstruction->get_mesh();
    data::material::sptr material = _p_reconstruction->get_material();

    vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh, poly_data);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly_data);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    data::color::sptr diffuse = material->diffuse();
    property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());
    property->SetOpacity(diffuse->alpha());

    data::color::sptr ambient = material->ambient();
    property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

    property->SetSpecularColor(1., 1., 1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    return actor;
}

//------------------------------------------------------------------------------

void model_series_obj_writer::write(sight::core::notification::observer::sptr _progress)
{
    auto object_lock = get_object();

    const std::filesystem::path prefix = this->get_folder();

    const data::model_series::csptr model_series = get_concrete_object();

    _progress->set_total_work_units(model_series->get_reconstruction_db().size());

    std::uint64_t units = 0;
    for(const data::reconstruction::sptr& rec : model_series->get_reconstruction_db())
    {
        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkActor> actor       = create_actor(rec);
        renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> render_window = vtkSmartPointer<vtkRenderWindow>::New();
        render_window->AddRenderer(renderer);

        const std::string filename = (prefix / (rec->get_organ_name() + "_" + rec->get_uuid())).string();

        vtkSmartPointer<vtkOBJExporter> exporter = vtkSmartPointer<vtkOBJExporter>::New();
        exporter->SetRenderWindow(render_window);
        exporter->SetFilePrefix(filename.c_str());
        exporter->Write();
        _progress->done_work(++units);

        // can not observe progression, not a vtkAlgorithm ...
    }
}

//------------------------------------------------------------------------------

std::string model_series_obj_writer::extension() const
{
    return ".obj";
}

} // namespace sight::io::vtk
