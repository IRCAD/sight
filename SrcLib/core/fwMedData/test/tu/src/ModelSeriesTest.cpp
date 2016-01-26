/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMedData/ModelSeries.hpp"

#include "ModelSeriesTest.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwTest/generator/Mesh.hpp>
#include <fwTools/Type.hpp>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::ModelSeriesTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void ModelSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::ModelSeries::New();
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

    ::fwData::Reconstruction::sptr rec1 = ::fwData::Reconstruction::New();
    CPPUNIT_ASSERT(rec1);
    ::fwData::Reconstruction::sptr rec2 = ::fwData::Reconstruction::New();
    CPPUNIT_ASSERT(rec2);

    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT(mesh1);
    ::fwTest::generator::Mesh::generateQuadMesh(mesh1);

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    CPPUNIT_ASSERT(mesh2);
    ::fwTest::generator::Mesh::generateQuadMesh(mesh2);

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

    ::fwData::Reconstruction::sptr rec1 = ::fwData::Reconstruction::New();
    ::fwData::Mesh::sptr mesh1          = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateQuadMesh(mesh1);

    rec1->setMesh(mesh1);
    ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    ::fwMedData::ModelSeries::sptr secondSeries = ::fwMedData::ModelSeries::New();

    secondSeries->deepCopy(m_series);

    CPPUNIT_ASSERT_EQUAL(1, (int)m_series->getReconstructionDB().size());
    CPPUNIT_ASSERT_EQUAL(1, (int)secondSeries->getReconstructionDB().size());
    CPPUNIT_ASSERT (m_series->getReconstructionDB()[0] != secondSeries->getReconstructionDB()[0]);

}

//------------------------------------------------------------------------------

void ModelSeriesTest::shallowCopyTest()
{
    CPPUNIT_ASSERT(m_series);

    ::fwData::Reconstruction::sptr rec1 = ::fwData::Reconstruction::New();
    ::fwData::Mesh::sptr mesh1          = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateQuadMesh(mesh1);
    rec1->setMesh(mesh1);
    ModelSeries::ReconstructionVectorType recs;
    recs.push_back(rec1);
    m_series->setReconstructionDB(recs);

    ::fwMedData::ModelSeries::sptr secondSeries = ::fwMedData::ModelSeries::New();

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
} //namespace fwMedData
