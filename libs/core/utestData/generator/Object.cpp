/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "utestData/generator/Object.hpp"

#include "utestData/generator/Image.hpp"
#include "utestData/generator/Mesh.hpp"
#include "utestData/generator/SeriesSet.hpp"

#include <core/tools/random/Generator.hpp>

#include <data/Integer.hpp>
#include <data/Plane.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/String.hpp>

#include <cmath>
#include <filesystem>

namespace sight::utestData::generator
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

data::StructureTraitsDictionary::sptr Object::createStructureTraitsDictionary()
{
    data::StructureTraitsDictionary::sptr structDico = data::StructureTraitsDictionary::New();
    data::StructureTraits::sptr skin                 = data::StructureTraits::New();
    std::string skinType                             = "Skin";
    skin->setType(skinType);
    skin->setClass(data::StructureTraits::ENVIRONMENT);
    auto skinColor = data::Color::New(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skinColor);
    data::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = data::StructureTraits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    data::StructureTraits::sptr liver = data::StructureTraits::New();
    liver->setType("Liver");
    liver->setClass(data::StructureTraits::ORGAN);
    liver->setColor(data::Color::New(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0));
    data::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = data::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    data::StructureTraits::sptr liverTumor = data::StructureTraits::New();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::StructureTraits::LESION);
    liverTumor->setColor(data::Color::New(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    data::StructureTraits::sptr tumor = data::StructureTraits::New();
    tumor->setType("Tumor");
    tumor->setClass(data::StructureTraits::LESION);
    tumor->setColor(data::Color::New(0.0F, 0.0F, 1.0F, 1.0F));
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
    colon->setColor(data::Color::New(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer colonCat(1);
    colonCat[0] = data::StructureTraits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    data::StructureTraits::sptr caecum = data::StructureTraits::New();
    caecum->setType("Caecum");
    caecum->setClass(data::StructureTraits::FUNCTIONAL);
    caecum->setColor(data::Color::New(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
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
    color->setRGBA(
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F
    );
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
    ss << "ID_" << safeRand() % 100;
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
    ss << "ID_" << safeRand() % 100;
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

    int nbInputs  = safeRand() % 5;
    int nbOutputs = safeRand() % 5;

    for(int i = 0 ; i < nbInputs ; ++i)
    {
        node->addInputPort(Object::createPort());
    }

    for(int i = 0 ; i < nbOutputs ; ++i)
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
    ssId << "ID_" << safeRand() % 100;
    port->setIdentifier(ssId.str());

    std::stringstream ssType;
    ssType << "Type_" << safeRand() % 100;
    port->setType(ssType.str());

    return port;
}

//------------------------------------------------------------------------------

data::Color::sptr Object::randomizeColor()
{
    data::Color::sptr col = data::Color::New();
    col->setRGBA(
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F,
        static_cast<float>(safeRand() % 100) / 100.F
    );
    return col;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr Object::createTFColor(
    const unsigned char nbPoints,
    const double window,
    const double level
)
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setName("color_t");
    tf->setBackgroundColor(
        data::TransferFunction::color_t(
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F
        )
    );

    auto tfData = data::TransferFunctionPiece::New();

    auto& pieces = tf->pieces();
    pieces.push_back(tfData);

    tfData->setLevel(level);
    tfData->setWindow(window);
    tfData->setInterpolationMode(data::TransferFunction::InterpolationMode::NEAREST);
    tfData->setClamped(false);

    for(unsigned char nb = 0 ; nb < nbPoints ; ++nb)
    {
        double value = safeRand() % 100 - level;
        (*tfData)[value] = data::TransferFunction::color_t(
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F,
            static_cast<float>(safeRand() % 100) / 100.F
        );
    }

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr Object::createTFColor()
{
    data::TransferFunction::sptr tf = data::TransferFunction::New();

    tf->setBackgroundColor(data::TransferFunction::color_t(1.0F, 0.3F, 0.6F, 0.1F));
    tf->setName("color_t");

    auto tfData = data::TransferFunctionPiece::New();

    auto& pieces = tf->pieces();
    pieces.push_back(tfData);

    tfData->setInterpolationMode(data::TransferFunction::InterpolationMode::NEAREST);
    tfData->setClamped(false);
    tfData->setLevel(900.6);
    tfData->setWindow(-200.02);

    tfData->insert({-40.33, data::TransferFunction::color_t(0.9F, 0.2F, 0.3F, 0.4F)});
    tfData->insert({3, data::TransferFunction::color_t(0.1F, 0.2F, 0.9F, 0.4F)}); // Invert point 3 <=> -0.2,
    // for tests
    tfData->insert({-0.2, data::TransferFunction::color_t(0.1F, 0.9F, 0.3F, 0.4F)});
    tfData->insert({150, data::TransferFunction::color_t(0.1F, 0.2F, 0.3F, 0.9F)});

    data::String::sptr myString = data::String::New("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::Material::sptr Object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::Color::sptr ambient = data::Color::New();
    ambient->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::Color::sptr diffuse = data::Color::New();
    diffuse->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    data::Material::sptr material = data::Material::New();

    material->setAmbient(data::Object::copy(ambient));
    material->setDiffuse(data::Object::copy(diffuse));

    return material;
}

//------------------------------------------------------------------------------

data::Point::sptr Object::generatePoint()
{
    std::array<double, 3> coord = {static_cast<double>(safeRand() % 300),
                                   static_cast<double>(safeRand() % 300),
                                   static_cast<double>(safeRand() % 300)
    };
    data::Point::sptr point = data::Point::New();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

data::Plane::sptr Object::generatePlane()
{
    data::Plane::sptr plane = data::Plane::New();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((safeRand() % 2) != 0));
    return plane;
}

//------------------------------------------------------------------------------

data::Resection::sptr Object::generateResection()
{
    data::Resection::sptr resection = data::Resection::New();

    resection->setName("Resection1");
    resection->setIsSafePart(((safeRand() % 2) != 0));
    resection->setIsValid(((safeRand() % 2) != 0));
    resection->setIsVisible(((safeRand() % 2) != 0));
    data::Reconstruction::sptr recInput = data::Reconstruction::New();

    utestData::generator::SeriesSet::generateReconstruction(recInput);
    data::Resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->setInputs(inputs);

    data::Reconstruction::sptr recOutput = data::Reconstruction::New();
    utestData::generator::SeriesSet::generateReconstruction(recOutput);
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

} // namespace sight::utestData::generator
