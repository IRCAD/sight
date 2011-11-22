/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include "ObjectGenerator.hpp"

using namespace boost::assign;

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr ObjectGenerator::createPatientDB()
{
    ::fwData::PatientDB::NewSptr pPatientDB;
    pPatientDB->addPatient(ObjectGenerator::createPatient());
    return pPatientDB;
}

//------------------------------------------------------------------------------

::fwData::Patient::sptr ObjectGenerator::createPatient()
{
    ::fwData::Patient::NewSptr pPatient;

    // patient informations
    const std::string PATIENT_NAME          = "NomPatient1" ;
    const std::string PATIENT_FIRSTNAME     = "PrenomPatient1" ;
    const std::string PATIENT_IDDICOM       = "IDDICOM" ;
    const std::string PATIENT_BIRTHDATE     = "2000-Feb-21 15:00:00";
    const bool PATIENT_SEX                  = true;
    const ::boost::int32_t  PATIENT_DBID    = 457;

    pPatient->setCRefName( PATIENT_NAME );
    pPatient->setCRefFirstname( PATIENT_FIRSTNAME );
    pPatient->setCRefIDDicom(   PATIENT_IDDICOM );
    pPatient->setCRefBirthdate( ::boost::posix_time::time_from_string( PATIENT_BIRTHDATE ));
    pPatient->setIsMale( PATIENT_SEX );
    pPatient->setDbID( PATIENT_DBID );

    pPatient->addStudy(ObjectGenerator::createStudy());

    return pPatient;
}

//------------------------------------------------------------------------------

::fwData::Study::sptr ObjectGenerator::createStudy()
{
    ::fwData::Study::NewSptr pStudy;

    // studies informations
    const std::string STUDY_HOSPITAL       = "hopital" ;
    const std::string STUDY_MODALITY       = "modality" ;
    const std::string STUDY_ZONE           = "IDDICOM" ;
    const std::string STUDY_RISID          = "risid569" ;
    const std::string STUDY_UID            = "UID569" ;
    const ::boost::int32_t  STUDY_DBID     = 156 ;

    pStudy->setCRefHospital( STUDY_HOSPITAL );
    pStudy->setCRefModality( STUDY_MODALITY );
    pStudy->setCRefAcquisitionZone( STUDY_ZONE );
    pStudy->setCRefRISId( STUDY_RISID );
    pStudy->setCRefUID( STUDY_UID );
    pStudy->setDbID( STUDY_DBID );

    pStudy->addAcquisition(ObjectGenerator::createAcquisition());

    return pStudy;
}

//------------------------------------------------------------------------------

