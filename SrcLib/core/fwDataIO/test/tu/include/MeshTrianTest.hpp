/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWDATAIO_UT_MESHTRIANTEST_HPP__
#define __FWDATAIO_UT_MESHTRIANTEST_HPP__

#include <fwData/Mesh.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataIO
{
namespace ut
{

/**
 * @brief Test many methods to read/create fw4spl trian mesh file.
 */
class MeshTrianTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MeshTrianTest );
CPPUNIT_TEST( testSimpleMesh );
CPPUNIT_TEST( testMeshWithCellNormals );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void testSimpleMesh();
    void testMeshWithCellNormals();

    void compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);


};

} //namespace ut
} //namespace fwDataIO

#endif //__FWDATAIO_UT_MESHTRIANTEST_HPP__
