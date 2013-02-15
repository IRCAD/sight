/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesContainer.hpp>

#include <fwAtoms/Sequence.hpp>
#include <fwAtomConversion/AtomHelper.hpp>
#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include "fwMedDataCamp/SeriesContainerTest.hpp"
#include "fwMedDataCamp/MedDataCampHelper.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::SeriesContainerTest );

namespace fwMedDataCamp
{
namespace ut
{

void SeriesContainerTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void SeriesContainerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesContainerTest::propertiesTest()
{
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("values");

    ::fwMedData::SeriesContainer::sptr obj = ::fwMedData::SeriesContainer::New();
    ::fwMedData::SeriesContainer::ContainerType vectSeries;
    vectSeries.push_back(::fwMedData::ImageSeries::New());
    vectSeries.push_back(::fwMedData::ImageSeries::New());
    vectSeries.push_back(::fwMedData::ModelSeries::New());
    obj->setContainer(vectSeries);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.0", vectSeries[0]);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.1", vectSeries[1]);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.2", vectSeries[2]);

    ::fwAtomConversion::AtomHelper metaHelper;
    ::fwAtoms::Object::sptr metaObject = metaHelper.dataToMeta(obj);
    ::fwAtoms::Object::Attributes attrs = metaObject->getAttributes();

    CPPUNIT_ASSERT_MESSAGE("Attributes values not found in SeriesContainer atom",
                           attrs.find("values") != attrs.end());

    ::fwAtoms::Base::sptr baseAtom = metaObject->getAttribut("values");
    CPPUNIT_ASSERT_MESSAGE("Bad Atom SeriesContainer conversion", baseAtom->isSequence());

    ::fwAtoms::Sequence::sptr seqAtom = ::fwAtoms::Sequence::dynamicCast(baseAtom);
    CPPUNIT_ASSERT_MESSAGE("Sequence dynamicCast failed", seqAtom);
    CPPUNIT_ASSERT_EQUAL( vectSeries.size(), seqAtom->getValue().size());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
