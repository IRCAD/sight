/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

namespace sight::atoms::conversion
{

namespace ut
{

class ConversionTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ConversionTest);
CPPUNIT_TEST(dataToAtomTest);
CPPUNIT_TEST(materialConversionTest);
CPPUNIT_TEST(meshConversionTest);
CPPUNIT_TEST(graphConversionTest);
CPPUNIT_TEST(tfConversionTest);
CPPUNIT_TEST(seriesDBConversionTest);
CPPUNIT_TEST(objectMultiReferencedTest);
CPPUNIT_TEST(recursiveObjectTest);
CPPUNIT_TEST(uuidExceptionTest);
CPPUNIT_TEST(uuidChangeTest);
CPPUNIT_TEST(uuidReuseTest);
CPPUNIT_TEST(dataFactoryNotFoundExceptionTest);
CPPUNIT_TEST(campFactoryNotFoundExceptionTest);
CPPUNIT_TEST(conversionNotManagedExceptionTest);
CPPUNIT_TEST(nullPtrManagmentTest);
CPPUNIT_TEST(landmarksConversionTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    /// Test conversion from fwData to fwAtoms
    void dataToAtomTest();

    /// Test the conversion data::Material -> atoms::Object -> data::Material
    void materialConversionTest();

    /// Test the conversion data::Mesh -> atoms::Object -> data::Mesh
    void meshConversionTest();

    //// Test data::Graph conversion, UUID management, and object field conversion
    void graphConversionTest();

    //// Test data::TransferFunction conversion
    void tfConversionTest();

    /// Test data::SeriesDB conversion
    void seriesDBConversionTest();

    /// Test using an object multi referenced in different structures, test also data::Composite conversion
    void objectMultiReferencedTest();

    /// Test recursive data conversion
    void recursiveObjectTest();

    /// Test exception throwing if uuid already exist
    void uuidExceptionTest();

    /// Test exception throwing if uuid already exist
    void uuidChangeTest();

    /// Test exception throwing if uuid already exist
    void uuidReuseTest();

    /// Test exception throwing if during atom to data conversion a classname is not found in data factory
    void dataFactoryNotFoundExceptionTest();

    /// Test exception throwing if during data to atom conversion a classname is not found in camp factory
    void campFactoryNotFoundExceptionTest();

    /// Test exception throwing if during data/atom conversion a conversion is not managed
    void conversionNotManagedExceptionTest();

    /// Test null ptr management (null ptr attribut, null ptr in vector, null ptr in map)
    void nullPtrManagmentTest();

    //// Test data::Landmarks conversion
    void landmarksConversionTest();
};

} // namespace ut

} // namespace sight::atoms::conversion