::fwData::Acquisition::sptr ObjectGenerator::createAcquisition()
{
    ::fwData::Acquisition::NewSptr pAcq;

    // aquisitions informations
    const ::boost::uint8_t  ACQ_BITSPERPIXEL   = 48 ;
    const float ACQ_SLICETHICKNESS             = 0.8f ;
    const ::boost::uint8_t  ACQ_AXE            = 12 ;
    const bool ACQ_UNSIGNEDFLAG                = true ;
    const ::boost::uint32_t ACQ_INDEX          = 5345 ;
    const std::string ACQ_IMAGETYPE            = "refimageType" ;
    const std::string ACQ_IMAGEFORMAT          = "refimageFormat" ;
    const std::string ACQ_CREATIONDATE         = "2007-Feb-26 15:00:00";
    const bool ACQ_ISMAIN                      = true ;
    const bool ACQ_ISNORMALDIR                 = true ;
    const std::string ACQ_UID                  = "uid324" ;
    const ::boost::int32_t ACQ_DBID            = 6354 ;
    const ::boost::uint32_t ACQ_LABOID         = 357 ;
    const ::boost::uint32_t ACQ_NETID          = 21 ;
    const std::string ACQ_DATESENDTOLABOAT     = "2007-Feb-26 10:05:00";
    const std::string ACQ_DATERECEIVEFROMLABOAT= "2007-Feb-27 10:05:00";
    const std::string ACQ_DATESENDTOBDDAT      = "2007-Feb-27 10:05:00";
    const std::string ACQ_DATEDISPONIBILITYAT  = "2007-Feb-28 10:05:00";
    const double ACQ_PATIENTSIZE               = 45.1352 ;
    const double ACQ_PATIENTWEIGHT             = 89.6545 ;
    const double ACQ_RADIATIONS                = 89.6545 ;
    const std::string ACQ_MEDICALPRINTER       = "MedicalPrinter" ;
    const std::string ACQ_MEDICALPRINTERCORP   = "MedicalPrinterCorp" ;
    const std::string ACQ_PATIENTPOSITION      = "PatientPosition" ;

    pAcq->setBitsPerPixel(ACQ_BITSPERPIXEL);
    pAcq->setSliceThickness(ACQ_SLICETHICKNESS);
    pAcq->setAxe(ACQ_AXE);
    pAcq->setUnsignedFlag(ACQ_UNSIGNEDFLAG);
    pAcq->setAcquisitionIndex(ACQ_INDEX);
    pAcq->setCRefImageType(ACQ_IMAGETYPE);
    pAcq->setCRefImageFormat(ACQ_IMAGEFORMAT);
    pAcq->setCRefCreationDate( ::boost::posix_time::time_from_string(ACQ_CREATIONDATE) );
    pAcq->setIsMain(ACQ_ISMAIN);
    pAcq->setIsNormalDir(ACQ_ISNORMALDIR);
    pAcq->setCRefUID(ACQ_UID);
    pAcq->setDbID(ACQ_DBID);
    pAcq->setLaboID(ACQ_LABOID);
    pAcq->setNetID(ACQ_NETID);
    pAcq->setCRefDateSendToLaboAt( ::boost::posix_time::time_from_string(ACQ_DATESENDTOLABOAT) );
    pAcq->setCRefDateReceiveFromLaboAt(::boost::posix_time::time_from_string(ACQ_DATERECEIVEFROMLABOAT));
    pAcq->setCRefDateSendToBDDAt(::boost::posix_time::time_from_string(ACQ_DATESENDTOBDDAT));
    pAcq->setCRefDateDisponibilityAt(::boost::posix_time::time_from_string(ACQ_DATEDISPONIBILITYAT));
    pAcq->setPatientSize(ACQ_PATIENTSIZE);
    pAcq->setPatientWeight(ACQ_PATIENTWEIGHT);
    pAcq->setRadiations(ACQ_RADIATIONS);
    pAcq->setCRefMedicalPrinter(ACQ_MEDICALPRINTER);
    pAcq->setCRefMedicalPrinterCorp(ACQ_MEDICALPRINTERCORP);
    pAcq->setCRefPatientPosition(ACQ_PATIENTPOSITION);

    pAcq->setImage(ObjectGenerator::createImage());

    pAcq->addReconstruction(ObjectGenerator::createReconstruction());

    return pAcq;
}

//------------------------------------------------------------------------------

