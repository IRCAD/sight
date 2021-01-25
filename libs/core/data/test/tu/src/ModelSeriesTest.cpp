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

#include "ModelSeriesTest.hpp"

#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <core/tools/Type.hpp>

#include <utestData/generator/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::ut::ModelSeriesTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void ModelSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = data::ModelSeries::New();
}

//------------------------------------------------------------------------------

void ModelSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void ModelSeriesTest::modelTest()
{
    CPPUNIT_ASSERT(m_series);

    data::Reconstruction::sptr rec1 = data::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    data::Reconstruction::sptr rec2 = data::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);

    data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    utestData::generator::Mesh::generateQuadMesh(mesh1);

    data::Mesh::sptr mesh2 = data::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    utestData::generator::Mesh::generateQuadMesh(mesh2);

    rec1->setMesh(mesh1);
    rec2->setMesh(mesh2);

    ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    recs.push_back(rec2);

    m_series->setReconstructionDB(recs);
    CPPUNIT_ASSERT_EQUAL(2, (int)m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(rec1, m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, m_series->getReconstructionDB()[1]);
}

//------------------------------------------------------------------------------

void ModelSeriesTest::deepCopyTest()
{
    CPPUNIT_ASSERT(m_series);

    data::Reconstruction::sptr rec1 = data::Reconstruction::New();
    data::Mesh::sptr mesh1          = data::Mesh::New();
    utestData::generator::Mesh::generateQuadMesh(mesh1);

    rec1->setMesh(mesh1);
    ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    data::ModelSeries::sptr secondSeries = data::ModelSeries::New();

    secondSeries->deepCopy(m_series);

    CPPUNIT_ASSERT_EQUAL(1, (int)m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(1, (int)secondSeries->getReconstructionDB().size());
    CPPUNIT_ASSERT(m_series->getReconstructionDB()[0] != secondSeries->getReconstructionDB()[0]);

}

//------------------------------------------------------------------------------

void ModelSeriesTest::shallowCopyTest()
{
    CPPUNIT_ASSERT(m_series);

    data::Reconstruction::sptr rec1 = data::Reconstruction::New();
    data::Mesh::sptr mesh1          = data::Mesh::New();
    utestData::generator::Mesh::generateQuadMesh(mesh1);
    rec1->setMesh(mesh1);
    ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    data::ModelSeries::sptr secondSeries = data::ModelSeries::New();

    secondSeries->shallowCopy(m_series);

    CPPUNIT_ASSERT(m_series->getReconstructionDB()[0] == secondSeries->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(m_series->getReconstructionDB()[0], secondSeries->getReconstructionDB()[0]);

    CPPUNIT_ASSERT_EQUAL(recs[0], m_series->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(recs[0], secondSeries->getReconstructionDB()[0]);
    CPPUNIT_ASSERT_EQUAL(1, (int)m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(1, (int)secondSeries->getReconstructionDB().size());

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::data
