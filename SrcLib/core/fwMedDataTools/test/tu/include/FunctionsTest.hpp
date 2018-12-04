/************************************************************************
 *
 * Copyright (C) 2004-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__
#define __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedDataTools
{
namespace ut
{

class FunctionsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FunctionsTest );
CPPUNIT_TEST(generatePatientIdTest);
CPPUNIT_TEST(generateStudyInstanceUidTest);
CPPUNIT_TEST_SUITE_END();

public:
    /**
     * @name Override
     * @{ */
    void setUp();
    void tearDown();
    /**  @} */

    /**
     * @name Tests UID generation methods by checking length and content.
     * @{ */
    void generatePatientIdTest();
    void generateStudyInstanceUidTest();
    /**  @} */
};

} // namespace ut

} // namespace fwMedDataTools

#endif // __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__

