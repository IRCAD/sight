/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "UtilsTest.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreColourValue.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::ut::UtilsTest );

namespace fwRenderOgre
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

void UtilsTest::convertOgreColorToFwColor()
{
    ::fwData::Color::sptr refColor = ::fwData::Color::New();
    refColor->setRGBA(1.f, 1.f, 1.f, 1.f);

    ::fwData::Color::sptr resultColor = ::fwRenderOgre::Utils::convertOgreColorToFwColor(::Ogre::ColourValue());
    CPPUNIT_ASSERT(static_cast<int>(resultColor->red()) == static_cast<int>(refColor->red()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->green()) == static_cast<int>(refColor->green()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->blue()) == static_cast<int>(refColor->blue()));
    CPPUNIT_ASSERT(static_cast<int>(resultColor->alpha()) == static_cast<int>(refColor->alpha()));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwRenderOgre
