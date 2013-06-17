/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <numeric>
#include <algorithm>
#include <iosfwd>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/join.hpp>

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

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/dateAndTime.hpp>

#include <fwMemory/stream/in/IFactory.hpp>
#include <fwMemory/BufferObject.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/ImageSeries.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"
#include "fwVtkIO/SeriesDBReader.hpp"

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
    m_lazyMode(true)
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
vtkSmartPointer< vtkDataObject  > getObj(FILE &file, SeriesDBReader *progressor)
{
    vtkSmartPointer< T > reader = vtkSmartPointer< T >::New();
    reader->SetFileName(file.string().c_str());
    if(progressor)
    {
        Progressor progress(reader, progressor->getSptr(), file.string());
    }
    reader->Update();
    return reader->GetOutput();
}

//------------------------------------------------------------------------------


::fwData::Object::sptr getDataObject(const vtkSmartPointer< vtkDataObject  > &obj, const boost::filesystem::path &file)
{
    vtkSmartPointer< vtkPolyData  > mesh = vtkPolyData::SafeDownCast(obj);
    vtkSmartPointer< vtkImageData  > img = vtkImageData::SafeDownCast(obj);
    ::fwData::Object::sptr dataObj;
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
        m_bufferStream( ::boost::make_shared<BufferStreamType>(static_cast<char *>(m_lock.getBuffer()), m_bufferObject->getSize()) )
    {
        this->push(*m_bufferStream);
    }

    ~FilteringStream()
    {
        try { this->reset(); } catch (...) { }
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
    {}

protected:

    ::fwData::Image::sptr getImage()
    {
        if(!::boost::filesystem::exists(m_path))
        {
            FW_RAISE("file "<< m_path.string() << " does not exist anymore or has moved.");
        }

        vtkSmartPointer< vtkDataObject  > obj;
        obj = getObj<READER>(m_path, NULL);

        return ::fwData::Image::dynamicCast(getDataObject(obj, m_path));
    }

    SPTR(std::istream) get()
    {
        SPTR(FilteringStream) is
            = ::boost::make_shared< FilteringStream>( this->getImage() );

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
}

void getInfo(const vtkSmartPointer< vtkGenericDataObjectReader > &reader, const ::fwData::Image::sptr &imgObj)
{
    vtkSmartPointer< vtkImageData  > vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());
    imgObj->setType( ::fwVtkIO::TypeTranslator::translate( vtkImage->GetScalarType() ) );

    vtkSmartPointer< vtkStructuredPointsReader > imgReader =
        vtkSmartPointer< vtkStructuredPointsReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->ReadMetaData(info);

    updateImageFromVtkInfo(info, imgObj);

    ::fwMemory::BufferObject::sptr buffObj = imgObj->getDataArray()->getBufferObject();
    boost::filesystem::path file = reader->GetFileName();
    buffObj->setIStreamFactory( ::boost::make_shared< ImageStream<vtkStructuredPointsReader> >(file), imgObj->getSizeInBytes());
}


void getInfo(const vtkSmartPointer< vtkXMLGenericDataObjectReader > &reader, const ::fwData::Image::sptr &imgObj)
{
    vtkSmartPointer< vtkImageData  > vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());
    imgObj->setType( ::fwVtkIO::TypeTranslator::translate( vtkImage->GetScalarType() ) );

    vtkSmartPointer< vtkXMLImageDataReader > imgReader =
        vtkSmartPointer< vtkXMLImageDataReader >::New();
    imgReader->SetFileName(reader->GetFileName());

    vtkSmartPointer< vtkInformation > info = vtkSmartPointer< vtkInformation >::New();
    imgReader->UpdateInformation();
    imgReader->CopyOutputInformation(info,0);

    updateImageFromVtkInfo(info, imgObj);

    ::fwMemory::BufferObject::sptr buffObj = imgObj->getDataArray()->getBufferObject();
    boost::filesystem::path file = reader->GetFileName();
    buffObj->setIStreamFactory( ::boost::make_shared< ImageStream<vtkXMLImageDataReader> >(file), imgObj->getSizeInBytes());

}

//------------------------------------------------------------------------------

    template< typename DATA_READER >
::fwData::Image::sptr lazyRead( const ::boost::filesystem::path &file)
{
    vtkSmartPointer< DATA_READER > reader = vtkSmartPointer< DATA_READER >::New();
    reader->SetFileName(file.string().c_str());
    reader->UpdateInformation();

    ::fwData::Image::sptr imgObj;

    if(checkIfReadDataTypeIsImage(reader))
    {
        imgObj = ::fwData::Image::New();
        getInfo(reader, imgObj);

    }

    return imgObj;
}



//------------------------------------------------------------------------------


void SeriesDBReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    const ::fwData::location::ILocation::VectPathType files = this->getFiles();
    const std::string instanceUID = ::fwTools::UUID::generateUUID();

    ::fwMedData::ModelSeries::ReconstructionVectorType recs;
    std::vector< std::string > errorFiles;
    BOOST_FOREACH(const ::fwData::location::ILocation::VectPathType::value_type& file, files)
    {

        vtkSmartPointer< vtkDataObject  > obj;
        ::fwData::Image::sptr img;
        ::fwData::Reconstruction::sptr rec;

        if(file.extension().string() == ".vtk")
        {
            if(m_lazyMode)
            {
                img = lazyRead<vtkGenericDataObjectReader>(file);
            }
            if (!img)
            {
                obj = getObj<vtkGenericDataObjectReader>(file, this);
            }
        }
        else if(file.extension().string() == ".vti")
        {
            if(m_lazyMode)
            {
                img = lazyRead<vtkXMLGenericDataObjectReader>(file);
            }
            if (!img)
            {
                obj = getObj<vtkXMLGenericDataObjectReader>(file, this);
            }
        }
        else if(file.extension().string() == ".mhd")
        {
            obj = getObj<vtkMetaImageReader>(file, this);
        }

        if (!img)
        {
            ::fwData::Object::sptr dataObj = getDataObject(obj, file);
            img = ::fwData::Image::dynamicCast(dataObj);
            rec = ::fwData::Reconstruction::dynamicCast(dataObj);
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

std::string  SeriesDBReader::extension()
{
    return ".vtk";
}

} // namespace fwVtkIO
