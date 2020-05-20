/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "UtilsTest.hpp"

#ifdef WIN32
#define UIVISUOGRE_EXPORTS
#endif
#include "../../../src/uiVisuOgre/helper/Utils.cpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::uiVisuOgre::ut::UtilsTest );

namespace uiVisuOgre
{
namespace ut
{

//------------------------------------------------------------------------------

void UtilsTest::setUp()
{
}

//------------------------------------------------------------------------------

void UtilsTest::tearDown()
{
}

//------------------------------------------------------------------------------

void UtilsTest::convertOgreColorToQColor()
{
    QColor refColor(255, 255, 255);

    QColor resultColor = ::uiVisuOgre::helper::Utils::converOgreColorToQColor(::Ogre::ColourValue());
    CPPUNIT_ASSERT(resultColor.red() == refColor.red());
    CPPUNIT_ASSERT(resultColor.green() == refColor.green());
    CPPUNIT_ASSERT(resultColor.blue() == refColor.blue());
    CPPUNIT_ASSERT(resultColor.alpha() == refColor.alpha());
}

//------------------------------------------------------------------------------

void UtilsTest::convertQColorToOgreColor()
{
    ::Ogre::ColourValue refColor;

    ::Ogre::ColourValue resultColor = ::uiVisuOgre::helper::Utils::convertQColorToOgreColor(QColor(255, 255, 255));
    CPPUNIT_ASSERT(static_cast<int>(resultColor.r) == static_cast<int>(refColor.r));
    CPPUNIT_ASSERT(static_cast<int>(resultColor.g) == static_cast<int>(refColor.g));
    CPPUNIT_ASSERT(static_cast<int>(resultColor.b) == static_cast<int>(refColor.b));
    CPPUNIT_ASSERT(static_cast<int>(resultColor.a) == static_cast<int>(refColor.a));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace uiVisuOgre
