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

#include "fwTest/generator/Object.hpp"

#include "fwTest/generator/Image.hpp"
#include "fwTest/generator/Mesh.hpp"
#include "fwTest/generator/SeriesDB.hpp"

#include <data/Integer.hpp>
#include <data/Plane.hpp>
#include <data/ProcessObject.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/String.hpp>

#include <cmath>
#include <filesystem>

namespace fwTest
{
namespace generator
{

//------------------------------------------------------------------------------

data::StructureTraitsDictionary::sptr Object::createStructureTraitsDictionary()
{
    data::StructureTraitsDictionary::sptr structDico = data::StructureTraitsDictionary::New();
    data::StructureTraits::sptr skin                 = data::StructureTraits::New();
    std::string skinType                             = "Skin";
    skin->setType(skinType);
    skin->setClass(data::StructureTraits::ENVIRONMENT);
    data::Color::sptr skinColor = data::Color::New(255.0f/255.0f, 179.0f/255.0f, 140.0f/255.0f, 1.0f);
    skin->setColor(skinColor);
    data::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = data::StructureTraits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    data::StructureTraits::sptr liver = data::StructureTraits::New();
    liver->setType("Liver");
    liver->setClass(data::StructureTraits::ORGAN);
    liver->setColor(data::Color::New(204.0f/255.0f, 51.0f/255.0f, 51.0f/255.0f, 1.0));
    data::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = data::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    data::StructureTraits::sptr liverTumor = data::StructureTraits::New();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::StructureTraits::LESION);
    liverTumor->setColor(data::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    data::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    data::StructureTraits::sptr tumor = data::StructureTraits::New();
    tumor->setType("Tumor");
    tumor->setClass(data::StructureTraits::LESION);
    tumor->setColor(data::Color::New(0.0f, 0.0f, 1.0f, 1.0f));
    data::StructureTraits::CategoryContainer tumorCat(8);
    tumorCat[0] = data::StructureTraits::BODY;
    tumorCat[1] = data::StructureTraits::HEAD;
    tumorCat[2] = data::StructureTraits::NECK;
    tumorCat[3] = data::StructureTraits::THORAX;
    tumorCat[4] = data::StructureTraits::ABDOMEN;
    tumorCat[5] = data::StructureTraits::PELVIS;
    tumorCat[6] = data::StructureTraits::ARM;
    tumorCat[7] = data::StructureTraits::LEG;
    tumor->setCategories(tumorCat);

    data::StructureTraits::sptr colon = data::StructureTraits::New();
    colon->setType("Colon");
    colon->setClass(data::StructureTraits::ORGAN);
    colon->setColor(data::Color::New(1.0f, 179.0f/255.0f, 128.0f/255.0f, 1.0f));
    data::StructureTraits::CategoryContainer colonCat(1);
    colonCat[0] = data::StructureTraits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    data::StructureTraits::sptr caecum = data::StructureTraits::New();
    caecum->setType("Caecum");
    caecum->setClass(data::StructureTraits::FUNCTIONAL);
    caecum->setColor(data::Color::New(1.0f, 179.0f/255.0f, 128.0f/255.0f, 1.0f));
    data::StructureTraits::CategoryContainer caecumCat(1);
    caecumCat[0] = data::StructureTraits::ABDOMEN;
    caecum->setCategories(caecumCat);
    caecum->setAttachmentType("Colon");
    structDico->addStructure(caecum);

    return structDico;
}

//------------------------------------------------------------------------------

data::StructureTraits::sptr Object::createStructureTraits()
{
    data::StructureTraits::sptr pStructureTraits = data::StructureTraits::New();

    const std::string type                                  = "Liver";
    const data::StructureTraits::StructureClass structClass = data::StructureTraits::ORGAN;
    const data::StructureTraits::ROIExpression nativeExp    = "inter(world(type(Skin)),not(class(Organ)))";
    const data::StructureTraits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    data::Color::sptr color = data::Color::New();
    color->setRGBA(static_cast<float>(rand()%100) / 100.f,
                   static_cast<float>(rand()%100) / 100.f,
                   static_cast<float>(rand()%100) / 100.f,
                   static_cast<float>(rand()%100) / 100.f);
    pStructureTraits->setColor(color);

    data::StructureTraits::CategoryContainer categories(2);
    categories[0] = data::StructureTraits::ABDOMEN;
    categories[1] = data::StructureTraits::BODY;
    pStructureTraits->setCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

data::Composite::sptr Object::createROITraits()
{
    data::Composite::sptr composite = data::Composite::New();

    data::ROITraits::sptr roi = data::ROITraits::New();
    std::stringstream ss;
    ss << "ID_" << rand()%100;
    roi->setIdentifier(ss.str());

    roi->setEvaluatedExp("inter(skin),not(liver)");
    data::StructureTraits::sptr structure = Object::createStructureTraits();
    roi->setStructureTraits(structure);

    data::Node::sptr maskNode = Object::createNode();
    roi->setMaskOpNode(maskNode);

    (*composite)["ROITraits"]       = roi;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"]      = maskNode;

    return composite;
}

//------------------------------------------------------------------------------

data::Composite::sptr Object::createReconstructionTraits()
{
    data::Composite::sptr composite = data::Composite::New();

    data::ReconstructionTraits::sptr rec = data::ReconstructionTraits::New();

    std::stringstream ss;
    ss << "ID_" << rand()%100;
    rec->setIdentifier(ss.str());

    data::StructureTraits::sptr structure = Object::createStructureTraits();
    rec->setStructureTraits(structure);

    data::Node::sptr maskNode = Object::createNode();
    rec->setMaskOpNode(maskNode);

    data::Node::sptr meshNode = Object::createNode();
    rec->setMeshOpNode(meshNode);

    (*composite)["ReconstructionTraits"] = rec;
    (*composite)["StructureTraits"]      = structure;
    (*composite)["MaskOpNode"]           = maskNode;
    (*composite)["MeshOpNode"]           = meshNode;

    return composite;
}

//------------------------------------------------------------------------------

data::Node::sptr Object::createNode()
{
    data::Node::sptr node = data::Node::New();

    unsigned int nbInputs  = rand()%5;
    unsigned int nbOutputs = rand()%5;

    for (unsigned int i = 0; i < nbInputs; ++i)
    {
        node->addInputPort(Object::createPort());
    }

    for (unsigned int i = 0; i < nbOutputs; ++i)
    {
        node->addOutputPort(Object::createPort());
    }

    return node;
}

//------------------------------------------------------------------------------

data::Port::sptr Object::createPort()
{
    data::Port::sptr port = data::Port::New();

    std::stringstream ssId;
    ssId << "ID_" << rand()%100;
    port->setIdentifier(ssId.str());

    std::stringstream ssType;
    ssType << "Type_" << rand()%100;
    port->setType(ssType.str());

    return port;
}

//------------------------------------------------------------------------------

data::Color::sptr Object::randomizeColor()
{
    data::Color::sptr col = data::Color::New();
    col->setRGBA(static_cast<float>(rand()%100) / 100.f,
                 static_cast<float>(rand()%100) / 100.f,
                 static_cast<float>(rand()%100) / 100.f,
                 static_cast<float>(rand()%100) / 100.f);
    return col;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr Object::createTFColor(const unsigned char nbPoints,
                                                   const double window,
                                                   const double level)
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor( data::TransferFunction::TFColor( static_cast<float>(rand()%100) / 100.f,
                                                             static_cast<float>(rand()%100) / 100.f,
                                                             static_cast<float>(rand()%100) / 100.f,
                                                             static_cast<float>(rand()%100) / 100.f) );
    tf->setInterpolationMode( data::TransferFunction::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel(level);
    tf->setName( "TFColor" );
    tf->setWindow(window );

    for (unsigned char nb = 0; nb < nbPoints; ++nb)
    {
        double value = rand()%100 - level;
        tf->addTFColor( value,
                        data::TransferFunction::TFColor( static_cast<float>(rand()%100) / 100.f,
                                                         static_cast<float>(rand()%100) / 100.f,
                                                         static_cast<float>(rand()%100) / 100.f,
                                                         static_cast<float>(rand()%100) / 100.f) );
    }

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr Object::createTFColor()
{

    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor( data::TransferFunction::TFColor( 1.0f, 0.3f, 0.6f, 0.1f) );
    tf->setInterpolationMode( data::TransferFunction::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel( 900.6 );
    tf->setName( "TFColor" );
    tf->setWindow( -200.02 );

    tf->addTFColor( -40.33, data::TransferFunction::TFColor( 0.9f, 0.2f, 0.3f, 0.4f) );
    tf->addTFColor( 3,      data::TransferFunction::TFColor( 0.1f, 0.2f, 0.9f, 0.4f) ); // Invert point 3 <=> -0.2,
    // for tests
    tf->addTFColor( -0.2,   data::TransferFunction::TFColor( 0.1f, 0.9f, 0.3f, 0.4f) );
    tf->addTFColor( 150,    data::TransferFunction::TFColor( 0.1f, 0.2f, 0.3f, 0.9f) );

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

data::Material::sptr Object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::Color::sptr ambient = data::Color::New();
    ambient->setRGBA(0.5f, 0.5f, 0.5f, 0.5f);

    data::Color::sptr diffuse = data::Color::New();
    diffuse->setRGBA(0.8f, 0.2f, 0.5f, 0.4f);

    data::Material::sptr material = data::Material::New();

    material->setAmbient(data::Object::copy( ambient ));
    material->setDiffuse(data::Object::copy( diffuse ));

    return material;
}

//------------------------------------------------------------------------------

data::ProcessObject::sptr Object::createProcessObject()
{
    const std::string IMAGEID1 = "myImage1";
    const std::string IMAGEID2 = "myImage2";
    const std::string FIELDID1 = "myField1";
    const std::string FIELDID2 = "myField2";
    data::Image::sptr image1   = data::Image::New();
    data::Image::sptr image2   = data::Image::New();
    data::Integer::sptr field1 = data::Integer::New(3);
    data::Integer::sptr field2 = data::Integer::New(8);

    // process
    data::ProcessObject::sptr po = data::ProcessObject::New();
    po->setInputValue(IMAGEID1, image1);
    po->setInputValue(FIELDID1, field1);
    po->setInputValue(FIELDID2, field2);
    po->setOutputValue(IMAGEID2, image2);
    return po;
}

//------------------------------------------------------------------------------

data::Point::sptr Object::generatePoint()
{
    std::array< double, 3> coord = {static_cast<double>(rand()%300),
                                    static_cast<double>(rand()%300),
                                    static_cast<double>(rand()%300)};
    data::Point::sptr point = data::Point::New();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

data::Plane::sptr Object::generatePlane()
{
    data::Plane::sptr plane = data::Plane::New();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((rand()%1) ? true : false));
    return plane;
}

//------------------------------------------------------------------------------

data::Resection::sptr Object::generateResection()
{

    data::Resection::sptr resection = data::Resection::New();

    resection->setName("Resection1");
    resection->setIsSafePart(((rand()%1) ? true : false));
    resection->setIsValid(((rand()%1) ? true : false));
    resection->setIsVisible(((rand()%1) ? true : false));
    data::Reconstruction::sptr recInput = data::Reconstruction::New();

    ::fwTest::generator::SeriesDB::generateReconstruction(recInput);
    data::Resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->setInputs(inputs);

    data::Reconstruction::sptr recOutput = data::Reconstruction::New();
    ::fwTest::generator::SeriesDB::generateReconstruction(recOutput);
    data::Resection::ResectionOutputs outputs;
    outputs.push_back(recOutput);
    resection->setOutputs(outputs);

    data::PlaneList::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    data::PlaneList::sptr planeList = data::PlaneList::New();
    planeList->setPlanes(planes);

    return resection;
}

//------------------------------------------------------------------------------

data::ResectionDB::sptr Object::generateResectionDB()
{
    data::ResectionDB::sptr resectionDB = data::ResectionDB::New();
    resectionDB->addResection(generateResection());
    return resectionDB;
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
