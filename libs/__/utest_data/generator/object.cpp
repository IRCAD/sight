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

#include "utest_data/generator/object.hpp"

#include "utest_data/generator/image.hpp"
#include "utest_data/generator/mesh.hpp"
#include "utest_data/generator/series_set.hpp"

#include <core/tools/random/generator.hpp>

#include <data/integer.hpp>
#include <data/plane.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/string.hpp>

#include <cmath>
#include <filesystem>

namespace sight::utest_data::generator
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

data::structure_traits_dictionary::sptr object::createStructureTraitsDictionary()
{
    data::structure_traits_dictionary::sptr structDico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                  = std::make_shared<data::structure_traits>();
    std::string skinType                               = "Skin";
    skin->setType(skinType);
    skin->setClass(data::structure_traits::ENVIRONMENT);
    auto skinColor = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skinColor);
    data::structure_traits::CategoryContainer skinCat(1);
    skinCat[0] = data::structure_traits::BODY;
    skin->setCategories(skinCat);
    structDico->addStructure(skin);

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->setType("Liver");
    liver->setClass(data::structure_traits::ORGAN);
    liver->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liverCat(1);
    liverCat[0] = data::structure_traits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    structDico->addStructure(liver);

    data::structure_traits::sptr liverTumor = std::make_shared<data::structure_traits>();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::structure_traits::LESION);
    liverTumor->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::structure_traits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    structDico->addStructure(liverTumor);

    data::structure_traits::sptr tumor = std::make_shared<data::structure_traits>();
    tumor->setType("Tumor");
    tumor->setClass(data::structure_traits::LESION);
    tumor->setColor(std::make_shared<data::color>(0.0F, 0.0F, 1.0F, 1.0F));
    data::structure_traits::CategoryContainer tumorCat(8);
    tumorCat[0] = data::structure_traits::BODY;
    tumorCat[1] = data::structure_traits::HEAD;
    tumorCat[2] = data::structure_traits::NECK;
    tumorCat[3] = data::structure_traits::THORAX;
    tumorCat[4] = data::structure_traits::ABDOMEN;
    tumorCat[5] = data::structure_traits::PELVIS;
    tumorCat[6] = data::structure_traits::ARM;
    tumorCat[7] = data::structure_traits::LEG;
    tumor->setCategories(tumorCat);

    data::structure_traits::sptr colon = std::make_shared<data::structure_traits>();
    colon->setType("Colon");
    colon->setClass(data::structure_traits::ORGAN);
    colon->setColor(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer colonCat(1);
    colonCat[0] = data::structure_traits::ABDOMEN;
    colon->setCategories(colonCat);
    structDico->addStructure(colon);

    data::structure_traits::sptr caecum = std::make_shared<data::structure_traits>();
    caecum->setType("Caecum");
    caecum->setClass(data::structure_traits::FUNCTIONAL);
    caecum->setColor(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer caecumCat(1);
    caecumCat[0] = data::structure_traits::ABDOMEN;
    caecum->setCategories(caecumCat);
    caecum->setAttachmentType("Colon");
    structDico->addStructure(caecum);

    return structDico;
}

//------------------------------------------------------------------------------

data::structure_traits::sptr object::createStructureTraits()
{
    data::structure_traits::sptr pStructureTraits = std::make_shared<data::structure_traits>();

    const std::string type                                   = "Liver";
    const data::structure_traits::StructureClass structClass = data::structure_traits::ORGAN;
    const data::structure_traits::ROIExpression nativeExp    = "inter(world(type(Skin)),not(class(Organ)))";
    const data::structure_traits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    data::color::sptr color = std::make_shared<data::color>();
    color->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    pStructureTraits->setColor(color);

    data::structure_traits::CategoryContainer categories(2);
    categories[0] = data::structure_traits::ABDOMEN;
    categories[1] = data::structure_traits::BODY;
    pStructureTraits->setCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

data::color::sptr object::randomizeColor()
{
    data::color::sptr col = std::make_shared<data::color>();
    col->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    return col;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr object::createTFColor(
    const unsigned char nbPoints,
    const double window,
    const double level
)
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->setName("color_t");
    tf->setBackgroundColor(
        data::transfer_function::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        )
    );

    auto tfData = std::make_shared<data::transfer_function_piece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tfData);

    tfData->setLevel(level);
    tfData->setWindow(window);
    tfData->setInterpolationMode(data::transfer_function::InterpolationMode::NEAREST);
    tfData->setClamped(false);

    for(unsigned char nb = 0 ; nb < nbPoints ; ++nb)
    {
        double value = safe_rand() % 100 - level;
        (*tfData)[value] = data::transfer_function::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        );
    }

    data::string::sptr myString = std::make_shared<data::string>("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr object::createTFColor()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->setBackgroundColor(data::transfer_function::color_t(1.0F, 0.3F, 0.6F, 0.1F));
    tf->setName("color_t");

    auto tfData = std::make_shared<data::transfer_function_piece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tfData);

    tfData->setInterpolationMode(data::transfer_function::InterpolationMode::NEAREST);
    tfData->setClamped(false);
    tfData->setLevel(900.6);
    tfData->setWindow(-200.02);

    tfData->insert({-40.33, data::transfer_function::color_t(0.9F, 0.2F, 0.3F, 0.4F)});
    tfData->insert({3, data::transfer_function::color_t(0.1F, 0.2F, 0.9F, 0.4F)}); // Invert point 3 <=> -0.2,
    // for tests
    tfData->insert({-0.2, data::transfer_function::color_t(0.1F, 0.9F, 0.3F, 0.4F)});
    tfData->insert({150, data::transfer_function::color_t(0.1F, 0.2F, 0.3F, 0.9F)});

    data::string::sptr myString = std::make_shared<data::string>("fieldStringValue");
    tf->setField("fieldStringKey", myString);

    return tf;
}

