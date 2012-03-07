/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/MeshGenerator.hpp>

#include "ObjectGenerator.hpp"

using namespace boost::assign;


//------------------------------------------------------------------------------

::fwData::StructureTraitsDictionary::sptr ObjectGenerator::createStructureTraitsDictionary()
{
    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    ::fwData::StructureTraits::NewSptr skin;
    std::string skinType = "Skin";
    skin->setType(skinType);
    skin->setClass(::fwData::StructureTraits::ENVIRONMENT);
    ::fwData::Color::sptr skinColor = ::fwData::Color::New(255.0f/255.0f, 179.0f/255.0f, 140.0f/255.0f, 1.0f);
    skin->setColor(skinColor);
    ::fwData::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = ::fwData::StructureTraits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    ::fwData::StructureTraits::NewSptr liver;
    liver->setType("Liver");
    liver->setClass(::fwData::StructureTraits::ORGAN);
    liver->setColor(::fwData::Color::New(204.0f/255.0f, 51.0f/255.0f, 51.0f/255.0f, 1.0));
    ::fwData::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    ::fwData::StructureTraits::NewSptr liverTumor;
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(::fwData::StructureTraits::LESION);
    liverTumor->setColor(::fwData::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    ::fwData::StructureTraits::NewSptr tumor;
    tumor->setType("Tumor");
    tumor->setClass(::fwData::StructureTraits::LESION);
    tumor->setColor(::fwData::Color::New(0.0f, 0.0f, 1.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer tumorCat(8);
    tumorCat[0] = ::fwData::StructureTraits::BODY;
    tumorCat[1] = ::fwData::StructureTraits::HEAD;
    tumorCat[2] = ::fwData::StructureTraits::NECK;
    tumorCat[3] = ::fwData::StructureTraits::THORAX;
    tumorCat[4] = ::fwData::StructureTraits::ABDOMEN;
    tumorCat[5] = ::fwData::StructureTraits::PELVIS;
    tumorCat[6] = ::fwData::StructureTraits::ARM;
    tumorCat[7] = ::fwData::StructureTraits::LEG;
    tumor->setCategories(tumorCat);

    ::fwData::StructureTraits::NewSptr colon;
    colon->setType("Colon");
    colon->setClass(::fwData::StructureTraits::ORGAN);
    colon->setColor(::fwData::Color::New(1.0f, 179.0f/255.0f, 128.0f/255.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer colonCat(1);
    colonCat[0] = ::fwData::StructureTraits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    ::fwData::StructureTraits::NewSptr caecum;
    caecum->setType("Caecum");
    caecum->setClass(::fwData::StructureTraits::FUNCTIONAL);
    caecum->setColor(::fwData::Color::New(1.0f, 179.0f/255.0f, 128.0f/255.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer caecumCat(1);
    caecumCat[0] = ::fwData::StructureTraits::ABDOMEN;
    caecum->setCategories(caecumCat);
    caecum->setAttachmentType("Colon");
    structDico->addStructure(caecum);

    return structDico;
}

//------------------------------------------------------------------------------

::fwData::StructureTraits::sptr ObjectGenerator::createStructureTraits()
{
    ::fwData::StructureTraits::NewSptr pStructureTraits;

    const std::string type = "Liver";
    const ::fwData::StructureTraits::StructureClass structClass = ::fwData::StructureTraits::ORGAN;
    const ::fwData::StructureTraits::ROIExpression nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    const ::fwData::StructureTraits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    ::fwData::Color::NewSptr color;
    color->setRGBA(rand()%100 / 100., rand()%100 / 100., rand()%100 / 100., rand()%100 / 100.);
    pStructureTraits->setColor(color);

    ::fwData::StructureTraits::CategoryContainer categories(2);
    categories[0] = ::fwData::StructureTraits::ABDOMEN;
    categories[1] = ::fwData::StructureTraits::BODY;
    pStructureTraits->setCRefCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr ObjectGenerator::createROITraits()
{
    ::fwData::Composite::NewSptr composite;

    ::fwData::ROITraits::NewSptr roi;
    std::stringstream ss;
    ss << "ID_" << rand()%100;
    roi->setIdentifier(ss.str());

    roi->setEvaluatedExp("inter(skin),not(liver)");
    ::fwData::StructureTraits::sptr structure = ObjectGenerator::createStructureTraits();
    roi->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = ObjectGenerator::createNode();
    roi->setMaskOpNode(maskNode);

    (*composite)["ROITraits"] = roi;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"] = maskNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr ObjectGenerator::createReconstructionTraits()
{
    ::fwData::Composite::NewSptr composite;

    ::fwData::ReconstructionTraits::NewSptr rec;

    std::stringstream ss;
    ss << "ID_" << rand()%100;
    rec->setIdentifier(ss.str());

    ::fwData::StructureTraits::sptr structure = ObjectGenerator::createStructureTraits();
    rec->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = ObjectGenerator::createNode();
    rec->setMaskOpNode(maskNode);

    ::fwData::Node::sptr meshNode = ObjectGenerator::createNode();
    rec->setMeshOpNode(meshNode);

    (*composite)["ReconstructionTraits"] = rec;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"] = maskNode;
    (*composite)["MeshOpNode"] = meshNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ObjectGenerator::createNode()
{
    ::fwData::Node::NewSptr node;

    unsigned int nbInputs = rand()%5;
    unsigned int nbOutputs = rand()%5;

    for (unsigned int i=0 ; i<nbInputs ; ++i)
    {
        node->addInputPort(ObjectGenerator::createPort());
    }

    for (unsigned int i=0 ; i<nbOutputs ; ++i)
    {
        node->addOutputPort(ObjectGenerator::createPort());
    }

    return node;
}

//------------------------------------------------------------------------------

::fwData::Port::sptr ObjectGenerator::createPort()
{
    ::fwData::Port::NewSptr port;

    std::stringstream ssId;
    ssId << "ID_" << rand()%100;
    port->setIdentifier(ssId.str());

    std::stringstream ssType;
    ssType << "Type_" << rand()%100;
    port->setType(ssType.str());

    return port;
}

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
    ::fwData::Image::SpacingType IMG_SPACING (3,2.5);
    ::fwData::Image::OriginType IMG_ORIGIN  (3,12.3);
    ::fwData::Image::SizeType::value_type SIZE = 50 ;
    ::fwData::Image::SizeType IMG_SIZE (3,SIZE);
    ::fwTools::Type type("uint8");

    ::fwDataTools::Image::generateImage(pImage, IMG_SIZE, IMG_SPACING, IMG_ORIGIN, type);


    const double IMG_WINDOWCENTER = 86 ;
    const double IMG_WINDOWWIDTH = 345 ;
    pImage->setWindowCenter(IMG_WINDOWCENTER);
    pImage->setWindowWidth(IMG_WINDOWWIDTH);

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

    int min, max, step;
    int val1 = rand()%2000 - 1000;
    int val2 = rand()%2000 - 1000;
    int val3 = rand()%50 + 1;
    min = std::min(val1, val2);
    max = std::max(val1, val2) + 1;
    step = ceil((double) ((max - min) / val3));
    for(int i = min; i <= max ; i+=step )
    {
        tf->getColor(i)->deepCopy(ObjectGenerator::createColor());
    }
    return tf;
}

//------------------------------------------------------------------------------
