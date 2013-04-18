/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Type.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/String.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Equipment.hpp>

#include "fwTest/generator/SeriesDB.hpp"
#include "fwTest/generator/Image.hpp"
#include "fwTest/generator/Mesh.hpp"


namespace fwTest
{
namespace generator
{

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SeriesDB::createSeriesDB(const unsigned char nbImgSeries,
                                                     const unsigned char nbModelSeries,
                                                     const unsigned char nbActivitySeries)
{
    Image::initRand();
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();
    for (unsigned char nb=0 ; nb<nbImgSeries ; ++nb)
    {
        ::fwMedData::Series::sptr imgSeries;
        imgSeries = SeriesDB::createImageSeries();
        seriesDB->getContainer().push_back(imgSeries);
    }

    for (unsigned char nb=0 ; nb<nbModelSeries ; ++nb)
    {
        ::fwMedData::Series::sptr modelSeries;
        modelSeries = SeriesDB::createModelSeries(rand()%15+1);
        seriesDB->getContainer().push_back(modelSeries);
    }

    for (unsigned char nb=0 ; nb<nbActivitySeries ; ++nb)
    {
        ::fwMedData::Series::sptr activitySeries;
        activitySeries = SeriesDB::createImageSeries();
        seriesDB->getContainer().push_back(activitySeries);
    }
    return seriesDB;
}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr SeriesDB::createPatient()
{
    ::fwMedData::Patient::sptr patient = ::fwMedData::Patient::New();
    const std::string PATIENT_NAME          = "NomSeriesDB1" ;
    const std::string PATIENT_FIRSTNAME     = "PrenomSeriesDB1" ;
    const std::string PATIENT_ID            = "456438375" ;
    const std::string PATIENT_BIRTHDATE     = "04-18-2013 09:53";
    const std::string PATIENT_SEX           = "O";

    patient->setName( PATIENT_NAME + "^" + PATIENT_FIRSTNAME );
    patient->setPatientId(   PATIENT_ID );
    patient->setBirthdate( PATIENT_BIRTHDATE );
    patient->setSex( PATIENT_SEX );

    return patient;
}

//------------------------------------------------------------------------------

::fwMedData::Study::sptr SeriesDB::createStudy()
{
    ::fwMedData::Study::sptr study = ::fwMedData::Study::New();
    // studies informations
    const std::string UID         = "1346357.1664.643101.421337.4123403" ;
    const std::string DATE        = "04-18-2013";
    const std::string TIME        = "09:59";
    const std::string PHYSICIAN   = "Dr Jekyl" ;
    const std::string DESCRIPTION = "Say 33." ;
    const std::string PATIENT_AGE = "42" ;

    study->setInstanceUID( UID );
    study->setDate( DATE );
    study->setTime( TIME );
    study->setReferringPhysicianName( PHYSICIAN );
    study->setDescription( DESCRIPTION );
    study->setPatientAge( PATIENT_AGE );

    return study;
}

//------------------------------------------------------------------------------

::fwMedData::Equipment::sptr SeriesDB::createEquipement()
{
    ::fwMedData::Equipment::sptr equipment = ::fwMedData::Equipment::New();
    const std::string INSTITUTION = "hospital" ;

    return equipment;
}


//------------------------------------------------------------------------------

void SeriesDB::generateSeriesInformation(::fwMedData::Series::sptr series)
{
    series->setPatient(SeriesDB::createPatient());
    series->setStudy(SeriesDB::createStudy());
    series->setEquipment(SeriesDB::createEquipement());

    const std::string UID         = "1346357.1664.482101.421337.4123403";
    const std::string MODALITY    = "CT";
    const std::string DATE        = "04-18-2013";
    const std::string TIME        = "10:10";
    const std::string DESCRIPTION = "Description";

    ::fwMedData::DicomValuesType performingPhysiciansName;
    performingPhysiciansName.push_back("Dr Jekyl");
    performingPhysiciansName.push_back("Dr House");
    performingPhysiciansName.push_back("Dr Einstein");
    series->setPerformingPhysiciansName(performingPhysiciansName);
}

//------------------------------------------------------------------------------

::fwMedData::ImageSeries::sptr SeriesDB::createImageSeries()
{
    ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();

    SeriesDB::generateSeriesInformation(imgSeries);

    ::fwData::Image::sptr image = ::fwData::Image::New();
    Image::generateRandomImage(image, ::fwTools::Type::s_INT16);

    return imgSeries;
}

//------------------------------------------------------------------------------

::fwMedData::ModelSeries::sptr SeriesDB::createModelSeries(unsigned char nbReconstruction)
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();

    SeriesDB::generateSeriesInformation(modelSeries);

    ::fwMedData::ModelSeries::ReconstructionVectorType recDB;
    for (unsigned char nb=0 ; nb<nbReconstruction ; ++nb)
    {
        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();

        SeriesDB::generateReconstruction(rec);

        recDB.push_back(rec);
    }
    modelSeries->setReconstructionDB(recDB);

    return modelSeries;
}

//------------------------------------------------------------------------------

::fwMedData::ActivitySeries::sptr SeriesDB::createActivitySeries()
{
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::New();

    SeriesDB::generateSeriesInformation(activitySeries);

    const std::string CONFIG_ID = "IdOfTheConfig";
    activitySeries->setActivityConfigId(CONFIG_ID);

    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    data->getContainer()["key1"] = ::fwData::String::New("ValueOfKey1");
    activitySeries->setData(data);

    return activitySeries;
}

//------------------------------------------------------------------------------

void SeriesDB::generateReconstruction(::fwData::Reconstruction::sptr rec)
{
    rec->setIsVisible(true);
    rec->setOrganName("liver");
    rec->setStructureType("LIVER");
    rec->setIsClosed(true);
    rec->setIsAutomatic(true);
    rec->setAvgVolume(rand()%1000/100.0);
    rec->setVolStdDeviation(rand()%1000/100.0);
    rec->setVolPctConfidence(rand()%1000/100.0);
    rec->setReconstructionTime("2007-Feb-24 18:55:00");
    rec->setMaskGenerated(true);
    rec->setLevel(rand()%127);
    rec->setLabel(rand()%500);
    rec->setGenerated3D(true);
    rec->setType3D("recType3D");
    rec->setPath("/tmp/myFile.trian");
    rec->setDbID(rand()%1000);

    ::fwData::Image::sptr img = ::fwData::Image::New();
    Image::generateRandomImage(img, ::fwTools::Type::create("uint16"));
    rec->setImage(img);

    ::fwData::Material::sptr material = ::fwData::Material::New();
    material->ambient()->red() = 0.75f;
    material->ambient()->green() = 0.10f;
    material->ambient()->blue() = 0.56f;
    material->ambient()->alpha() = 0.8f;
    material->diffuse()->red() = 0.85f;
    material->diffuse()->green() = 0.20f;
    material->diffuse()->blue() = 0.66f;
    material->diffuse()->alpha() = 0.9f;
    rec->setMaterial(material);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    Mesh::generateMesh( mesh );

    rec->setMesh(mesh);
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
