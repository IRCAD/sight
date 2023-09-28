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

#include "io/vtk/ModelSeriesObjWriter.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/tools/uuid.hpp>

#include <data/Material.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <vtkActor.h>
#include <vtkOBJExporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::ModelSeriesObjWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ModelSeriesObjWriter::ModelSeriesObjWriter() :
    m_job(std::make_shared<core::jobs::observer>("ModelSeries Writer"))
{
}

//------------------------------------------------------------------------------

ModelSeriesObjWriter::~ModelSeriesObjWriter()
= default;

//------------------------------------------------------------------------------

vtkSmartPointer<vtkActor> createActor(const data::Reconstruction::sptr& pReconstruction)
{
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    data::Mesh::sptr mesh         = pReconstruction->getMesh();
    data::Material::sptr material = pReconstruction->getMaterial();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh, polyData);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    data::Color::sptr diffuse = material->diffuse();
    property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());
    property->SetOpacity(diffuse->alpha());

    data::Color::sptr ambient = material->ambient();
    property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

    property->SetSpecularColor(1., 1., 1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    return actor;
}

//------------------------------------------------------------------------------

void ModelSeriesObjWriter::write()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const std::filesystem::path prefix = this->get_folder();

    const data::ModelSeries::csptr modelSeries = getConcreteObject();

    m_job->set_total_work_units(modelSeries->getReconstructionDB().size());
    std::uint64_t units = 0;
    for(const data::Reconstruction::sptr& rec : modelSeries->getReconstructionDB())
    {
        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkActor> actor       = createActor(rec);
        renderer->AddActor(actor);

        vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);

        const std::string filename = (prefix / (rec->getOrganName() + "_" + rec->get_uuid())).string();

        vtkSmartPointer<vtkOBJExporter> exporter = vtkSmartPointer<vtkOBJExporter>::New();
        exporter->SetRenderWindow(renderWindow);
        exporter->SetFilePrefix(filename.c_str());
        exporter->Write();
        m_job->done_work(++units);

        // can not observe progression, not a vtkAlgorithm ...
    }

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ModelSeriesObjWriter::extension() const
{
    return ".obj";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr ModelSeriesObjWriter::getJob() const
{
    return m_job;
}

} // namespace sight::io::vtk
