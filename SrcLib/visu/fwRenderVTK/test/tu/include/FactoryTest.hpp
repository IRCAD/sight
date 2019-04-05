/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#pragma once

#include <fwRenderVTK/vtk/Instantiator.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwRenderVTK
{
namespace ut
{

class FactoryTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FactoryTest );
CPPUNIT_TEST(instantiationTest);
CPPUNIT_TEST_SUITE_END();

public:
    // Set up before runing test.
    void setUp();
    // Clean every thing after the test.
    void tearDown();

    void instantiationTest();

    ::fwRenderVTK::vtk::Instantiator* m_factory;
};

} //namespace ut
} //namespace arData
