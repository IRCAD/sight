/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/vtk/SeriesDBReader.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/memory/BufferObject.hpp>
#include <core/memory/stream/in/IFactory.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Reconstruction.hpp>
#include <data/Study.hpp>

#include <io/base/reader/registry/macros.hpp>

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

SIGHT_REGISTER_IO_READER(sight::io::vtk::SeriesDBReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void initSeries(data::Series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::getDate(now);
    const std::string time       = core::tools::getTime(now);
    series->setDate(date);
    series->setTime(time);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
}

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(io::base::reader::IObjectReader::Key) :
    m_job(core::jobs::Observer::New("SeriesDB reader")),
    m_lazyMode(true)
{
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
}

//------------------------------------------------------------------------------
template<typename T, typename FILE>
vtkSmartPointer<vtkDataObject> getObj(FILE& file, const core::jobs::Observer::sptr& job)
{
    using namespace sight::io::vtk::helper;

    vtkSmartPointer<T> reader = vtkSmartPointer<T>::New();
    reader->SetFileName(file.string().c_str());

    if(job)
    {
        vtkSmartPointer<vtkLambdaCommand> progressCallback;
        progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
        progressCallback->SetCallback(
            [&](vtkObject* caller, long unsigned int, void*)
            {
                auto filter = static_cast<T*>(caller);
                job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
            });
        reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

        job->addSimpleCancelHook(
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

    if(grid)
    {
        data::Mesh::sptr meshObj = data::Mesh::New();
        io::vtk::helper::Mesh::fromVTKGrid(grid, meshObj);

        data::Reconstruction::sptr rec = data::Reconstruction::New();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }

    if(mesh)
    {
        data::Mesh::sptr meshObj = data::Mesh::New();
        io::vtk::helper::Mesh::fromVTKMesh(mesh, meshObj);
        data::Reconstruction::sptr rec = data::Reconstruction::New();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    else if(img)
    {
        try
        {
            data::Image::sptr imgObj = data::Image::New();
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

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkMetaImageReader>&)
{
    return true;
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkGenericDataObjectReader>& reader)
{
    return reader->IsFileStructuredPoints();
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer<vtkXMLGenericDataObjectReader>& reader)
{
    return reader->GetImageDataOutput() != 0;
}

//------------------------------------------------------------------------------

void SeriesDBReader::read()
{
    data::SeriesDB::sptr seriesDB = this->getConcreteObject();

    const std::vector<std::filesystem::path>& files = this->getFiles();
    const std::string instanceUID                   = core::tools::UUID::generateUUID();

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
            img = data::Image::dynamicCast(dataObj);
            rec = data::Reconstruction::dynamicCast(dataObj);
        }

        if(img)
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::New();
            initSeries(imgSeries, instanceUID);
            imgSeries->setImage(img);
            seriesDB->getContainer().push_back(imgSeries);
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
        SIGHT_THROW("SeriesDBReader cannot read VTK file(s) : " << boost::algorithm::join(errorFiles, ", "));
    }

    // Adds loaded Reconstructions in SeriesDB
    if(!recs.empty())
    {
        data::ModelSeries::sptr modelSeries = data::ModelSeries::New();
        initSeries(modelSeries, instanceUID);
        modelSeries->setReconstructionDB(recs);
        seriesDB->getContainer().push_back(modelSeries);
    }
}

//------------------------------------------------------------------------------

std::string SeriesDBReader::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr SeriesDBReader::getJob() const
{
    return m_job;
}

} // namespace sight::io::vtk
