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

#include "io/vtk/series_set_reader.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/observer.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/memory/stream/in/factory.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <boost/algorithm/string/join.hpp>

#include <vtkDataSetAttributes.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkMetaImageReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkXMLGenericDataObjectReader.h>
#include <vtkXMLImageDataReader.h>

#include <algorithm>
#include <filesystem>
#include <iosfwd>
#include <numeric>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void init_series(data::series::sptr _series, const std::string& _instance_uid)
{
    _series->set_modality(data::dicom::modality_t::ot);
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::get_date(now);
    const std::string time       = core::tools::get_time(now);
    _series->set_series_date(date);
    _series->set_series_time(time);

    _series->set_study_instance_uid(_instance_uid);
    _series->set_study_date(date);
    _series->set_study_time(time);
}

//------------------------------------------------------------------------------

series_set_reader::series_set_reader() :
    m_job(std::make_shared<core::jobs::aggregator>("series_set reader")),
    m_lazy_mode(true)
{
}

//------------------------------------------------------------------------------
template<typename T, typename FILE>
vtkSmartPointer<vtkDataObject> get_obj(FILE& _file, const core::jobs::observer::sptr& _job)
{
    vtkSmartPointer<T> reader = vtkSmartPointer<T>::New();
    reader->SetFileName(_file.string().c_str());

    if(_job)
    {
        vtkSmartPointer<helper::vtk_lambda_command> progress_callback;
        progress_callback = vtkSmartPointer<helper::vtk_lambda_command>::New();
        progress_callback->set_callback(
            [&](vtkObject* _caller, std::uint64_t, void*)
            {
                auto* filter = static_cast<T*>(_caller);
                _job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
            });
        reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

        _job->add_simple_cancel_hook(
            [&]()
            {
                reader->AbortExecuteOn();
            });
        reader->Update();
        _job->finish();
    }
    else
    {
        reader->Update();
    }

    return reader->GetOutput();
}

//------------------------------------------------------------------------------

data::object::sptr get_data_object(const vtkSmartPointer<vtkDataObject>& _obj, const std::filesystem::path& _file)
{
    vtkSmartPointer<vtkPolyData> mesh         = vtkPolyData::SafeDownCast(_obj);
    vtkSmartPointer<vtkImageData> img         = vtkImageData::SafeDownCast(_obj);
    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkUnstructuredGrid::SafeDownCast(_obj);
    data::object::sptr data_obj;

    if(grid != nullptr)
    {
        data::mesh::sptr mesh_obj = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::from_vtk_grid(grid, mesh_obj);

        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        rec->set_mesh(mesh_obj);
        rec->set_organ_name(_file.stem().string());
        rec->set_is_visible(true);
        data_obj = rec;
    }

    if(mesh != nullptr)
    {
        data::mesh::sptr mesh_obj = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::from_vtk_mesh(mesh, mesh_obj);
        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        rec->set_mesh(mesh_obj);
        rec->set_organ_name(_file.stem().string());
        rec->set_is_visible(true);
        data_obj = rec;
    }
    else if(img != nullptr)
    {
        try
        {
            data::image::sptr img_obj = std::make_shared<data::image>();
            io::vtk::from_vtk_image(img, img_obj);
            data_obj = img_obj;
        }
        catch(std::exception& e)
        {
            SIGHT_THROW("VTKImage to data::image failed " << e.what());
        }
    }

    return data_obj;
}

//------------------------------------------------------------------------------

bool check_if_read_data_type_is_image(const vtkSmartPointer<vtkMetaImageReader>& /*unused*/)
{
    return true;
}

//------------------------------------------------------------------------------

bool check_if_read_data_type_is_image(const vtkSmartPointer<vtkGenericDataObjectReader>& _reader)
{
    return _reader->IsFileStructuredPoints() != 0;
}

//------------------------------------------------------------------------------

bool check_if_read_data_type_is_image(const vtkSmartPointer<vtkXMLGenericDataObjectReader>& _reader)
{
    return _reader->GetImageDataOutput() != nullptr;
}

//------------------------------------------------------------------------------

void series_set_reader::read()
{
    auto series_set = get_concrete_object();

    const std::vector<std::filesystem::path>& files = get_files();
    const std::string instance_uid                  = core::tools::uuid::generate();

    data::model_series::reconstruction_vector_t recs;
    std::vector<std::string> error_files;
    for(const auto& file : files)
    {
        const auto job_observer = std::make_shared<core::jobs::observer>(file.string());
        m_job->add(job_observer);

        vtkSmartPointer<vtkDataObject> obj;
        data::image::sptr img;
        data::reconstruction::sptr rec;

        if(file.extension().string() == ".vtk")
        {
            if(!img)
            {
                obj = get_obj<vtkGenericDataObjectReader>(file, job_observer);
            }
        }
        else if(file.extension().string() == ".vti")
        {
            if(!img)
            {
                obj = get_obj<vtkXMLGenericDataObjectReader>(file, job_observer);
            }
        }
        else if(file.extension().string() == ".mhd")
        {
            obj = get_obj<vtkMetaImageReader>(file, job_observer);
        }
        else if(file.extension().string() == ".vtu" || file.extension().string() == ".vtp")
        {
            obj = get_obj<vtkXMLGenericDataObjectReader>(file, job_observer);
        }
        else if(file.extension().string() == ".obj")
        {
            obj = get_obj<vtkOBJReader>(file, job_observer);
        }
        else if(file.extension().string() == ".stl")
        {
            obj = get_obj<vtkSTLReader>(file, job_observer);
        }
        else if(file.extension().string() == ".ply")
        {
            obj = get_obj<vtkPLYReader>(file, job_observer);
        }

        if(!img)
        {
            data::object::sptr data_obj = get_data_object(obj, file);
            img = std::dynamic_pointer_cast<data::image>(data_obj);
            rec = std::dynamic_pointer_cast<data::reconstruction>(data_obj);
        }

        if(img)
        {
            auto img_series = std::make_shared<data::image_series>();
            init_series(img_series, instance_uid);
            img_series->image::shallow_copy(img);
            series_set->push_back(img_series);
        }
        else if(rec)
        {
            recs.push_back(rec);
        }
        else
        {
            error_files.push_back(file.string());
        }
    }

    if(!error_files.empty())
    {
        SIGHT_THROW("SeriesSetReader cannot read VTK file(s) : " << boost::algorithm::join(error_files, ", "));
    }

    // Adds loaded Reconstructions in series_set
    if(!recs.empty())
    {
        data::model_series::sptr model_series = std::make_shared<data::model_series>();
        init_series(model_series, instance_uid);
        model_series->set_reconstruction_db(recs);
        series_set->push_back(model_series);
    }
}

//------------------------------------------------------------------------------

std::string series_set_reader::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr series_set_reader::get_job() const
{
    return m_job;
}

} // namespace sight::io::vtk
