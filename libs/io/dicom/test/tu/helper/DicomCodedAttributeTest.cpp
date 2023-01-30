/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "DicomCodedAttributeTest.hpp"

#include <io/dicom/container/DicomCodedAttribute.hpp>
#include <io/dicom/helper/DicomCodedAttribute.hpp>

#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::DicomCodedAttributeTest);

namespace sight::io::dicom::helper::ut
{

using helper = helper::DicomCodedAttribute;

//------------------------------------------------------------------------------

void DicomCodedAttributeTest::convertEntryToCodedAttributeTest()
{
    std::vector<container::DicomCodedAttribute> result = helper::convertEntryToCodedAttribute(
        "(AAA;BBB;CCC)(DDD;EEE;FFF)"
    );
    CPPUNIT_ASSERT_EQUAL(container::DicomCodedAttribute("AAA", "BBB", "CCC"), result[0]);
    CPPUNIT_ASSERT_EQUAL(container::DicomCodedAttribute("DDD", "EEE", "FFF"), result[1]);
}

//------------------------------------------------------------------------------

void DicomCodedAttributeTest::convertEntryToGdcmCodedAttributeTest()
{
    using namespace std::literals::string_literals;

    gdcm::Segment::BasicCodedEntryVector result =
        helper::convertEntryToGDCMCodedAttribute("(AAA;BBB;CCC)(DDD;EEE;FFF)");

    CPPUNIT_ASSERT_EQUAL("AAA"s, result[0].CV);
    CPPUNIT_ASSERT_EQUAL("BBB"s, result[0].CSD);
    CPPUNIT_ASSERT_EQUAL("CCC"s, result[0].CM);

    CPPUNIT_ASSERT_EQUAL("DDD"s, result[1].CV);
    CPPUNIT_ASSERT_EQUAL("EEE"s, result[1].CSD);
    CPPUNIT_ASSERT_EQUAL("FFF"s, result[1].CM);
}

//------------------------------------------------------------------------------

void DicomCodedAttributeTest::checkAndFormatEntryWithWrongValueTest()
{
    std::string input = "(AAA;BBB)";
    CPPUNIT_ASSERT(!helper::checkAndFormatEntry(input));

    input = "(AAA;BBB;CCC";
    CPPUNIT_ASSERT(!helper::checkAndFormatEntry(input));

    input = "AAA;BBB;CCC)";
    CPPUNIT_ASSERT(!helper::checkAndFormatEntry(input));
}

//------------------------------------------------------------------------------

void DicomCodedAttributeTest::checkAndFormatEntryWithSingleValueTest()
{
    using namespace std::literals::string_literals;

    std::string input = "(AAA;BBB;CCC)";
    CPPUNIT_ASSERT(helper::checkAndFormatEntry(input));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)"s, input);

    input = "   (AAA;BBB;CCC)   ";
    CPPUNIT_ASSERT(helper::checkAndFormatEntry(input));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)"s, input);

    input = "   (AAA;BBB;CCC)   ";
    CPPUNIT_ASSERT(helper::checkAndFormatEntry(input, true));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)"s, input);
}

//------------------------------------------------------------------------------

void DicomCodedAttributeTest::checkAndFormatEntryWithMultipleValuesTest()
{
    using namespace std::literals::string_literals;

    std::string input = "(AAA;BBB;CCC)(DDD;EEE;FFF)";
    CPPUNIT_ASSERT(!helper::checkAndFormatEntry(input));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)(DDD;EEE;FFF)"s, input);

    input = "   (AAA;BBB;CCC)   (DDD;EEE;FFF)   ";
    CPPUNIT_ASSERT(!helper::checkAndFormatEntry(input));
    CPPUNIT_ASSERT_EQUAL("   (AAA;BBB;CCC)   (DDD;EEE;FFF)   "s, input); // The input isn't formatted if it is wrong

    input = "(AAA;BBB;CCC)(DDD;EEE;FFF)";
    CPPUNIT_ASSERT(helper::checkAndFormatEntry(input, true));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)(DDD;EEE;FFF)"s, input);

    input = "   (AAA;BBB;CCC)   (DDD;EEE;FFF)   ";
    CPPUNIT_ASSERT(helper::checkAndFormatEntry(input, true));
    CPPUNIT_ASSERT_EQUAL("(AAA;BBB;CCC)(DDD;EEE;FFF)"s, input);
}

} // namespace sight::io::dicom::helper::ut
