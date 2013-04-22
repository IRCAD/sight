/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLGenericDataObjectReader.h>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/dateAndTime.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/ImageSeries.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/helper/Mesh.hpp"
#include "vtkIO/SeriesDBReader.hpp"
#include "vtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOReaderRegisterMacro( ::vtkIO::SeriesDBReader );

namespace vtkIO
{
//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
                ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
template <typename T, typename FILE, typename PROGRESSOR>
vtkSmartPointer< vtkDataObject  > getObj(FILE &file, PROGRESSOR *progressor)
{
    vtkSmartPointer< T > reader = vtkSmartPointer< T >::New();
    reader->SetFileName(file.string().c_str());
    Progressor progress(reader, progressor->getSptr(), file.string());
    reader->Update();
    return reader->GetOutput();
}

void SeriesDBReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    const ::fwData::location::ILocation::VectPathType files = this->getFiles();
    const std::string instanceUID = ::fwTools::UUID::generateUUID();

    ::fwMedData::ModelSeries::ReconstructionVectorType recs;
    BOOST_FOREACH(const ::fwData::location::ILocation::VectPathType::value_type& file, files)
    {
        vtkSmartPointer< vtkDataObject  > obj;

        if(file.extension().string() == ".vtk")
        {
            obj = getObj<vtkGenericDataObjectReader>(file, this);
        }
        else if(file.extension().string() == ".vti")
        {
            obj = getObj<vtkXMLGenericDataObjectReader>(file, this);
        }
        else if(file.extension().string() == ".mhd")
        {
            obj = getObj<vtkMetaImageReader>(file, this);
        }

        vtkSmartPointer< vtkPolyData  > mesh = vtkPolyData::SafeDownCast(obj);
        vtkSmartPointer< vtkImageData  > img = vtkImageData::SafeDownCast(obj);
        if(mesh)
        {
            ::fwData::Mesh::sptr meshObj = ::fwData::Mesh::New();
            ::vtkIO::helper::Mesh::fromVTKMesh(mesh, meshObj);
            ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();
            rec->setMesh(meshObj);
            rec->setOrganName((file.has_stem())?file.stem().string():file.string());
            rec->setIsVisible(true);
            recs.push_back(rec);
        }
        else if(img)
        {
            try
            {
                ::fwData::Image::sptr imgObj = ::fwData::Image::New();
                ::vtkIO::fromVTKImage( img, imgObj);
                ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();
                this->initSeries(imgSeries, instanceUID);
                imgSeries->setImage(imgObj);
                seriesDB->getContainer().push_back(imgSeries);
            }
            catch( std::exception &e)
            {
                FW_RAISE("VTKImage to fwData::Image failed "<<e.what());
            }
        }
        else
        {
            FW_RAISE("SeriesDBReader cannot read VTK file : "<< file.string());
        }
    }

    // Adds loaded Reconstructions in SeriesDB
    if(!recs.empty())
    {
        ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
        this->initSeries(modelSeries, instanceUID);
        modelSeries->setReconstructionDB(recs);
        seriesDB->getContainer().push_back(modelSeries);
    }
}

//------------------------------------------------------------------------------

void  SeriesDBReader::initSeries(::fwMedData::Series::sptr series, const std::string& instanceUID)
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

std::string  SeriesDBReader::extension()
{
   return ".vtk";
}

} // namespace vtkIO
