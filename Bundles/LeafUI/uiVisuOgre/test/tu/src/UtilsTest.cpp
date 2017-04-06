/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    CPPUNIT_ASSERT(resultColor.r == refColor.r);
    CPPUNIT_ASSERT(resultColor.g == refColor.g);
    CPPUNIT_ASSERT(resultColor.b == refColor.b);
    CPPUNIT_ASSERT(resultColor.a == refColor.a);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace uiVisuOgre
