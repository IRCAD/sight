/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include "ModelSeriesTest.hpp"

#include <data/Color.hpp>
#include <data/Material.hpp>
#include <data/ModelSeries.hpp>
#include <data/tools/ModelSeries.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::ModelSeriesTest);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

void ModelSeriesTest::setUp()
{
}

//------------------------------------------------------------------------------

void ModelSeriesTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ModelSeriesTest::addReconstruction()
{
    data::ModelSeries::sptr modelSeries = data::ModelSeries::New();
    data::Reconstruction::sptr rec1     = data::Reconstruction::New();

    data::tools::ModelSeries::addReconstruction(modelSeries, rec1);

    const data::ModelSeries::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);

    data::Reconstruction::sptr rec2 = data::Reconstruction::New();

    data::tools::ModelSeries::addReconstruction(modelSeries, rec2);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);

    data::Reconstruction::sptr rec3 = data::Reconstruction::New();
    data::tools::ModelSeries::addReconstruction(modelSeries, rec3);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, recDB[2]);

    CPPUNIT_ASSERT_THROW(data::tools::ModelSeries::addReconstruction(modelSeries, rec1), core::Exception);

    data::Reconstruction::sptr nullRec;
    CPPUNIT_ASSERT_THROW(data::tools::ModelSeries::addReconstruction(modelSeries, nullRec), core::Exception);
}

//------------------------------------------------------------------------------

void ModelSeriesTest::createReconstructionFromMesh()
{
    data::Mesh::sptr mesh   = data::Mesh::New();
    data::Color::sptr color = data::Color::New(1.0F, 0.3F, 0.7F, 0.5F);

    {
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        auto rec = data::tools::ModelSeries::createReconstructionFromMesh(
            mesh,
            organName,
            structureType,
            color,
            data::Material::SURFACE
        );

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::Material::SURFACE);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        auto rec = data::tools::ModelSeries::createReconstructionFromMesh(
            mesh,
            organName,
            structureType,
            color,
            data::Material::WIREFRAME,
            false
        );

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::Material::WIREFRAME);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }
}

//------------------------------------------------------------------------------

void ModelSeriesTest::addMesh()
{
    data::ModelSeries::sptr modelSeries                      = data::ModelSeries::New();
    const data::ModelSeries::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();

    {
        data::Mesh::sptr mesh           = data::Mesh::New();
        data::Color::sptr color         = data::Color::New(1.0F, 0.3F, 0.7F, 0.5F);
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        data::tools::ModelSeries::addMesh(
            modelSeries,
            mesh,
            organName,
            structureType,
            color,
            data::Material::SURFACE
        );

        auto rec = recDB[0];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::Material::SURFACE);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        data::Mesh::sptr mesh           = data::Mesh::New();
        data::Color::sptr color         = data::Color::New(0.1F, 0.3F, 0.7F, 1.5F);
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        data::tools::ModelSeries::addMesh(
            modelSeries,
            mesh,
            organName,
            structureType,
            color,
            data::Material::WIREFRAME,
            false
        );
        auto rec = recDB[1];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::Material::WIREFRAME);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 0.1F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 1.5F);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
