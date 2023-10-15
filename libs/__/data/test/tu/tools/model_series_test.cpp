/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "model_series_test.hpp"

#include <data/color.hpp>
#include <data/material.hpp>
#include <data/model_series.hpp>
#include <data/tools/model_series.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::model_series_test);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

void model_series_test::setUp()
{
}

//------------------------------------------------------------------------------

void model_series_test::tearDown()
{
}

//------------------------------------------------------------------------------

void model_series_test::addReconstruction()
{
    data::model_series::sptr modelSeries = std::make_shared<data::model_series>();
    data::reconstruction::sptr rec1      = std::make_shared<data::reconstruction>();

    data::tools::model_series::addReconstruction(modelSeries, rec1);

    const data::model_series::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);

    data::reconstruction::sptr rec2 = std::make_shared<data::reconstruction>();

    data::tools::model_series::addReconstruction(modelSeries, rec2);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);

    data::reconstruction::sptr rec3 = std::make_shared<data::reconstruction>();
    data::tools::model_series::addReconstruction(modelSeries, rec3);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, recDB[2]);

    CPPUNIT_ASSERT_THROW(data::tools::model_series::addReconstruction(modelSeries, rec1), core::exception);

    data::reconstruction::sptr nullRec;
    CPPUNIT_ASSERT_THROW(data::tools::model_series::addReconstruction(modelSeries, nullRec), core::exception);
}

//------------------------------------------------------------------------------

void model_series_test::createReconstructionFromMesh()
{
    data::mesh::sptr mesh   = std::make_shared<data::mesh>();
    data::color::sptr color = std::make_shared<data::color>(1.0F, 0.3F, 0.7F, 0.5F);

    {
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        auto rec = data::tools::model_series::createReconstructionFromMesh(
            mesh,
            organName,
            structureType,
            color,
            data::material::SURFACE
        );

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::material::SURFACE);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        auto rec = data::tools::model_series::createReconstructionFromMesh(
            mesh,
            organName,
            structureType,
            color,
            data::material::WIREFRAME,
            false
        );

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::material::WIREFRAME);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }
}

//------------------------------------------------------------------------------

void model_series_test::addMesh()
{
    data::model_series::sptr modelSeries                      = std::make_shared<data::model_series>();
    const data::model_series::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();

    {
        data::mesh::sptr mesh           = std::make_shared<data::mesh>();
        data::color::sptr color         = std::make_shared<data::color>(1.0F, 0.3F, 0.7F, 0.5F);
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        data::tools::model_series::addMesh(
            modelSeries,
            mesh,
            organName,
            structureType,
            color,
            data::material::SURFACE
        );

        auto rec = recDB[0];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::material::SURFACE);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        data::mesh::sptr mesh           = std::make_shared<data::mesh>();
        data::color::sptr color         = std::make_shared<data::color>(0.1F, 0.3F, 0.7F, 1.5F);
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        data::tools::model_series::addMesh(
            modelSeries,
            mesh,
            organName,
            structureType,
            color,
            data::material::WIREFRAME,
            false
        );
        auto rec = recDB[1];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(), data::material::WIREFRAME);

        auto ambient = rec->getMaterial()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 0.1F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 1.5F);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
