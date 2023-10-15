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

#include "io/vtk/SeriesSetReader.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
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

#include <io/__/reader/registry/macros.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>

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

SIGHT_REGISTER_IO_READER(sight::io::vtk::SeriesSetReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void initSeries(data::series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::get_date(now);
    const std::string time       = core::tools::get_time(now);
    series->setSeriesDate(date);
    series->setSeriesTime(time);

    series->setStudyInstanceUID(instanceUID);
    series->setStudyDate(date);
    series->setStudyTime(time);
}

//------------------------------------------------------------------------------

SeriesSetReader::SeriesSetReader() :
    m_job(std::make_shared<core::jobs::observer>("series_set reader")),
    m_lazyMode(true)
{
}

//------------------------------------------------------------------------------
template<typename T, typename FILE>
vtkSmartPointer<vtkDataObject> getObj(FILE& file, const core::jobs::observer::sptr& job)
{
    vtkSmartPointer<T> reader = vtkSmartPointer<T>::New();
    reader->SetFileName(file.string().c_str());

    if(job)
    {
        vtkSmartPointer<helper::vtkLambdaCommand> progress_callback;
        progress_callback = vtkSmartPointer<helper::vtkLambdaCommand>::New();
        progress_callback->SetCallback(
            [&](vtkObject* caller, std::uint64_t, void*)
            {
                auto* filter = static_cast<T*>(caller);
                job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
            });
        reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

        job->add_simple_cancel_hook(
            [&]()
            {
                reader->AbortExecuteOn();
            });
        reader->Update();
        job->finish();
    }
    else
    {
        reader->Update();
    }

    return reader->GetOutput();
}

//------------------------------------------------------------------------------

data::object::sptr getDataObject(const vtkSmartPointer<vtkDataObject>& obj, const std::filesystem::path& file)
{
    vtkSmartPointer<vtkPolyData> mesh         = vtkPolyData::SafeDownCast(obj);
    vtkSmartPointer<vtkImageData> img         = vtkImageData::SafeDownCast(obj);
    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkUnstructuredGrid::SafeDownCast(obj);
    data::object::sptr dataObj;

    if(grid != nullptr)
    {
        data::mesh::sptr meshObj = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::fromVTKGrid(grid, meshObj);

        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }

    if(mesh != nullptr)
    {
        data::mesh::sptr meshObj = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::fromVTKMesh(mesh, meshObj);
        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    else if(img != nullptr)
    {
        try
        {
            data::image::sptr imgObj = std::make_shared<data::image>();
            io::vtk::fromVTKImage(img, imgObj);
            dataObj = imgObj;
        }
        catch(std::exception& e)
        {
            SIGHT_THROW("VTKImage to data::image failed " << e.what());
        }
    }

    return dataObj;
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkMetaImageReader>& /*unused*/)
{
    return true;
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkGenericDataObjectReader>& reader)
{
    return reader->IsFileStructuredPoints() != 0;
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkXMLGenericDataObjectReader>& reader)
{
    return reader->GetImageDataOutput() != nullptr;
}

//------------------------------------------------------------------------------

void SeriesSetReader::read()
{
    auto series_set = getConcreteObject();

    const std::vector<std::filesystem::path>& files = get_files();
    const std::string instanceUID                   = core::tools::UUID::generate();

    data::model_series::ReconstructionVectorType recs;
    std::vector<std::string> errorFiles;
    for(const auto& file : files)
    {
        vtkSmartPointer<vtkDataObject> obj;
        data::image::sptr img;
        data::reconstruction::sptr rec;

        if(file.extension().string() == ".vtk")
        {
            if(!img)
            {
                obj = getObj<vtkGenericDataObjectReader>(file, m_job);
            }
        }
        else if(file.extension().string() == ".vti")
        {
            if(!img)
            {
                obj = getObj<vtkXMLGenericDataObjectReader>(file, m_job);
            }
        }
        else if(file.extension().string() == ".mhd")
        {
            obj = getObj<vtkMetaImageReader>(file, m_job);
        }
        else if(file.extension().string() == ".vtu" || file.extension().string() == ".vtp")
        {
            obj = getObj<vtkXMLGenericDataObjectReader>(file, m_job);
        }
        else if(file.extension().string() == ".obj")
        {
            obj = getObj<vtkOBJReader>(file, m_job);
        }
        else if(file.extension().string() == ".stl")
        {
            obj = getObj<vtkSTLReader>(file, m_job);
        }
        else if(file.extension().string() == ".ply")
        {
            obj = getObj<vtkPLYReader>(file, m_job);
        }

        if(!img)
        {
            data::object::sptr dataObj = getDataObject(obj, file);
            img = std::dynamic_pointer_cast<data::image>(dataObj);
            rec = std::dynamic_pointer_cast<data::reconstruction>(dataObj);
        }

        if(img)
        {
            auto imgSeries = std::make_shared<data::image_series>();
            initSeries(imgSeries, instanceUID);
            imgSeries->image::shallow_copy(img);
            series_set->push_back(imgSeries);
        }
        else if(rec)
        {
            recs.push_back(rec);
        }
        else
        {
            errorFiles.push_back(file.string());
        }
    }

    if(!errorFiles.empty())
    {
        SIGHT_THROW("SeriesSetReader cannot read VTK file(s) : " << boost::algorithm::join(errorFiles, ", "));
    }

    // Adds loaded Reconstructions in series_set
    if(!recs.empty())
    {
        data::model_series::sptr modelSeries = std::make_shared<data::model_series>();
        initSeries(modelSeries, instanceUID);
        modelSeries->setReconstructionDB(recs);
        series_set->push_back(modelSeries);
    }
}

//------------------------------------------------------------------------------

std::string SeriesSetReader::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr SeriesSetReader::getJob() const
{
    return m_job;
}

} // namespace sight::io::vtk
