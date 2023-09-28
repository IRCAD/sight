/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/tools/random/generator.hpp>

#include <data/Integer.hpp>
#include <data/Plane.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/String.hpp>

#include <cmath>
#include <filesystem>

namespace sight::utestData::generator
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

data::StructureTraitsDictionary::sptr Object::createStructureTraitsDictionary()
{
    data::StructureTraitsDictionary::sptr structDico = std::make_shared<data::StructureTraitsDictionary>();
    data::StructureTraits::sptr skin                 = std::make_shared<data::StructureTraits>();
    std::string skinType                             = "Skin";
    skin->setType(skinType);
    skin->setClass(data::StructureTraits::ENVIRONMENT);
    auto skinColor = std::make_shared<data::Color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skinColor);
    data::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = data::StructureTraits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    data::StructureTraits::sptr liver = std::make_shared<data::StructureTraits>();
    liver->setType("Liver");
    liver->setClass(data::StructureTraits::ORGAN);
    liver->setColor(std::make_shared<data::Color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = data::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    data::StructureTraits::sptr liverTumor = std::make_shared<data::StructureTraits>();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::StructureTraits::LESION);
    liverTumor->setColor(std::make_shared<data::Color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    data::StructureTraits::sptr tumor = std::make_shared<data::StructureTraits>();
    tumor->setType("Tumor");
    tumor->setClass(data::StructureTraits::LESION);
    tumor->setColor(std::make_shared<data::Color>(0.0F, 0.0F, 1.0F, 1.0F));
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

    data::StructureTraits::sptr colon = std::make_shared<data::StructureTraits>();
    colon->setType("Colon");
    colon->setClass(data::StructureTraits::ORGAN);
    colon->setColor(std::make_shared<data::Color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer colonCat(1);
    colonCat[0] = data::StructureTraits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    data::StructureTraits::sptr caecum = std::make_shared<data::StructureTraits>();
    caecum->setType("Caecum");
    caecum->setClass(data::StructureTraits::FUNCTIONAL);
    caecum->setColor(std::make_shared<data::Color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
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
    data::StructureTraits::sptr pStructureTraits = std::make_shared<data::StructureTraits>();

    const std::string type                                  = "Liver";
    const data::StructureTraits::StructureClass structClass = data::StructureTraits::ORGAN;
    const data::StructureTraits::ROIExpression nativeExp    = "inter(world(type(Skin)),not(class(Organ)))";
    const data::StructureTraits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    data::Color::sptr color = std::make_shared<data::Color>();
    color->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    pStructureTraits->setColor(color);

    data::StructureTraits::CategoryContainer categories(2);
    categories[0] = data::StructureTraits::ABDOMEN;
    categories[1] = data::StructureTraits::BODY;
    pStructureTraits->setCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

data::Color::sptr Object::randomizeColor()
{
    data::Color::sptr col = std::make_shared<data::Color>();
    col->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
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
    data::TransferFunction::sptr tf = std::make_shared<data::TransferFunction>();

    tf->setName("color_t");
    tf->setBackgroundColor(
        data::TransferFunction::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        )
    );

    auto tfData = std::make_shared<data::TransferFunctionPiece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tfData);

    tfData->setLevel(level);
    tfData->setWindow(window);
    tfData->setInterpolationMode(data::TransferFunction::InterpolationMode::NEAREST);
    tfData->setClamped(false);

    for(unsigned char nb = 0 ; nb < nbPoints ; ++nb)
    {
        double value = safe_rand() % 100 - level;
        (*tfData)[value] = data::TransferFunction::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        );
    }

    data::String::sptr myString = std::make_shared<data::String>("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr Object::createTFColor()
{
    data::TransferFunction::sptr tf = std::make_shared<data::TransferFunction>();

    tf->setBackgroundColor(data::TransferFunction::color_t(1.0F, 0.3F, 0.6F, 0.1F));
    tf->setName("color_t");

    auto tfData = std::make_shared<data::TransferFunctionPiece>();

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

    data::String::sptr myString = std::make_shared<data::String>("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::Material::sptr Object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::Color::sptr ambient = std::make_shared<data::Color>();
    ambient->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::Color::sptr diffuse = std::make_shared<data::Color>();
    diffuse->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    data::Material::sptr material = std::make_shared<data::Material>();

    material->setAmbient(data::Object::copy(ambient));
    material->setDiffuse(data::Object::copy(diffuse));

    return material;
}

//------------------------------------------------------------------------------

data::Point::sptr Object::generatePoint()
{
    std::array<double, 3> coord = {static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300)
    };
    data::Point::sptr point = std::make_shared<data::Point>();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

data::Plane::sptr Object::generatePlane()
{
    data::Plane::sptr plane = std::make_shared<data::Plane>();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((safe_rand() % 2) != 0));
    return plane;
}

//------------------------------------------------------------------------------

data::Resection::sptr Object::generateResection()
{
    data::Resection::sptr resection = std::make_shared<data::Resection>();

    resection->setName("Resection1");
    resection->setIsSafePart(((safe_rand() % 2) != 0));
    resection->setIsValid(((safe_rand() % 2) != 0));
    resection->setIsVisible(((safe_rand() % 2) != 0));
    data::Reconstruction::sptr recInput = std::make_shared<data::Reconstruction>();

    utestData::generator::SeriesSet::generateReconstruction(recInput);
    data::Resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->setInputs(inputs);

    data::Reconstruction::sptr recOutput = std::make_shared<data::Reconstruction>();
    utestData::generator::SeriesSet::generateReconstruction(recOutput);
    data::Resection::ResectionOutputs outputs;
    outputs.push_back(recOutput);
    resection->setOutputs(outputs);

    data::PlaneList::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    data::PlaneList::sptr planeList = std::make_shared<data::PlaneList>();
    planeList->setPlanes(planes);

    return resection;
}

//------------------------------------------------------------------------------

data::ResectionDB::sptr Object::generateResectionDB()
{
    data::ResectionDB::sptr resectionDB = std::make_shared<data::ResectionDB>();
    resectionDB->addResection(generateResection());
    return resectionDB;
}

//------------------------------------------------------------------------------

} // namespace sight::utestData::generator
