/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ModelSeriesTest.hpp"

#include <fwData/Color.hpp>
#include <fwData/Material.hpp>

#include <fwMedData/ModelSeries.hpp>
#include <fwMedDataTools/ModelSeries.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataTools::ut::ModelSeriesTest );

namespace fwMedDataTools
{

namespace ut
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
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
    ::fwData::Reconstruction::sptr rec1        = ::fwData::Reconstruction::New();

    ::fwMedDataTools::ModelSeries::addReconstruction(modelSeries, rec1);

    const ::fwMedData::ModelSeries::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);

    ::fwData::Reconstruction::sptr rec2 = ::fwData::Reconstruction::New();

    ::fwMedDataTools::ModelSeries::addReconstruction(modelSeries, rec2);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);

    ::fwData::Reconstruction::sptr rec3 = ::fwData::Reconstruction::New();
    ::fwMedDataTools::ModelSeries::addReconstruction(modelSeries, rec3);

    CPPUNIT_ASSERT_EQUAL(rec1, recDB[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, recDB[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, recDB[2]);

    CPPUNIT_ASSERT_THROW(::fwMedDataTools::ModelSeries::addReconstruction(modelSeries, rec1), ::fwCore::Exception);

    ::fwData::Reconstruction::sptr nullRec;
    CPPUNIT_ASSERT_THROW(::fwMedDataTools::ModelSeries::addReconstruction(modelSeries, nullRec), ::fwCore::Exception);

}

//------------------------------------------------------------------------------

void ModelSeriesTest::createReconstructionFromMesh()
{
    ::fwData::Mesh::sptr mesh   = ::fwData::Mesh::New();
    ::fwData::Color::sptr color = ::fwData::Color::New(1.0f, 0.3f, 0.7f, 0.5f);

    {
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        auto rec = ::fwMedDataTools::ModelSeries::createReconstructionFromMesh( mesh, organName, structureType, color,
                                                                                ::fwData::Material::MODE_SURFACE);

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(),  ::fwData::Material::MODE_SURFACE);

        auto ambient = rec->getMaterial()->ambient();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0f);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3f);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7f);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5f);
    }

    {
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        auto rec = ::fwMedDataTools::ModelSeries::createReconstructionFromMesh( mesh, organName, structureType, color,
                                                                                ::fwData::Material::MODE_WIREFRAME,
                                                                                false);

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(),  ::fwData::Material::MODE_WIREFRAME);

        auto ambient = rec->getMaterial()->ambient();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0f);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3f);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7f);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5f);
    }
}

//------------------------------------------------------------------------------

void ModelSeriesTest::addMesh()
{
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
    const ::fwMedData::ModelSeries::ReconstructionVectorType& recDB = modelSeries->getReconstructionDB();

    {
        ::fwData::Mesh::sptr mesh   = ::fwData::Mesh::New();
        ::fwData::Color::sptr color = ::fwData::Color::New(1.0f, 0.3f, 0.7f, 0.5f);
        const std::string organName     = "Liver1";
        const std::string structureType = "Liver";

        ::fwMedDataTools::ModelSeries::addMesh( modelSeries, mesh, organName, structureType, color,
                                                ::fwData::Material::MODE_SURFACE);

        auto rec = recDB[0];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(),  ::fwData::Material::MODE_SURFACE);

        auto ambient = rec->getMaterial()->ambient();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0f);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3f);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7f);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5f);

    }

    {
        ::fwData::Mesh::sptr mesh   = ::fwData::Mesh::New();
        ::fwData::Color::sptr color = ::fwData::Color::New(0.1f, 0.3f, 0.7f, 1.5f);
        const std::string organName     = "Foo";
        const std::string structureType = "Foofoo";

        ::fwMedDataTools::ModelSeries::addMesh( modelSeries, mesh, organName, structureType, color,
                                                ::fwData::Material::MODE_WIREFRAME,
                                                false);
        auto rec = recDB[1];

        CPPUNIT_ASSERT_EQUAL(rec->getMesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->getOrganName(), organName);
        CPPUNIT_ASSERT_EQUAL(rec->getStructureType(), structureType);
        CPPUNIT_ASSERT_EQUAL(rec->getIsVisible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->getMaterial()->getRepresentationMode(),  ::fwData::Material::MODE_WIREFRAME);

        auto ambient = rec->getMaterial()->ambient();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 0.1f);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3f);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7f);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 1.5f);
    }
}


//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwMedDataTools
