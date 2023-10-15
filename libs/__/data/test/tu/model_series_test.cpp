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

#include "model_series_test.hpp"

#include <core/type.hpp>

#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <utest_data/generator/mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::model_series_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void model_series_test::setUp()
{
    // Set up context before running a test.
    m_series = std::make_shared<data::model_series>();
}

//------------------------------------------------------------------------------

void model_series_test::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void model_series_test::modelTest()
{
    CPPUNIT_ASSERT(m_series);

    data::reconstruction::sptr rec1 = std::make_shared<data::reconstruction>();
    CPPUNIT_ASSERT(rec1);
    data::reconstruction::sptr rec2 = std::make_shared<data::reconstruction>();
    CPPUNIT_ASSERT(rec2);

    data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh1);
    utest_data::generator::mesh::generateQuadMesh(mesh1);

    data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    utest_data::generator::mesh::generateQuadMesh(mesh2);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);

    model_series::ReconstructionVectorType recs;
    recs.push_back(rec1);
    recs.push_back(rec2);

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(2, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);
}

//------------------------------------------------------------------------------

void model_series_test::deep_copyTest()
{
    CPPUNIT_ASSERT(m_series);

    auto rec1  = std::make_shared<data::reconstruction>();
    auto mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generateQuadMesh(mesh1);

    rec1->setMesh(mesh1);
    model_series::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    auto secondSeries = std::make_shared<data::model_series>();

    CPPUNIT_ASSERT(*m_series != *secondSeries);

    secondSeries->deep_copy(m_series);

    CPPUNIT_ASSERT(*m_series == *secondSeries);

    CPPUNIT_ASSERT_EQUAL(1, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(1, (int) secondSeries->getReconstructionDB().size());
    CPPUNIT_ASSERT(m_series->getReconstructionDB()[0] != secondSeries->getReconstructionDB()[0]);
}

//------------------------------------------------------------------------------

void model_series_test::shallow_copyTest()
{
    CPPUNIT_ASSERT(m_series);

    auto rec1  = std::make_shared<data::reconstruction>();
    auto mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generateQuadMesh(mesh1);
    rec1->setMesh(mesh1);
    model_series::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    auto secondSeries = std::make_shared<data::model_series>();

    CPPUNIT_ASSERT(*m_series != *secondSeries);

    secondSeries->shallow_copy(m_series);

    CPPUNIT_ASSERT(*m_series == *secondSeries);

    CPPUNIT_ASSERT(m_series->getReconstructionDB()[0] == secondSeries->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(m_series->getReconstructionDB()[0], secondSeries->getReconstructionDB()[0]);

    CPPUNIT_ASSERT_EQUAL(recs[0], m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(recs[0], secondSeries->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(1, (int) m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(1, (int) secondSeries->getReconstructionDB().size());
}

//------------------------------------------------------------------------------

void model_series_test::equalityTest()
{
    auto series1 = std::make_shared<data::model_series>();
    auto series2 = std::make_shared<data::model_series>();

    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    series1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Series must be different when using " #op " on the first one", \
        *series1 != *series2 && !(*series1 == *series2) \
    ); \
    series2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Series must be equal when using " #op " on both", \
        *series1 == *series2 && !(*series1 != *series2) \
    );

    TEST(setReconstructionDB({std::make_shared<data::reconstruction>()}));
    TEST(setDicomReference(std::make_shared<data::dicom_series>()));

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
