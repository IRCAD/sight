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

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/memory/stream/in/factory.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

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

void initSeries(data::Series::sptr series, const std::string& instanceUID)
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
    m_job(std::make_shared<core::jobs::observer>("SeriesSet reader")),
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

data::Object::sptr getDataObject(const vtkSmartPointer<vtkDataObject>& obj, const std::filesystem::path& file)
{
    vtkSmartPointer<vtkPolyData> mesh         = vtkPolyData::SafeDownCast(obj);
    vtkSmartPointer<vtkImageData> img         = vtkImageData::SafeDownCast(obj);
    vtkSmartPointer<vtkUnstructuredGrid> grid = vtkUnstructuredGrid::SafeDownCast(obj);
    data::Object::sptr dataObj;

    if(grid != nullptr)
    {
        data::Mesh::sptr meshObj = std::make_shared<data::Mesh>();
        io::vtk::helper::Mesh::fromVTKGrid(grid, meshObj);

        data::Reconstruction::sptr rec = std::make_shared<data::Reconstruction>();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }

    if(mesh != nullptr)
    {
        data::Mesh::sptr meshObj = std::make_shared<data::Mesh>();
        io::vtk::helper::Mesh::fromVTKMesh(mesh, meshObj);
        data::Reconstruction::sptr rec = std::make_shared<data::Reconstruction>();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    else if(img != nullptr)
    {
        try
        {
            data::Image::sptr imgObj = std::make_shared<data::Image>();
            io::vtk::fromVTKImage(img, imgObj);
            dataObj = imgObj;
        }
        catch(std::exception& e)
        {
            SIGHT_THROW("VTKImage to data::Image failed " << e.what());
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

    data::ModelSeries::ReconstructionVectorType recs;
    std::vector<std::string> errorFiles;
    for(const auto& file : files)
    {
        vtkSmartPointer<vtkDataObject> obj;
        data::Image::sptr img;
        data::Reconstruction::sptr rec;

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
            data::Object::sptr dataObj = getDataObject(obj, file);
            img = std::dynamic_pointer_cast<data::Image>(dataObj);
            rec = std::dynamic_pointer_cast<data::Reconstruction>(dataObj);
        }

        if(img)
        {
            auto imgSeries = std::make_shared<data::ImageSeries>();
            initSeries(imgSeries, instanceUID);
            imgSeries->Image::shallow_copy(img);
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

    // Adds loaded Reconstructions in SeriesSet
    if(!recs.empty())
    {
        data::ModelSeries::sptr modelSeries = std::make_shared<data::ModelSeries>();
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
