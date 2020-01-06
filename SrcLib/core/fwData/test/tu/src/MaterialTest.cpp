/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "MaterialTest.hpp"

#include <fwData/Color.hpp>
#include <fwData/Material.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MaterialTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void MaterialTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void MaterialTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MaterialTest::methode1()
{
    //-----------test values
    ::fwData::Color::sptr CAMBIENT = ::fwData::Color::New();
    CAMBIENT->setRGBA(0.5f, 0.5f, 0.5f, 0.5f);

    ::fwData::Color::sptr CDIFF = ::fwData::Color::New();
    CDIFF->setRGBA(0.8f, 0.2f, 0.5f, 0.4f);

    ::fwData::Material::sptr material = ::fwData::Material::New();

    material->setAmbient(::fwData::Object::copy(CAMBIENT));
    material->setDiffuse(::fwData::Object::copy(CDIFF));

    CPPUNIT_ASSERT_EQUAL(material->ambient()->getRGBA()[0], CAMBIENT->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(material->diffuse()->getRGBA()[0], CDIFF->getRGBA()[0]);
}

} //namespace ut
} //namespace fwData