//------------------------------------------------------------------------------

data::material::sptr object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::color::sptr ambient = std::make_shared<data::color>();
    ambient->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::color::sptr diffuse = std::make_shared<data::color>();
    diffuse->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    data::material::sptr material = std::make_shared<data::material>();

    material->setAmbient(data::object::copy(ambient));
    material->setDiffuse(data::object::copy(diffuse));

    return material;
}

//------------------------------------------------------------------------------

data::point::sptr object::generatePoint()
{
    std::array<double, 3> coord = {static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300)
    };
    data::point::sptr point = std::make_shared<data::point>();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

data::plane::sptr object::generatePlane()
{
    data::plane::sptr plane = std::make_shared<data::plane>();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((safe_rand() % 2) != 0));
    return plane;
}

//------------------------------------------------------------------------------

data::resection::sptr object::generateResection()
{
    data::resection::sptr resection = std::make_shared<data::resection>();

    resection->setName("Resection1");
    resection->setIsSafePart(((safe_rand() % 2) != 0));
    resection->setIsValid(((safe_rand() % 2) != 0));
    resection->setIsVisible(((safe_rand() % 2) != 0));
    data::reconstruction::sptr recInput = std::make_shared<data::reconstruction>();

    utest_data::generator::series_set::generateReconstruction(recInput);
    data::resection::ResectionInputs inputs;
    inputs.push_back(recInput);
    resection->SetInputs(inputs);

    data::reconstruction::sptr recOutput = std::make_shared<data::reconstruction>();
    utest_data::generator::series_set::generateReconstruction(recOutput);
    data::resection::ResectionOutputs outputs;
    outputs.push_back(recOutput);
    resection->setOutputs(outputs);

    data::plane_list::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    data::plane_list::sptr planeList = std::make_shared<data::plane_list>();
    planeList->setPlanes(planes);

    return resection;
}

//------------------------------------------------------------------------------

data::resection_db::sptr object::generateResectionDB()
{
    data::resection_db::sptr resectionDB = std::make_shared<data::resection_db>();
    resectionDB->addResection(generateResection());
    return resectionDB;
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
