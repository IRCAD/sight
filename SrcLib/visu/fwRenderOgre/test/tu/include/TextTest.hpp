/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

#include <OgreRoot.h>

namespace fwRenderOgre
{
namespace ut
{

class TextTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( TextTest );
CPPUNIT_TEST( factoryTest );
CPPUNIT_TEST_SUITE_END();

public:

    TextTest();
    virtual ~TextTest();

    void setUp() override;
    void tearDown() override;

    void factoryTest();

private:
    /// Ogre root to setup the scene and instantiate 'Text' objects.
    ::Ogre::Root* m_ogreRoot {nullptr};

};

} //namespace ut
} //namespace fwRenderOgre
