/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwTest/generator/SeriesDB.hpp"
#include "fwTest/generator/Image.hpp"
#include "fwTest/generator/Mesh.hpp"

#include <fwTools/Type.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
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

#include <sstream>


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
    for (unsigned char nb = 0; nb<nbImgSeries; ++nb)
    {
        ::fwMedData::Series::sptr imgSeries;
        imgSeries = SeriesDB::createImageSeries();
        seriesDB->getContainer().push_back(imgSeries);
    }

    for (unsigned char nb = 0; nb<nbModelSeries; ++nb)
    {
        ::fwMedData::Series::sptr modelSeries;
        modelSeries = SeriesDB::createModelSeries(rand()%15+1);
        seriesDB->getContainer().push_back(modelSeries);
    }

    for (unsigned char nb = 0; nb<nbActivitySeries; ++nb)
    {
        ::fwMedData::Series::sptr activitySeries;
        activitySeries = SeriesDB::createActivitySeries();
        seriesDB->getContainer().push_back(activitySeries);
    }
    return seriesDB;
}

//------------------------------------------------------------------------------

::fwMedData::Patient::sptr SeriesDB::createPatient()
{
    ::fwMedData::Patient::sptr patient = ::fwMedData::Patient::New();
    const std::string PATIENT_NAME      = "NomSeriesDB1";
    const std::string PATIENT_FIRSTNAME = "PrenomSeriesDB1";
    const std::string PATIENT_ID        = "4564383757";
    const std::string PATIENT_BIRTHDATE = "19710418";
    const std::string PATIENT_SEX       = "O ";

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

    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    // studies informations
    const std::string UID         = "1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str();
    const std::string DATE        = "20130418";
    const std::string TIME        = "095948.689872 ";
    const std::string PHYSICIAN   = "Dr^Jekyl";
    const std::string DESCRIPTION = "Say 33. ";
    const std::string PATIENT_AGE = "042Y";

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
    const std::string INSTITUTION = "hospital";
    equipment->setInstitutionName(INSTITUTION);

    return equipment;
}


//------------------------------------------------------------------------------

void SeriesDB::generateSeriesInformation(::fwMedData::Series::sptr series)
{
    series->setPatient(SeriesDB::createPatient());
    series->setStudy(SeriesDB::createStudy());
    series->setEquipment(SeriesDB::createEquipement());

    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    const std::string UID         = "1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str();
    const std::string MODALITY    = "CT";
    const std::string DATE        = "20130418";
    const std::string TIME        = "101010.101010 ";
    const std::string DESCRIPTION = "Description ";

    ::fwMedData::DicomValuesType performingPhysiciansName;
    performingPhysiciansName.push_back("Dr^Jekyl");
    performingPhysiciansName.push_back("Dr^House");
    performingPhysiciansName.push_back("Dr^Einstein ");
    series->setPerformingPhysiciansName(performingPhysiciansName);

    series->setInstanceUID(UID);
    series->setModality(MODALITY);
    series->setDate(DATE);
    series->setTime(TIME);
    series->setDescription(DESCRIPTION);
}

//------------------------------------------------------------------------------

::fwMedData::ImageSeries::sptr SeriesDB::createImageSeries()
{
    ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();

    SeriesDB::generateSeriesInformation(imgSeries);

    ::fwData::Image::sptr image = ::fwData::Image::New();
    Image::generateRandomImage(image, ::fwTools::Type::s_INT16);

    imgSeries->setImage(image);

    return imgSeries;
}

//------------------------------------------------------------------------------

::fwMedData::ModelSeries::sptr SeriesDB::createModelSeries(unsigned char nbReconstruction)
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();

    SeriesDB::generateSeriesInformation(modelSeries);

    ::fwMedData::ModelSeries::ReconstructionVectorType recDB;
    for (unsigned char nb = 0; nb<nbReconstruction; ++nb)
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
    data->getContainer()["key1"]   = ::fwData::String::New("ValueOfKey1");
    activitySeries->setData(data);

    return activitySeries;
}

//------------------------------------------------------------------------------

void SeriesDB::generateReconstruction(::fwData::Reconstruction::sptr rec)
{
    rec->setIsVisible(true);
    rec->setOrganName("liver");
    rec->setStructureType("Liver");

    ::fwData::Image::sptr img = ::fwData::Image::New();
    Image::generateRandomImage(img, ::fwTools::Type::create("uint16"));
    rec->setImage(img);

    ::fwData::Material::sptr material = ::fwData::Material::New();
    material->ambient()->red()        = 0.75f;
    material->ambient()->green()      = 0.10f;
    material->ambient()->blue()       = 0.56f;
    material->ambient()->alpha()      = 0.8f;
    material->diffuse()->red()        = 0.85f;
    material->diffuse()->green()      = 0.20f;
    material->diffuse()->blue()       = 0.66f;
    material->diffuse()->alpha()      = 0.9f;
    rec->setMaterial(material);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    Mesh::generateMesh( mesh );

    rec->setMesh(mesh);
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
