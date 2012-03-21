/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/DictionaryOrgan.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/Plane.hpp>
#include <fwData/ProcessObject.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/MeshGenerator.hpp>
#include <fwDataTools/Patient.hpp>
#include "fwDataTools/ObjectGenerator.hpp"

#include <fwMath/IntrasecTypes.hpp>

using namespace boost::assign;

namespace fwDataTools
{

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
    color->setRGBA(rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f);
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

::fwData::PatientDB::sptr ObjectGenerator::createPatientDB(const unsigned char nbPatient,
                                                           const unsigned char nbStudy,
                                                           const unsigned char nbAcquisition,
                                                           const unsigned char nbReconstruction)
{
    ::fwData::PatientDB::NewSptr pPatientDB;
    for (unsigned char nb=0 ; nb<nbPatient ; ++nb)
    {
        ::fwData::Patient::NewSptr pPatient;
        ::fwDataTools::Patient::generatePatient(pPatient, nbStudy, nbAcquisition, nbReconstruction);
        pPatientDB->addPatient(pPatient);
    }
    return pPatientDB;
}

//------------------------------------------------------------------------------

::fwData::Color::sptr ObjectGenerator::randomizeColor()
{
    ::fwData::Color::NewSptr col;
    col->setRGBA(rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f);
    return col;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr ObjectGenerator::randomizeArray(const std::string type, ::fwData::Array::SizeType sizes)
{
    ::fwData::Array::NewSptr array;

    array->resize( ::fwTools::Type::create(type), sizes, 1, true );

    Image::randomizeArray( array );

    return array;
}

//------------------------------------------------------------------------------

::fwData::TransfertFunction_VERSION_II::sptr ObjectGenerator::createTFColor(const unsigned char nbPoints, const double window, const double level)
{
    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    tf->setBackgroundColor( ::fwData::TransfertFunction_VERSION_II::TFColor( rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f) );
    tf->setInterpolationMode( ::fwData::TransfertFunction_VERSION_II::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel(level);
    tf->setName( "TFColor" );
    tf->setWindow(window );

    for (unsigned char nb=0 ; nb<nbPoints ; ++nb)
    {
        double value = rand()%100 - level;
        tf->addTFColor( value, ::fwData::TransfertFunction_VERSION_II::TFColor( rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f ));
    }

    ::fwData::String::NewSptr myString ("fieldStringValue");
    tf->setField_NEWAPI( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

::fwData::TransfertFunction_VERSION_II::sptr ObjectGenerator::createTFColor()
{

    ::fwData::TransfertFunction_VERSION_II::NewSptr tf;

    tf->setBackgroundColor( ::fwData::TransfertFunction_VERSION_II::TFColor( 1.0f, 0.3f, 0.6f, 0.1f) );
    tf->setInterpolationMode( ::fwData::TransfertFunction_VERSION_II::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel( 900.6 );
    tf->setName( "TFColor" );
    tf->setWindow( -200.02 );

    tf->addTFColor( -40.33, ::fwData::TransfertFunction_VERSION_II::TFColor( 0.9f, 0.2f, 0.3f, 0.4f) );
    tf->addTFColor( 3,      ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1f, 0.2f, 0.9f, 0.4f) ); // Invert point 3 <=> -0.2, for tests
    tf->addTFColor( -0.2,   ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1f, 0.9f, 0.3f, 0.4f) );
    tf->addTFColor( 150,    ::fwData::TransfertFunction_VERSION_II::TFColor( 0.1f, 0.2f, 0.3f, 0.9f) );

    ::fwData::String::NewSptr myString ("fieldStringValue");
    tf->setField_NEWAPI( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr ObjectGenerator::createMaterial()
{
    // use the default value MODE_PHONG,MODE_SURFACE, MODE_STANDARD

    ::fwData::Color::NewSptr ambient;
    ambient->setRGBA(0.5f,0.5f,0.5f,0.5f);

    ::fwData::Color::NewSptr diffuse;
    diffuse->setRGBA(0.8f,0.2f,0.5f,0.4f);

    ::fwData::Material::NewSptr material;

    material->ambient()->deepCopy( ambient );
    material->diffuse()->deepCopy( diffuse );

    return material;
}

//------------------------------------------------------------------------------

::fwData::ProcessObject::sptr ObjectGenerator::createProcessObject()
{
    const std::string IMAGEID1 = "myImage1";
    const std::string IMAGEID2 = "myImage2";
    const std::string FIELDID1 = "myField1";
    const std::string FIELDID2 = "myField2";
    ::fwData::Image::sptr image1 = ::fwData::Image::New();
    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    ::fwData::Integer::sptr field1 = ::fwData::Integer::New(3);
    ::fwData::Integer::sptr field2 = ::fwData::Integer::New(8);

    // process
    ::fwData::ProcessObject::NewSptr po;
    po->setInputValue(IMAGEID1, image1);
    po->setInputValue(FIELDID1, field1);
    po->setInputValue(FIELDID2, field2);
    po->setOutputValue(IMAGEID2, image2);
    return po;
}


//------------------------------------------------------------------------------

::fwData::Point::sptr ObjectGenerator::generatePoint()
{
    fwVec3d coord = {{rand()%300, rand()%300, rand()%300}};
    ::fwData::Point::NewSptr point;
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

::fwData::Plane::sptr ObjectGenerator::generatePlane()
{
    ::fwData::Plane::NewSptr plane;
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection((rand()%1 ? true :false));
    return plane;
}

//------------------------------------------------------------------------------

::fwData::Resection::sptr ObjectGenerator::generateResection()
{

    ::fwData::Resection::NewSptr resection;

    resection->setName("Resection1");
    resection->setIsSafePart((rand()%1 ? true : false));
    resection->setIsValid((rand()%1 ? true : false));
    resection->setIsVisible((rand()%1 ? true : false));
    ::fwData::Reconstruction::NewSptr recInput;

    ::fwDataTools::Patient::generateReconstruction(recInput);
    ::fwData::Resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->setInputs(inputs);

    ::fwData::Reconstruction::NewSptr recOutput;
    ::fwDataTools::Patient::generateReconstruction(recOutput);
    ::fwData::Resection::ResectionOutputs outputs;
    outputs.push_back(recOutput);
    resection->setOutputs(outputs);

    ::fwData::PlaneList::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    ::fwData::PlaneList::NewSptr planeList;
    planeList->setPlanes(planes);

    return resection;

}

//------------------------------------------------------------------------------

::fwData::ResectionDB::sptr ObjectGenerator::generateResectionDB()
{

    ::fwData::ResectionDB::NewSptr resectionDB;
    resectionDB->addResection(generateResection());
    return resectionDB;

}
//------------------------------------------------------------------------------

::fwData::DictionaryOrgan::sptr ObjectGenerator::createDictionaryOrgan()
{
    const std::string CREFSTRUCTURETYPE = "CRefStructureType" ;
    const std::string CREFSALABEL   = "CRefSALabel" ;
    const bool ISMEDICALSTRUCTURE   = true ;
    const ::boost::uint32_t AOCOLOR = 10 ;
    const double AVGVOLUME  = 12.23 ;
    const double VOLSTDDEVIATION    = 21.01 ;
    const ::boost::uint32_t  NBEXAMS    = 9 ;
    const std::string CREFPOSITION  = "crefposition" ;
    const ::boost::filesystem::path CREFICONPATH    = ::boost::filesystem::path("C:/TMP") ;
    const double RED    = 23.17 ;
    const double GREEN  = 12.88 ;
    const double BLUE   = 4.002 ;
    const double ALPHA  = 35.06 ;
    const ::boost::filesystem::path CREFTEXTUREPATH = ::boost::filesystem::path("C:/TMP");
    const ::boost::uint32_t AVGTRIANGLENB   = 5 ;
    const double SEGMIN = 1.897 ;
    const double SEGMAX = 2.144 ;
    const std::string CREFMODALITY  = "crefmodality" ;
    const double SURFACEELASTICITY  = 3.455 ;
    const double SURFACERUPTURE = 3.33333 ;
    const double INTERIORRUPTURE    = 2.22222 ;
    const std::string CREFCOMMENT   = "crefcomment" ;
    std::string CREFBELONGSTO = "crefbelongsto" ;
    std::vector< std::string > VECTORCREFBELONGSTO ;
    VECTORCREFBELONGSTO.push_back(CREFBELONGSTO);

    // process
    ::fwData::DictionaryOrgan::NewSptr organDico;
    organDico->setCRefStructureType( CREFSTRUCTURETYPE );
    organDico->setCRefSALabel( CREFSALABEL );
    organDico->setIsMedicalStructure( ISMEDICALSTRUCTURE );
    organDico->setAOColor( AOCOLOR );
    organDico->setAvgVolume( AVGVOLUME );
    organDico->setVolStdDeviation( VOLSTDDEVIATION );
    organDico->setNbExams( NBEXAMS );
    organDico->setCRefPosition( CREFPOSITION );
    organDico->setCRefIconPath( CREFICONPATH );
    organDico->setRed( RED );
    organDico->setGreen( GREEN );
    organDico->setBlue( BLUE );
    organDico->setAlpha( ALPHA );
    organDico->setCRefTexturePath( CREFTEXTUREPATH );
    organDico->setAvgTriangleNb( AVGTRIANGLENB );
    organDico->setSegMin( SEGMIN );
    organDico->setSegMax( SEGMAX );
    organDico->setCRefModality( CREFMODALITY );
    organDico->setSurfaceElasticity( SURFACEELASTICITY );
    organDico->setSurfaceRupture( SURFACERUPTURE );
    organDico->setInteriorRupture( INTERIORRUPTURE );
    organDico->setCRefComment( CREFCOMMENT );
    organDico->setCRefBelongsTo( VECTORCREFBELONGSTO );
    return organDico;
}

//------------------------------------------------------------------------------

::fwData::Dictionary::sptr ObjectGenerator::createDictionary()
{
    std::string DICTIONARYORGANNAME = "myDictionary";

    ::fwData::Dictionary::NewSptr dico;

    ::fwData::DictionaryOrgan::sptr dictionaryOrgan( ::fwDataTools::ObjectGenerator::createDictionaryOrgan());
    dictionaryOrgan->setCRefStructureType(DICTIONARYORGANNAME) ;

    dico->setDictionaryOrgan(dictionaryOrgan) ;

    return dico;
}

//------------------------------------------------------------------------------

} // namespace fwDataTools

