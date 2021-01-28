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

#include "fwVtkIO/SeriesDBReader.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

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

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>

#include <io/base/reader/registry/macros.hpp>

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

fwDataIOReaderRegisterMacro( ::fwVtkIO::SeriesDBReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

void initSeries(data::Series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    const std::string date = core::tools::getDate(now);
    const std::string time = core::tools::getTime(now);
    series->setDate(date);
    series->setTime(time);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
}

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(io::base::reader::IObjectReader::Key) :
    data::location::enableMultiFiles< io::base::reader::IObjectReader >(this),
    m_job(core::jobs::Observer::New("SeriesDB reader")),
    m_lazyMode(true)
{
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
}

//------------------------------------------------------------------------------
template <typename T, typename FILE>
vtkSmartPointer< vtkDataObject  > getObj(FILE& file, const core::jobs::Observer::sptr& job)
{
    using namespace fwVtkIO::helper;

    vtkSmartPointer< T > reader = vtkSmartPointer< T >::New();
    reader->SetFileName(file.string().c_str());

    if(job)
    {
        vtkSmartPointer<vtkLambdaCommand> progressCallback;
        progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
        progressCallback->SetCallback([&](vtkObject* caller, long unsigned int, void* )
            {
                auto filter = static_cast<T*>(caller);
                job->doneWork( static_cast<std::uint64_t>(filter->GetProgress()*100.) );
            });
        reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

        job->addSimpleCancelHook( [&]()
            {
                reader->AbortExecuteOn();
            } );
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

data::Object::sptr getDataObject(const vtkSmartPointer< vtkDataObject  >& obj, const std::filesystem::path& file)
{
    vtkSmartPointer< vtkPolyData > mesh         = vtkPolyData::SafeDownCast(obj);
    vtkSmartPointer< vtkImageData > img         = vtkImageData::SafeDownCast(obj);
    vtkSmartPointer< vtkUnstructuredGrid > grid = vtkUnstructuredGrid::SafeDownCast(obj);
    data::Object::sptr dataObj;

    if(grid)
    {
        data::Mesh::sptr meshObj = data::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKGrid(grid, meshObj);

        data::Reconstruction::sptr rec = data::Reconstruction::New();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    if(mesh)
    {
        data::Mesh::sptr meshObj = data::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKMesh(mesh, meshObj);
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
            ::fwVtkIO::fromVTKImage( img, imgObj);
            dataObj = imgObj;
        }
        catch( std::exception& e)
        {
            FW_RAISE("VTKImage to data::Image failed "<<e.what());
        }
    }
    return dataObj;
}
//------------------------------------------------------------------------------

struct FilteringStream : ::boost::iostreams::filtering_istream
{

    typedef ::boost::iostreams::stream< ::boost::iostreams::array_source > BufferStreamType;

    FilteringStream(const data::Image::sptr& source ) :
        m_image(source),
        m_bufferObject(source->getBufferObject()),
        m_lock( m_bufferObject->lock() ),
        m_bufferStream( std::make_shared<BufferStreamType>(static_cast<char*>(m_lock.getBuffer()),
                                                           m_bufferObject->getSize()) )
    {
        this->push(*m_bufferStream);
    }

    ~FilteringStream()
    {
        try
        {
            this->reset();
        }
        catch (...)
        {
        }
    }

    data::Image::sptr m_image;
    core::memory::BufferObject::sptr m_bufferObject;
    core::memory::BufferObject::Lock m_lock;
    SPTR(BufferStreamType) m_bufferStream;
};

//------------------------------------------------------------------------------

template< typename READER >
class ImageStream : public core::memory::stream::in::IFactory
{
public:

    ImageStream( const std::filesystem::path& path ) :
        m_path(path)
    {
    }

protected:

    //------------------------------------------------------------------------------

    data::Image::sptr getImage()
    {
        if(!std::filesystem::exists(m_path))
        {
            FW_RAISE("file "<< m_path.string() << " does not exist anymore or has moved.");
        }

        vtkSmartPointer< vtkDataObject  > obj;
        obj = getObj<READER>(m_path, nullptr);

        return data::Image::dynamicCast(getDataObject(obj, m_path));
    }

    SPTR(std::istream) get()
    {
        SPTR(FilteringStream) is
            = std::make_shared< FilteringStream>( this->getImage() );

        return is;
    }

    std::filesystem::path m_path;
};

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkMetaImageReader >&)
{
    return true;
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkGenericDataObjectReader >& reader)
{
    return reader->IsFileStructuredPoints();
}

//------------------------------------------------------------------------------

bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkXMLGenericDataObjectReader >& reader)
{
    return (reader->GetImageDataOutput() != 0);
}

//------------------------------------------------------------------------------

void updateImageFromVtkInfo(const vtkSmartPointer< vtkInformation >& info, const data::Image::sptr& imgObj)
{
    int extent[6];
    info->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
    const data::Image::Size size = { static_cast<size_t>(extent[1]-extent[0]+1),
                                     static_cast<size_t>(extent[3]-extent[2]+1),
                                     static_cast<size_t>(extent[5]-extent[4]+1)};
    imgObj->setSize2(size);

    data::Image::Spacing spacing;
    info->Get(vtkDataObject::SPACING(), &spacing[0]);
    imgObj->setSpacing2(spacing);

    data::Image::Origin origin;
    info->Get(vtkDataObject::ORIGIN(), &origin[0]);
    imgObj->setOrigin2(origin);

    vtkInformation* attrInfo = vtkDataObject::GetActiveFieldInformation(info, vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                                                        vtkDataSetAttributes::SCALARS);
    int nbOfComponents = attrInfo->Get(vtkDataObject::FIELD_NUMBER_OF_COMPONENTS());
    imgObj->setType( ::fwVtkIO::TypeTranslator::translate( attrInfo->Get(vtkDataObject::FIELD_ARRAY_TYPE()) ) );
    imgObj->setNumberOfComponents(static_cast<size_t>(nbOfComponents));
}

//------------------------------------------------------------------------------

void getInfo(const vtkSmartPointer< vtkGenericDataObjectReader >& reader, const data::Image::sptr& imgObj)
{
    vtkSmartPointer< vtkStructuredPointsReader > imgReader = vtkSmartPointer< vtkStructuredPointsReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->ReadMetaData(info);

    updateImageFromVtkInfo(info, imgObj);

    std::filesystem::path file = reader->GetFileName();
    imgObj->setIStreamFactory( std::make_shared< ImageStream<vtkStructuredPointsReader> >(file),
                               imgObj->getSizeInBytes());
}

//------------------------------------------------------------------------------

void getInfo(const vtkSmartPointer< vtkXMLGenericDataObjectReader >& reader, const data::Image::sptr& imgObj)
{
    vtkSmartPointer< vtkXMLImageDataReader > imgReader = vtkSmartPointer< vtkXMLImageDataReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->UpdateInformation();
    imgReader->CopyOutputInformation(info, 0);

    updateImageFromVtkInfo(info, imgObj);

    std::filesystem::path file = reader->GetFileName();
    imgObj->setIStreamFactory( std::make_shared< ImageStream<vtkXMLImageDataReader> >(file),
                               imgObj->getSizeInBytes());

}

//------------------------------------------------------------------------------

template< typename DATA_READER >
data::Image::sptr lazyRead( const std::filesystem::path& file, const core::jobs::Observer::sptr& job)
{
    vtkSmartPointer< DATA_READER > reader = vtkSmartPointer< DATA_READER >::New();
    reader->SetFileName(file.string().c_str());
    reader->UpdateInformation();

    data::Image::sptr imgObj;

    if(checkIfReadDataTypeIsImage(reader))
    {
        imgObj = data::Image::New();
        getInfo(reader, imgObj);

        job->finish();
    }

    return imgObj;
}

//------------------------------------------------------------------------------

void SeriesDBReader::read()
{
    data::SeriesDB::sptr seriesDB = this->getConcreteObject();

    const data::location::ILocation::VectPathType files = this->getFiles();
    const std::string instanceUID                       = core::tools::UUID::generateUUID();

    data::ModelSeries::ReconstructionVectorType recs;
    std::vector< std::string > errorFiles;
    for(const data::location::ILocation::VectPathType::value_type& file :  files)
    {
        vtkSmartPointer< vtkDataObject  > obj;
        data::Image::sptr img;
        data::Reconstruction::sptr rec;

        if(file.extension().string() == ".vtk")
        {
            if(m_lazyMode)
            {
                img = lazyRead<vtkGenericDataObjectReader>(file, m_job);
            }
            if (!img)
            {
                obj = getObj<vtkGenericDataObjectReader>(file, m_job);
            }
        }
        else if(file.extension().string() == ".vti")
        {
            if(m_lazyMode)
            {
                img = lazyRead<vtkXMLGenericDataObjectReader>(file, m_job);
            }
            if (!img)
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

        if (!img)
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
        else if (rec)
        {
            recs.push_back(rec);
        }
        else
        {
            errorFiles.push_back(file.string());
        }
    }

    if (!errorFiles.empty())
    {
        FW_RAISE("SeriesDBReader cannot read VTK file(s) : "<< ::boost::algorithm::join(errorFiles, ", ") );
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

std::string SeriesDBReader::extension()
{
    return ".vtk";
}

//------------------------------------------------------------------------------

::core::jobs::IJob::sptr SeriesDBReader::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO
