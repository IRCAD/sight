/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwData/Integer.hpp>
#include <fwData/Plane.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/String.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <filesystem>

#include <cmath>

namespace fwTest
{
namespace generator
{

//------------------------------------------------------------------------------

::fwData::StructureTraitsDictionary::sptr Object::createStructureTraitsDictionary()
{
    ::fwData::StructureTraitsDictionary::sptr structDico = ::fwData::StructureTraitsDictionary::New();
    ::fwData::StructureTraits::sptr skin                 = ::fwData::StructureTraits::New();
    std::string skinType = "Skin";
    skin->setType(skinType);
    skin->setClass(::fwData::StructureTraits::ENVIRONMENT);
    ::fwData::Color::sptr skinColor = ::fwData::Color::New(255.0f/255.0f, 179.0f/255.0f, 140.0f/255.0f, 1.0f);
    skin->setColor(skinColor);
    ::fwData::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = ::fwData::StructureTraits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    ::fwData::StructureTraits::sptr liver = ::fwData::StructureTraits::New();
    liver->setType("Liver");
    liver->setClass(::fwData::StructureTraits::ORGAN);
    liver->setColor(::fwData::Color::New(204.0f/255.0f, 51.0f/255.0f, 51.0f/255.0f, 1.0));
    ::fwData::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    ::fwData::StructureTraits::sptr liverTumor = ::fwData::StructureTraits::New();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(::fwData::StructureTraits::LESION);
    liverTumor->setColor(::fwData::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    ::fwData::StructureTraits::sptr tumor = ::fwData::StructureTraits::New();
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

    ::fwData::StructureTraits::sptr colon = ::fwData::StructureTraits::New();
    colon->setType("Colon");
    colon->setClass(::fwData::StructureTraits::ORGAN);
    colon->setColor(::fwData::Color::New(1.0f, 179.0f/255.0f, 128.0f/255.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer colonCat(1);
    colonCat[0] = ::fwData::StructureTraits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    ::fwData::StructureTraits::sptr caecum = ::fwData::StructureTraits::New();
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

::fwData::StructureTraits::sptr Object::createStructureTraits()
{
    ::fwData::StructureTraits::sptr pStructureTraits = ::fwData::StructureTraits::New();

    const std::string type                                      = "Liver";
    const ::fwData::StructureTraits::StructureClass structClass = ::fwData::StructureTraits::ORGAN;
    const ::fwData::StructureTraits::ROIExpression nativeExp    = "inter(world(type(Skin)),not(class(Organ)))";
    const ::fwData::StructureTraits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    ::fwData::Color::sptr color = ::fwData::Color::New();
    color->setRGBA(rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f);
    pStructureTraits->setColor(color);

    ::fwData::StructureTraits::CategoryContainer categories(2);
    categories[0] = ::fwData::StructureTraits::ABDOMEN;
    categories[1] = ::fwData::StructureTraits::BODY;
    pStructureTraits->setCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr Object::createROITraits()
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    ::fwData::ROITraits::sptr roi = ::fwData::ROITraits::New();
    std::stringstream ss;
    ss << "ID_" << rand()%100;
    roi->setIdentifier(ss.str());

    roi->setEvaluatedExp("inter(skin),not(liver)");
    ::fwData::StructureTraits::sptr structure = Object::createStructureTraits();
    roi->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = Object::createNode();
    roi->setMaskOpNode(maskNode);

    (*composite)["ROITraits"]       = roi;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"]      = maskNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr Object::createReconstructionTraits()
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    ::fwData::ReconstructionTraits::sptr rec = ::fwData::ReconstructionTraits::New();

    std::stringstream ss;
    ss << "ID_" << rand()%100;
    rec->setIdentifier(ss.str());

    ::fwData::StructureTraits::sptr structure = Object::createStructureTraits();
    rec->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = Object::createNode();
    rec->setMaskOpNode(maskNode);

    ::fwData::Node::sptr meshNode = Object::createNode();
    rec->setMeshOpNode(meshNode);

    (*composite)["ReconstructionTraits"] = rec;
    (*composite)["StructureTraits"]      = structure;
    (*composite)["MaskOpNode"]           = maskNode;
    (*composite)["MeshOpNode"]           = meshNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr Object::createNode()
{
    ::fwData::Node::sptr node = ::fwData::Node::New();

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

::fwData::Port::sptr Object::createPort()
{
    ::fwData::Port::sptr port = ::fwData::Port::New();

    std::stringstream ssId;
    ssId << "ID_" << rand()%100;
    port->setIdentifier(ssId.str());

    std::stringstream ssType;
    ssType << "Type_" << rand()%100;
    port->setType(ssType.str());

    return port;
}

//------------------------------------------------------------------------------

::fwData::Color::sptr Object::randomizeColor()
{
    ::fwData::Color::sptr col = ::fwData::Color::New();
    col->setRGBA(rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f);
    return col;
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr Object::createTFColor(const unsigned char nbPoints,
                                                       const double window,
                                                       const double level)
{
    ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();

    tf->setBackgroundColor( ::fwData::TransferFunction::TFColor( rand()%100 / 100.f, rand()%100 / 100.f,
                                                                 rand()%100 / 100.f, rand()%100 / 100.f) );
    tf->setInterpolationMode( ::fwData::TransferFunction::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel(level);
    tf->setName( "TFColor" );
    tf->setWindow(window );

    for (unsigned char nb = 0; nb < nbPoints; ++nb)
    {
        double value = rand()%100 - level;
        tf->addTFColor( value,
                        ::fwData::TransferFunction::TFColor( rand()%100 / 100.f, rand()%100 / 100.f, rand()%100 / 100.f,
                                                             rand()%100 / 100.f ));
    }

    ::fwData::String::sptr myString = ::fwData::String::New("fieldStringValue");
    tf->setField( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr Object::createTFColor()
{

    ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();

    tf->setBackgroundColor( ::fwData::TransferFunction::TFColor( 1.0f, 0.3f, 0.6f, 0.1f) );
    tf->setInterpolationMode( ::fwData::TransferFunction::NEAREST );
    tf->setIsClamped( false );
    tf->setLevel( 900.6 );
    tf->setName( "TFColor" );
    tf->setWindow( -200.02 );

    tf->addTFColor( -40.33, ::fwData::TransferFunction::TFColor( 0.9f, 0.2f, 0.3f, 0.4f) );
    tf->addTFColor( 3,      ::fwData::TransferFunction::TFColor( 0.1f, 0.2f, 0.9f, 0.4f) ); // Invert point 3 <=> -0.2,
                                                                                            // for tests
    tf->addTFColor( -0.2,   ::fwData::TransferFunction::TFColor( 0.1f, 0.9f, 0.3f, 0.4f) );
    tf->addTFColor( 150,    ::fwData::TransferFunction::TFColor( 0.1f, 0.2f, 0.3f, 0.9f) );

    ::fwData::String::sptr myString = ::fwData::String::New("fieldStringValue");
    tf->setField( "fieldStringKey", myString );

    return tf;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    ::fwData::Color::sptr ambient = ::fwData::Color::New();
    ambient->setRGBA(0.5f, 0.5f, 0.5f, 0.5f);

    ::fwData::Color::sptr diffuse = ::fwData::Color::New();
    diffuse->setRGBA(0.8f, 0.2f, 0.5f, 0.4f);

    ::fwData::Material::sptr material = ::fwData::Material::New();

    material->setAmbient(::fwData::Object::copy( ambient ));
    material->setDiffuse(::fwData::Object::copy( diffuse ));

    return material;
}

//------------------------------------------------------------------------------

::fwData::ProcessObject::sptr Object::createProcessObject()
{
    const std::string IMAGEID1 = "myImage1";
    const std::string IMAGEID2 = "myImage2";
    const std::string FIELDID1 = "myField1";
    const std::string FIELDID2 = "myField2";
    ::fwData::Image::sptr image1   = ::fwData::Image::New();
    ::fwData::Image::sptr image2   = ::fwData::Image::New();
    ::fwData::Integer::sptr field1 = ::fwData::Integer::New(3);
    ::fwData::Integer::sptr field2 = ::fwData::Integer::New(8);

    // process
    ::fwData::ProcessObject::sptr po = ::fwData::ProcessObject::New();
    po->setInputValue(IMAGEID1, image1);
    po->setInputValue(FIELDID1, field1);
    po->setInputValue(FIELDID2, field2);
    po->setOutputValue(IMAGEID2, image2);
    return po;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr Object::generatePoint()
{
    std::array< double, 3> coord = {static_cast<double>(rand()%300),
                                    static_cast<double>(rand()%300),
                                    static_cast<double>(rand()%300)};
    ::fwData::Point::sptr point = ::fwData::Point::New();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

::fwData::Plane::sptr Object::generatePlane()
{
    ::fwData::Plane::sptr plane = ::fwData::Plane::New();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((rand()%1) ? true : false));
    return plane;
}

//------------------------------------------------------------------------------

::fwData::Resection::sptr Object::generateResection()
{

    ::fwData::Resection::sptr resection = ::fwData::Resection::New();

    resection->setName("Resection1");
    resection->setIsSafePart(((rand()%1) ? true : false));
    resection->setIsValid(((rand()%1) ? true : false));
    resection->setIsVisible(((rand()%1) ? true : false));
    ::fwData::Reconstruction::sptr recInput = ::fwData::Reconstruction::New();

    ::fwTest::generator::SeriesDB::generateReconstruction(recInput);
    ::fwData::Resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->setInputs(inputs);

    ::fwData::Reconstruction::sptr recOutput = ::fwData::Reconstruction::New();
    ::fwTest::generator::SeriesDB::generateReconstruction(recOutput);
    ::fwData::Resection::ResectionOutputs outputs;
    outputs.push_back(recOutput);
    resection->setOutputs(outputs);

    ::fwData::PlaneList::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    ::fwData::PlaneList::sptr planeList = ::fwData::PlaneList::New();
    planeList->setPlanes(planes);

    return resection;
}

//------------------------------------------------------------------------------

::fwData::ResectionDB::sptr Object::generateResectionDB()
{
    ::fwData::ResectionDB::sptr resectionDB = ::fwData::ResectionDB::New();
    resectionDB->addResection(generateResection());
    return resectionDB;
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
