/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <gdcmIO/writer/DicomPatientDBWriterManager.hpp>
#include <gdcmIO/reader/DicomPatientDBReader.hpp>

#include "DicomTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DicomTest );

const unsigned int NBPTS   = 100;
const unsigned int NBCELLS = 100;

//------------------------------------------------------------------------------

void DicomTest::setUp()
{
    // Set up context before running a test.
    setlocale(LC_NUMERIC,"C");

    // create patientDB
    m_originalPatientDB = createPatientDB();

    // save patientDB in fwXML
    const ::boost::filesystem::path PATH = "dicomTest";

    ::boost::filesystem::create_directories( PATH );

    ::gdcmIO::writer::DicomPatientDBWriterManager::NewSptr writer;
    writer->setObject(m_originalPatientDB);
    writer->setFolder(PATH);
    writer->write();

    // load patientDB
    m_readedPatientDB = ::fwData::PatientDB::New();
    ::gdcmIO::reader::DicomPatientDBReader::NewSptr reader;
    reader->setObject(m_readedPatientDB);
    reader->setFolder(PATH);
    reader->read();

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void DicomTest::tearDown()
{
    // Clean up after the test run.
    m_originalPatientDB.reset();
    m_readedPatientDB.reset();
}

//------------------------------------------------------------------------------

void DicomTest::checkPatient()
{
    //------------------------------------------------------------------------------
    // check patientDB
    CPPUNIT_ASSERT(m_readedPatientDB);
    CPPUNIT_ASSERT_EQUAL(m_originalPatientDB->getNumberOfPatients(), m_readedPatientDB->getNumberOfPatients());

    ::fwData::Patient::sptr pPatient = m_originalPatientDB->getPatients().front();
    ::fwData::Patient::sptr pPatient2 = m_readedPatientDB->getPatients().front();
    CPPUNIT_ASSERT_EQUAL(pPatient->getName(), pPatient2->getName());
    CPPUNIT_ASSERT_EQUAL(pPatient->getFirstname(), pPatient2->getFirstname());
    CPPUNIT_ASSERT_EQUAL(pPatient->getBirthdate(), pPatient2->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(pPatient->getIDDicom(), pPatient2->getIDDicom());
    CPPUNIT_ASSERT_EQUAL(pPatient->getIsMale(), pPatient2->getIsMale());
    CPPUNIT_ASSERT_EQUAL(pPatient->getNumberOfStudies(), pPatient2->getNumberOfStudies());

    //------------------------------------------------------------------------------
    // check study
    ::fwData::Study::sptr pStudy = pPatient->getStudies().front();
    ::fwData::Study::sptr pStudy2 = pPatient2->getStudies().front();

    std::string study2Hospital = pStudy2->getHospital();
    ::boost::algorithm::trim(study2Hospital);
    CPPUNIT_ASSERT_EQUAL(pStudy->getHospital(), study2Hospital);
    CPPUNIT_ASSERT_EQUAL(pStudy->getModality(), pStudy2->getModality());
    //CPPUNIT_ASSERT_EQUAL(pStudy->getAcquisitionZone(), pStudy2->getAcquisitionZone());
    CPPUNIT_ASSERT_EQUAL(pStudy->getNumberOfAcquisitions(), pStudy2->getNumberOfAcquisitions());
    CPPUNIT_ASSERT_EQUAL(pStudy2->getNumberOfAcquisitions(), size_t(2));

}

//------------------------------------------------------------------------------

void DicomTest::checkImage()
{
    ::fwData::Patient::sptr pPatient = m_originalPatientDB->getPatients().front();
    ::fwData::Patient::sptr pPatient2 = m_readedPatientDB->getPatients().front();
    ::fwData::Study::sptr pStudy = pPatient->getStudies().front();
    ::fwData::Study::sptr pStudy2 = pPatient2->getStudies().front();

    //------------------------------------------------------------------------------
    // check acquisition
    ::fwData::Acquisition::sptr pAcq = pStudy->getAcquisitions().front();
    ::fwData::Acquisition::sptr pAcq2 = pStudy2->getAcquisitions().front();
    CPPUNIT_ASSERT(pAcq);
    CPPUNIT_ASSERT(pAcq2);
    CPPUNIT_ASSERT_EQUAL((int)pAcq->getBitsPerPixel(), (int)pAcq2->getBitsPerPixel());

    //------------------------------------------------------------------------------
    // check image
    ::fwData::Image::sptr image = pAcq->getImage();
    ::fwData::Image::sptr image2 = pAcq2->getImage();
    CPPUNIT_ASSERT(image);
    CPPUNIT_ASSERT(image2);
    CPPUNIT_ASSERT_EQUAL(image->getNumberOfDimensions(), image2->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(image->getOrigin().back(), image2->getOrigin().back());
    CPPUNIT_ASSERT_EQUAL(image->getWindowCenter(), image2->getWindowCenter());
    CPPUNIT_ASSERT_EQUAL(image->getWindowWidth(), image2->getWindowWidth());
    CPPUNIT_ASSERT_EQUAL(image->getSize()[0], image2->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[1], image2->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[2], image2->getSize()[2]);
    CPPUNIT_ASSERT_EQUAL(image->getSpacing()[0], image2->getSpacing()[0]);
    CPPUNIT_ASSERT_EQUAL(image->getSpacing()[1], image2->getSpacing()[1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[2], image2->getSpacing()[2], 0.000001);
    CPPUNIT_ASSERT_EQUAL(pAcq->getNumberOfReconstructions(), pAcq2->getNumberOfReconstructions());
}

//------------------------------------------------------------------------------

void DicomTest::checkReconstruction()
{
    ::fwData::Patient::sptr pPatient = m_originalPatientDB->getPatients().front();
    ::fwData::Patient::sptr pPatient2 = m_readedPatientDB->getPatients().front();
    ::fwData::Study::sptr pStudy = pPatient->getStudies().front();
    ::fwData::Study::sptr pStudy2 = pPatient2->getStudies().front();
    ::fwData::Acquisition::sptr pAcq = pStudy->getAcquisitions().front();
    ::fwData::Acquisition::sptr pAcq2 = pStudy2->getAcquisitions().front();

    //------------------------------------------------------------------------------
    // check reconstruction
    ::fwData::Reconstruction::sptr pRec  = pAcq->getReconstructions().front();
    ::fwData::Reconstruction::sptr pRec2 = pAcq2->getReconstructions().front();
    CPPUNIT_ASSERT(pRec);
    CPPUNIT_ASSERT(pRec2);
    CPPUNIT_ASSERT_EQUAL(pRec->getIsVisible(), pRec2->getIsVisible());

    //------------------------------------------------------------------------------
    // check mesh
    ::fwData::Mesh::csptr mesh  = pRec->getMesh();
    ::fwData::Mesh::csptr mesh2 = pRec2->getMesh();
    CPPUNIT_ASSERT(mesh);
    CPPUNIT_ASSERT(mesh2);
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), mesh2->getNumberOfCells());

    ::fwData::Material::csptr pMat  = pRec->getMaterial();
    ::fwData::Material::csptr pMat2 = pRec2->getMaterial();
    CPPUNIT_ASSERT(pMat);
    CPPUNIT_ASSERT(pMat2);

    //------------------------------------------------------------------------------
    // check color
    ::fwData::Color::sptr pCol  = pMat->ambient();
    ::fwData::Color::sptr pCol2 = pMat2->ambient();
    CPPUNIT_ASSERT(pCol);
    CPPUNIT_ASSERT(pCol2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pCol->red(), pCol2->red(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pCol->green(), pCol2->green(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pCol->blue(), pCol2->blue(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pCol->alpha(), pCol2->alpha(), 0.001);
}

//------------------------------------------------------------------------------

void DicomTest::checkLandmark()
{
    ::fwData::Patient::sptr pPatient = m_originalPatientDB->getPatients().front();
    ::fwData::Patient::sptr pPatient2 = m_readedPatientDB->getPatients().front();
    ::fwData::Study::sptr pStudy = pPatient->getStudies().front();
    ::fwData::Study::sptr pStudy2 = pPatient2->getStudies().front();
    ::fwData::Acquisition::sptr pAcq = pStudy->getAcquisitions().front();
    ::fwData::Acquisition::sptr pAcq2 = pStudy2->getAcquisitions().front();
    ::fwData::Image::sptr image = pAcq->getImage();
    ::fwData::Image::sptr image2 = pAcq2->getImage();

    //------------------------------------------------------------------------------
    // check landmark
    CPPUNIT_ASSERT(image2->getField( ::fwComEd::Dictionary::m_imageLandmarksId));
    ::fwData::PointList::sptr landmarks1 = image->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
    ::fwData::PointList::sptr landmarks2 = image2->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
    CPPUNIT_ASSERT_EQUAL(landmarks1->getPoints().size(), landmarks2->getPoints().size());
    ::fwData::Point::sptr point1 = landmarks1->getPoints().front();
    ::fwData::Point::sptr point2 = landmarks2->getPoints().front();

    // Landmarks x and y coordinates are rounded to one decimal
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[0], point2->getCoord()[0], 0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[1], point2->getCoord()[1], 0.5);
    // Landmarks z coordinates is repositioned on image slice
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getCoord()[2], point2->getCoord()[2], image->getSpacing()[2]/2.);

    std::string label1 = point1->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value();
    std::string label2 = point2->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(label1, label2);
}

//------------------------------------------------------------------------------

void DicomTest::checkDistance()
{
    ::fwData::Patient::sptr pPatient = m_originalPatientDB->getPatients().front();
    ::fwData::Patient::sptr pPatient2 = m_readedPatientDB->getPatients().front();
    ::fwData::Study::sptr pStudy = pPatient->getStudies().front();
    ::fwData::Study::sptr pStudy2 = pPatient2->getStudies().front();
    ::fwData::Acquisition::sptr pAcq = pStudy->getAcquisitions().front();
    ::fwData::Acquisition::sptr pAcq2 = pStudy2->getAcquisitions().front();
    ::fwData::Image::sptr image = pAcq->getImage();
    ::fwData::Image::sptr image2 = pAcq2->getImage();

    //------------------------------------------------------------------------------
    // check distance
    ::fwData::Vector::sptr vectDist1;
    vectDist1 = image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    CPPUNIT_ASSERT(vectDist1);

    ::fwData::Vector::sptr vectDist2;
    vectDist2 = image2->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    CPPUNIT_ASSERT(vectDist2);

    CPPUNIT_ASSERT_EQUAL(vectDist1->size(), vectDist2->size());

    ::fwData::PointList::sptr pl1 = ::fwData::PointList::dynamicCast(vectDist2->front());
    ::fwData::Point::sptr pt11 = pl1->getPoints()[0];
    ::fwData::Point::sptr pt12 = pl1->getPoints()[1];

    ::fwData::PointList::sptr pl2 = ::fwData::PointList::dynamicCast(vectDist2->front());
    ::fwData::Point::sptr pt21 = pl2->getPoints()[0];
    ::fwData::Point::sptr pt22 = pl2->getPoints()[1];

    // x and y coordinates are rounded to one decimal
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt11->getCoord()[0], pt21->getCoord()[0], 0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt11->getCoord()[1], pt21->getCoord()[1], 0.5);
    // z coordinates is repositioned on image slice
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt11->getCoord()[2], pt21->getCoord()[2], image->getSpacing()[2]/2.);

    // x and y coordinates are rounded to one decimal
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt12->getCoord()[0], pt22->getCoord()[0], 0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt12->getCoord()[1], pt22->getCoord()[1], 0.5);
    // z coordinates is repositioned on image slice
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pt12->getCoord()[2], pt22->getCoord()[2], image->getSpacing()[2]/2.);
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr DicomTest::createPatientDB()
{
    // patient informations
    const std::string PATIENT_NAME          = "NomPatient1" ;
    const std::string PATIENT_FIRSTNAME     = "PrenomPatient1" ;
    const std::string PATIENT_IDDICOM       = "IDDICOM" ;
    const std::string PATIENT_BIRTHDATE     = "2009-May-27 00:00:00";
    const bool PATIENT_SEX                  = true;
    const ::boost::int32_t  PATIENT_DBID    = 457;

    // studies informations
    const std::string STUDY1_HOSPITAL       = "Study hospital" ;
    const std::string STUDY1_MODALITY       = "CT" ;
    const std::string STUDY1_ZONE           = "Study zone" ;

    // image informations
    const ::boost::uint8_t IMG1_DIMENSION   = 3 ;
    std::vector<double> IMG1_VSPACING (3,2);
    IMG1_VSPACING[0] = 0.1234;
    IMG1_VSPACING[1] = 0.246;
    IMG1_VSPACING[2] = 0.48;

    std::vector<double> IMG1_ORIGIN (3,10);

    ::boost::int32_t IMG1_SIZEX = 10 ;
    ::boost::int32_t IMG1_SIZEY = 20;
    ::boost::int32_t IMG1_SIZEZ = 30 ;
    ::fwData::Image::SizeType IMG1_VSIZE;
    IMG1_VSIZE.push_back(IMG1_SIZEX);
    IMG1_VSIZE.push_back(IMG1_SIZEY);
    IMG1_VSIZE.push_back(IMG1_SIZEZ);
    int size = IMG1_SIZEX*IMG1_SIZEY*IMG1_SIZEZ;

    const double IMG1_WINDOWCENTER = 86 ;
    const double IMG1_WINDOWWIDTH = 345 ;
    const double IMG1_RESCALEINTERCEPT = 1 ;

    ::boost::uint8_t IMG1_BITSPERPIXEL = 16 ;
    ::fwTools::Type IMG1_PIXELTYPE = ::fwTools::Type::create("int16");
    signed short * buffer1 = new signed short[size];
    for (int i=0 ; i< size ; i++)
    {
        buffer1[i] = 100;
    }

    ::boost::uint8_t IMG2_BITSPERPIXEL = 8 ;
    ::fwTools::Type IMG2_PIXELTYPE = ::fwTools::Type::create("uint8");
    unsigned char * buffer2 = new unsigned char[size];
    for (int i=0 ; i< size ; i++)
    {
        buffer2[i] = 200;
    }

    //create patientDB
    ::fwData::PatientDB::NewSptr pPatientDB;
    ::fwData::Patient::NewSptr pPatient;
    pPatientDB->addPatient(pPatient);

    pPatient->setCRefName( PATIENT_NAME );
    pPatient->setCRefFirstname( PATIENT_FIRSTNAME );
    pPatient->setCRefIDDicom(   PATIENT_IDDICOM );
    pPatient->setCRefBirthdate( ::boost::posix_time::time_from_string( PATIENT_BIRTHDATE ));
    pPatient->setIsMale( PATIENT_SEX );
    pPatient->setDbID( PATIENT_DBID );

    ::fwData::Study::NewSptr pStudy1;
    pPatient->addStudy(pStudy1);

    pStudy1->setCRefHospital( STUDY1_HOSPITAL );
    pStudy1->setCRefModality( STUDY1_MODALITY );
    pStudy1->setCRefAcquisitionZone( STUDY1_ZONE );

    ::fwData::Acquisition::NewSptr pAcq1;
    pStudy1->addAcquisition(pAcq1);
    pAcq1->setBitsPerPixel(IMG1_BITSPERPIXEL);

    ::fwData::Image::NewSptr pImage1;
    pAcq1->setImage(pImage1);

    pImage1->setSize( IMG1_VSIZE );
    pImage1->setOrigin( IMG1_ORIGIN );
    pImage1->setSpacing( IMG1_VSPACING );
    pImage1->setType( IMG1_PIXELTYPE );
    ::fwData::Array::NewSptr array1;
    array1->setBuffer( static_cast<void *>(buffer1), true, IMG1_PIXELTYPE, IMG1_VSIZE, 1 );
    pImage1->setDataArray( array1 );
    pImage1->setWindowCenter(IMG1_WINDOWCENTER);
    pImage1->setWindowWidth(IMG1_WINDOWWIDTH );
    ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(pImage1);
    ::fwData::PointList::sptr landmarks = pImage1->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
    ::fwData::Point::sptr point = ::fwData::Point::New(2.6f, 1.2f, 4.5f);
    point->setField( ::fwComEd::Dictionary::m_labelId , ::fwData::String::New("Label1") );
    landmarks->getRefPoints().push_back(point);
    ::fwData::Point::sptr point2 = ::fwData::Point::New(1.2f, 2.4f, 0.3f);
    point2->setField( ::fwComEd::Dictionary::m_labelId , ::fwData::String::New("Label2") );
    landmarks->getRefPoints().push_back(point2);
    ::fwData::Point::sptr point3 = ::fwData::Point::New(1.2f, 2.4f, (IMG1_VSIZE[2]-1) * IMG1_VSPACING[2]);
    point3->setField( ::fwComEd::Dictionary::m_labelId , ::fwData::String::New("toto") );
    landmarks->getRefPoints().push_back(point3);

    // Add distance
    ::fwData::PointList::NewSptr pl;
    ::fwData::Point::sptr pt1 = ::fwData::Point::New(0., 0., 0.);
    ::fwData::Point::sptr pt2 = ::fwData::Point::New((IMG1_VSIZE[0]-1) * IMG1_VSPACING[0], (IMG1_VSIZE[1]-1) * IMG1_VSPACING[1], (IMG1_VSIZE[2]-1) * IMG1_VSPACING[2]);
    pl->getRefPoints().push_back( pt1 );
    pl->getRefPoints().push_back( pt2 );

    ::fwData::Vector::sptr vectDist;
    vectDist = pImage1->setDefaultField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId, ::fwData::Vector::New());
    vectDist->getContainer().push_back(pl);


    ::fwData::Mesh::sptr mesh = this->generateMesh();
    ::fwData::Material::NewSptr material;
    material->ambient()->red()   = 0.;
    material->ambient()->green() = 0.;
    material->ambient()->blue()  = 1.;
    ::fwData::Reconstruction::NewSptr reconstruction;
    reconstruction->setMesh(mesh);
    reconstruction->setMaterial(material);
    reconstruction->setOrganName("portalVein");
    reconstruction->setStructureType("");
    reconstruction->setIsVisible(true);
    pAcq1->addReconstruction(reconstruction);

    ::fwData::Acquisition::NewSptr pAcq2;
    pStudy1->addAcquisition(pAcq2);
    pAcq2->setBitsPerPixel(IMG2_BITSPERPIXEL);

    ::fwData::Image::NewSptr pImage2;
    pAcq2->setImage(pImage2);


    pImage2->setSize( IMG1_VSIZE );
    pImage2->setOrigin( IMG1_ORIGIN );
    pImage2->setSpacing( IMG1_VSPACING );
    pImage2->setType(IMG2_PIXELTYPE);
    ::fwData::Array::NewSptr array2;
    array2->setBuffer( static_cast<void *>(buffer2), true, IMG2_PIXELTYPE, IMG1_VSIZE, 1 );
    pImage2->setDataArray( array2 );
    pImage2->setWindowCenter(IMG1_WINDOWCENTER);
    pImage2->setWindowWidth(IMG1_WINDOWWIDTH);
    ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(pImage2);

    return pPatientDB;
}

//------------------------------------------------------------------------------

::fwData::Mesh::sptr DicomTest::generateMesh()
{
    ::fwData::Mesh::NewSptr mesh;
    ::fwDataTools::MeshGenerator::generateTriangleMesh(mesh);
    ::fwDataTools::MeshGenerator::shakePoint(mesh);

    mesh->adjustAllocatedMemory();
    return mesh;
}

//------------------------------------------------------------------------------
