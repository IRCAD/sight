/************************************************************************
 *
 * Copyright (C) 2004-2023 IRCAD France
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::ut
{

class functions_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(functions_test);
CPPUNIT_TEST(generate_patient_id_test);
CPPUNIT_TEST(generate_study_instance_uid_test);
CPPUNIT_TEST_SUITE_END();

public:

    /**
     * @name Override
     * @{ */
    void setUp() override;
    void tearDown() override;
    /**  @} */

    /**
     * @name Tests UID generation methods by checking length and content.
     * @{ */
    static void generate_patient_id_test();
    static void generate_study_instance_uid_test();
    /**  @} */
};

} // namespace sight::io::dicom::ut
