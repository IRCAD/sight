/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/SeriesDBReader.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwCore/base.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwMemory/BufferObject.hpp>
#include <fwMemory/stream/in/IFactory.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>

#include <vtkDataSetAttributes.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkMetaImageReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkXMLGenericDataObjectReader.h>
#include <vtkXMLImageDataReader.h>

#include <algorithm>
#include <iosfwd>
#include <numeric>

fwDataIOReaderRegisterMacro( ::fwVtkIO::SeriesDBReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

void  initSeries(::fwMedData::Series::sptr series, const std::string& instanceUID)
{
    const std::string unknown = "unknown";
    series->setModality("OT");
    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    const std::string date = ::fwTools::getDate(now);
    const std::string time = ::fwTools::getTime(now);
    series->setDate(date);
    series->setTime(time);
    //series->setDescription(??);
    //series->setPerformingPhysiciansName(??);

    series->getEquipment()->setInstitutionName(unknown);

    series->getPatient()->setName(unknown);
    series->getPatient()->setPatientId(unknown);
    series->getPatient()->setBirthdate(unknown);
    series->getPatient()->setSex(unknown);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
    series->getStudy()->setReferringPhysicianName(unknown);
    series->getStudy()->setDescription(unknown);
    series->getStudy()->setPatientAge(unknown);
}

//------------------------------------------------------------------------------


SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >(this),
    m_lazyMode(true),
    m_job(::fwJobs::Observer::New("SeriesDB reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
template <typename T, typename FILE>
vtkSmartPointer< vtkDataObject  > getObj(FILE &file, const ::fwJobs::Observer::sptr& job)
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
                job->doneWork( filter->GetProgress()*100 );
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

::fwData::Object::sptr getDataObject(const vtkSmartPointer< vtkDataObject  > &obj, const boost::filesystem::path &file)
{
    vtkSmartPointer< vtkPolyData > mesh         = vtkPolyData::SafeDownCast(obj);
    vtkSmartPointer< vtkImageData > img         = vtkImageData::SafeDownCast(obj);
    vtkSmartPointer< vtkUnstructuredGrid > grid = vtkUnstructuredGrid::SafeDownCast(obj);
    ::fwData::Object::sptr dataObj;

    if(grid)
    {
        ::fwData::Mesh::sptr meshObj = ::fwData::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKGrid(grid, meshObj);

        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    if(mesh)
    {
        ::fwData::Mesh::sptr meshObj = ::fwData::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKMesh(mesh, meshObj);
        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();
        rec->setMesh(meshObj);
        rec->setOrganName(file.stem().string());
        rec->setIsVisible(true);
        dataObj = rec;
    }
    else if(img)
    {
        try
        {
            ::fwData::Image::sptr imgObj = ::fwData::Image::New();
            ::fwVtkIO::fromVTKImage( img, imgObj);
            dataObj = imgObj;
        }
        catch( std::exception &e)
        {
            FW_RAISE("VTKImage to fwData::Image failed "<<e.what());
        }
    }
    return dataObj;
}
//------------------------------------------------------------------------------

struct FilteringStream : ::boost::iostreams::filtering_istream
{

    typedef ::boost::iostreams::stream< ::boost::iostreams::array_source > BufferStreamType;


    FilteringStream(const ::fwData::Image::sptr &source ) :
        m_image(source),
        m_bufferObject(source->getDataArray()->getBufferObject()),
        m_lock( m_bufferObject->lock() ),
        m_bufferStream( std::make_shared<BufferStreamType>(static_cast<char *>(m_lock.getBuffer()),
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

    ::fwData::Image::sptr m_image;
    ::fwMemory::BufferObject::sptr m_bufferObject;
    ::fwMemory::BufferObject::Lock m_lock;
    SPTR(BufferStreamType) m_bufferStream;
};

//------------------------------------------------------------------------------

template< typename READER >
class ImageStream : public ::fwMemory::stream::in::IFactory
{
public:

    ImageStream( const ::boost::filesystem::path& path ) :  m_path(path)
    {
    }

protected:

    ::fwData::Image::sptr getImage()
    {
        if(!::boost::filesystem::exists(m_path))
        {
            FW_RAISE("file "<< m_path.string() << " does not exist anymore or has moved.");
        }

        vtkSmartPointer< vtkDataObject  > obj;
        obj = getObj<READER>(m_path, nullptr);

        return ::fwData::Image::dynamicCast(getDataObject(obj, m_path));
    }

    SPTR(std::istream) get()
    {
        SPTR(FilteringStream) is
            = std::make_shared< FilteringStream>( this->getImage() );

        return is;
    }

    ::boost::filesystem::path m_path;
};

//------------------------------------------------------------------------------


bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkMetaImageReader > &reader)
{
    return true;
}

bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkGenericDataObjectReader > &reader)
{
    return reader->IsFileStructuredPoints();
}

bool checkIfReadDataTypeIsImage(const vtkSmartPointer< vtkXMLGenericDataObjectReader > &reader)
{
    return (reader->GetImageDataOutput () != 0);
}

void updateImageFromVtkInfo(const vtkSmartPointer< vtkInformation > &info, const ::fwData::Image::sptr &imgObj)
{
    int extent[6];
    info->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
    ::fwData::Image::SizeType size(3,0);
    size[0] = extent[1]-extent[0]+1;
    size[1] = extent[3]-extent[2]+1;
    size[2] = extent[5]-extent[4]+1;
    imgObj->setSize(size);

    ::fwData::Image::SpacingType spacing(3, 0);
    info->Get(vtkDataObject::SPACING(), &spacing[0]);
    imgObj->setSpacing(spacing);

    ::fwData::Image::OriginType origin(3, 0);
    info->Get(vtkDataObject::ORIGIN(), &origin[0]);
    imgObj->setOrigin(origin);

    vtkInformation *attrInfo = vtkDataObject::GetActiveFieldInformation(info, vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                                                        vtkDataSetAttributes::SCALARS);
    int type           = attrInfo->Get(vtkDataObject::FIELD_ARRAY_TYPE());
    int nbOfComponents = attrInfo->Get(vtkDataObject::FIELD_NUMBER_OF_COMPONENTS());
    imgObj->setType( ::fwVtkIO::TypeTranslator::translate( attrInfo->Get(vtkDataObject::FIELD_ARRAY_TYPE()) ) );
    imgObj->setNumberOfComponents(nbOfComponents);
    imgObj->getDataArray()->setType(imgObj->getType());
}

void getInfo(const vtkSmartPointer< vtkGenericDataObjectReader > &reader, const ::fwData::Image::sptr &imgObj)
{
    vtkSmartPointer< vtkStructuredPointsReader > imgReader = vtkSmartPointer< vtkStructuredPointsReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->ReadMetaData(info);

    updateImageFromVtkInfo(info, imgObj);
    imgObj->getDataArray()->resize(imgObj->getSize(), false);

    ::fwMemory::BufferObject::sptr buffObj = imgObj->getDataArray()->getBufferObject();
    boost::filesystem::path file = reader->GetFileName();
    buffObj->setIStreamFactory( std::make_shared< ImageStream<vtkStructuredPointsReader> >(file),
                                imgObj->getSizeInBytes());
}


void getInfo(const vtkSmartPointer< vtkXMLGenericDataObjectReader > &reader, const ::fwData::Image::sptr &imgObj)
{
    vtkSmartPointer< vtkXMLImageDataReader > imgReader = vtkSmartPointer< vtkXMLImageDataReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->UpdateInformation();
    imgReader->CopyOutputInformation(info,0);

    updateImageFromVtkInfo(info, imgObj);
    imgObj->getDataArray()->resize(imgObj->getSize(), false);

    ::fwMemory::BufferObject::sptr buffObj = imgObj->getDataArray()->getBufferObject();
    boost::filesystem::path file = reader->GetFileName();
    buffObj->setIStreamFactory( std::make_shared< ImageStream<vtkXMLImageDataReader> >(file),
                                imgObj->getSizeInBytes());

}

//------------------------------------------------------------------------------

template< typename DATA_READER >
::fwData::Image::sptr lazyRead( const ::boost::filesystem::path &file, const ::fwJobs::Observer::sptr& job)
{
    vtkSmartPointer< DATA_READER > reader = vtkSmartPointer< DATA_READER >::New();
    reader->SetFileName(file.string().c_str());
    reader->UpdateInformation();

    ::fwData::Image::sptr imgObj;

    if(checkIfReadDataTypeIsImage(reader))
    {
        imgObj = ::fwData::Image::New();
        getInfo(reader, imgObj);

        job->finish();
    }

    return imgObj;
}


//------------------------------------------------------------------------------


void SeriesDBReader::read()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    const ::fwData::location::ILocation::VectPathType files = this->getFiles();
    const std::string instanceUID                           = ::fwTools::UUID::generateUUID();

    ::fwMedData::ModelSeries::ReconstructionVectorType recs;
    std::vector< std::string > errorFiles;
    for(const ::fwData::location::ILocation::VectPathType::value_type& file :  files)
    {
        vtkSmartPointer< vtkDataObject  > obj;
        ::fwData::Image::sptr img;
        ::fwData::Reconstruction::sptr rec;

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
        else if(file.extension().string() == ".vtu")
        {
            obj = getObj<vtkXMLGenericDataObjectReader>(file, m_job);
        }

        if (!img)
        {
            ::fwData::Object::sptr dataObj = getDataObject(obj, file);
            img                            = ::fwData::Image::dynamicCast(dataObj);
            rec                            = ::fwData::Reconstruction::dynamicCast(dataObj);
        }
        if(img)
        {
            ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();
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
        ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
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

::fwJobs::IJob::sptr SeriesDBReader::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO
