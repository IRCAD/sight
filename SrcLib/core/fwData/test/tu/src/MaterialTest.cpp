/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Material.hpp>
#include <fwData/Color.hpp>
#include "MaterialTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MaterialTest );

namespace fwData
{
namespace ut
{

void MaterialTest::setUp()
{
    // Set up context before running a test.

}
void MaterialTest::tearDown()
{
    // Clean up after the test run.
}

void MaterialTest::methode1()
{
    //-----------test values
    ::fwData::Color::NewSptr CAMBIENT;
    CAMBIENT->setRGBA(0.5f,0.5f,0.5f,0.5f);

    ::fwData::Color::NewSptr CDIFF;
    CDIFF->setRGBA(0.8f,0.2f,0.5f,0.4f);

    ::fwData::Material::NewSptr material;

    material->ambient()->deepCopy( CAMBIENT );
    material->diffuse()->deepCopy( CDIFF );

    CPPUNIT_ASSERT_EQUAL(material->ambient()->getRGBA()[0], CAMBIENT->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(material->diffuse()->getRGBA()[0], CDIFF->getRGBA()[0]);
}

} //namespace ut
} //namespace fwData

