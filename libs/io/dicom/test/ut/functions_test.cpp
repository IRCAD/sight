/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "functions_test.hpp"

#include "io/dicom/helper/functions.hpp"

#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::functions_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void functions_test::setUp()
{
}

//------------------------------------------------------------------------------

void functions_test::tearDown()
{
}

//------------------------------------------------------------------------------

void functions_test::generate_patient_id_test()
{
    const std::string id = io::dicom::helper::generate_patient_id();
    CPPUNIT_ASSERT(64 >= id.length());
    CPPUNIT_ASSERT(0 < id.length());
}

//------------------------------------------------------------------------------

void functions_test::generate_study_instance_uid_test()
{
    const std::string id = io::dicom::helper::generate_study_instance_uid();
    CPPUNIT_ASSERT(16 == id.length());
}

} // namespace sight::io::dicom::ut