::fwData::Reconstruction::sptr ObjectGenerator::createReconstruction()
{
    ::fwData::Reconstruction::NewSptr pRecons;
    pRecons->setImage(ObjectGenerator::createImage());
    pRecons->setMesh(ObjectGenerator::createMesh());
    pRecons->setMaterial(ObjectGenerator::createMaterial());
    return pRecons;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr ObjectGenerator::createImage()
{
    ::fwData::Image::NewSptr pImage;

    // images informations
    const ::boost::uint8_t IMG_DIMENSION   = 3;
    const bool IMG_MANAGESBUFF             = true ;
    std::vector<double> IMG_VECTORCREFSPACING (3,2.5);
    std::vector<double> IMG_VECTORCREFORIGIN  (3,12.3);
    ::boost::int32_t IMG_CREFSIZE = 50 ;
    std::vector< ::boost::int32_t > IMG_VECTORCREFSIZE (3,IMG_CREFSIZE);
    const ::boost::filesystem::path IMG_CREFFILENAME   = ::boost::filesystem::path("D:/TMP") ;
    const double IMG_WINDOWCENTER = 86 ;
    const double IMG_WINDOWWIDTH = 345 ;
    const double IMG_RESCALEINTERCEPT = 1 ;

    pImage->setDimension(IMG_DIMENSION);
    pImage->setManagesBuff(IMG_MANAGESBUFF);
    pImage->setCRefSpacing(IMG_VECTORCREFSPACING);
    pImage->setCRefOrigin(IMG_VECTORCREFORIGIN);
    pImage->setCRefSize(IMG_VECTORCREFSIZE);
    pImage->setCRefFilename(IMG_CREFFILENAME);
    pImage->setWindowCenter(IMG_WINDOWCENTER);
    pImage->setWindowWidth(IMG_WINDOWWIDTH);
    pImage->setRescaleIntercept(IMG_RESCALEINTERCEPT);

    ::fwTools::DynamicType dtc;
    dtc.setType<unsigned char>();
    pImage->setPixelType(dtc);

    unsigned char * buff = new unsigned char[IMG_CREFSIZE*IMG_CREFSIZE*IMG_CREFSIZE];
    pImage->setBuffer(buff);

    return pImage;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr ObjectGenerator::createMaterial()
{
    ::fwData::Material::NewSptr mat;
    mat->setShadingMode(::fwData::Material::MODE_PHONG);
    mat->setRepresentationMode(::fwData::Material::MODE_SURFACE);
    mat->setOptionsMode(::fwData::Material::MODE_STANDARD);

    mat->ambient()->deepCopy(ObjectGenerator::createColor());
    mat->diffuse()->deepCopy(ObjectGenerator::createColor());
    return mat;
}

//------------------------------------------------------------------------------

::fwData::Color::sptr ObjectGenerator::createColor()
{
    ::fwData::Color::NewSptr col;
    col->setRGBA(rand()%100 / 100., rand()%100 / 100., rand()%100 / 100., rand()%100 / 100.);
    return col;
}

//------------------------------------------------------------------------------

::fwData::Mesh::sptr ObjectGenerator::createMesh()
{
    ::fwData::Mesh::NewSptr mesh;
    ::fwDataTools::MeshGenerator::generateTriangleQuadMesh(mesh);
    ::fwDataTools::MeshGenerator::shakePoint(mesh);
    ::fwDataTools::MeshGenerator::colorizeMeshPoints(mesh);
    ::fwDataTools::MeshGenerator::colorizeMeshCells(mesh);
    ::fwDataTools::MeshGenerator::generatePointNormals(mesh);
    ::fwDataTools::MeshGenerator::generateCellNormals(mesh);

    ::fwData::Array::sptr dataArray1 = ObjectGenerator::createArray();
    ::fwData::Array::sptr dataArray2 = ObjectGenerator::createArray();

    mesh->addDataArray("dataArray1", dataArray1);
    mesh->addDataArray("dataArray2", dataArray2);

    mesh->adjustAllocatedMemory();
    return mesh;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr ObjectGenerator::createArray()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 2;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);
    char *iter = array->begin<char>();

    unsigned int count = 0;
    for (; iter != array->end<char>() ; ++iter)
    {
        *iter = count++;
    }
    return array;
}

//------------------------------------------------------------------------------

::fwData::TriangularMesh::sptr ObjectGenerator::createTriangularMesh()
{
    ::fwData::TriangularMesh::NewSptr trian;
    const unsigned int nbPts   = 100;
    const unsigned int nbCells = 100;
    unsigned int i;

    std::vector<float> vPoint(3, 0.0);
    trian->points().resize(nbPts, vPoint);
    for( i=0 ; i<nbPts ; ++i )
    {
        ::fwData::TriangularMesh::PointContainer::value_type &vPoints = trian->points()[i];
        vPoints[0] = i;
        vPoints[1] = i+1;
        vPoints[2] = i+2;
    }

    std::vector<int> vCell(3, 0);
    trian->cells().resize(nbCells, vCell);
    for( i=0 ; i<nbCells ; ++i )
    {
        ::fwData::TriangularMesh::CellContainer::value_type &vCells = trian->cells()[i];
        vCells[0] = i%nbPts;
        vCells[1] = (i+1)%nbPts;
        vCells[2] = (i+2)%nbPts;
    }
    return trian;
}

//------------------------------------------------------------------------------

::fwData::TransfertFunction::sptr ObjectGenerator::createTransfertFunction()
{
    ::fwData::TransfertFunction::NewSptr tf;

    size_t min, max, step;
    int val1 = rand()%2000 - 1000;
    int val2 = rand()%2000 - 1000;
    int val3 = rand()%50 + 1;
    min = std::min(val1, val2);
    max = std::max(val1, val2) + 1;
    step = ceil((double) ((max - min) / val3));
    for(size_t i = min; i <= max ; i+=step )
    {
        tf->getColor(i)->deepCopy(ObjectGenerator::createColor());
    }
    return tf;
}

//------------------------------------------------------------------------------
