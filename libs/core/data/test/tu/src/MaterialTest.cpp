/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <data/Color.hpp>
#include <data/Material.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( data::ut::MaterialTest );

namespace sight::data
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
    data::Color::sptr CAMBIENT = data::Color::New();
    CAMBIENT->setRGBA(0.5f, 0.5f, 0.5f, 0.5f);

    data::Color::sptr CDIFF = data::Color::New();
    CDIFF->setRGBA(0.8f, 0.2f, 0.5f, 0.4f);

    data::Material::sptr material = data::Material::New();

    material->setAmbient(data::Object::copy(CAMBIENT));
    material->setDiffuse(data::Object::copy(CDIFF));

    CPPUNIT_ASSERT_EQUAL(material->ambient()->getRGBA()[0], CAMBIENT->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(material->diffuse()->getRGBA()[0], CDIFF->getRGBA()[0]);
}

} //namespace ut
} //namespace sight::data
