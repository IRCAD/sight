/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ProcessObjectTest.hpp"

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/ProcessObject.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::ut::ProcessObjectTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void ProcessObjectTest::constructeur()
{
    const std::string IMAGEID1 = "myImage1";
    const std::string IMAGEID2 = "myImage2";
    const std::string FIELDID1 = "myField1";
    const std::string FIELDID2 = "myField2";
    data::Image::sptr image1   = data::Image::New();
    data::Image::sptr image2   = data::Image::New();
    data::Integer::sptr field1 = data::Integer::New(3);
    data::Integer::sptr field2 = data::Integer::New(8);

    // process
    data::ProcessObject::sptr po = data::ProcessObject::New();
    po->setInputValue(IMAGEID1, image1);
    po->setInputValue(FIELDID1, field1);
    po->setInputValue(FIELDID2, field2);
    po->setOutputValue(IMAGEID2, image2);

    // check
    CPPUNIT_ASSERT_EQUAL(image1, po->getInput< data::Image >(IMAGEID1));
    CPPUNIT_ASSERT_EQUAL(field1, po->getInput< data::Integer >(FIELDID1));
    CPPUNIT_ASSERT_EQUAL(field2, po->getInput< data::Integer >(FIELDID2));
    CPPUNIT_ASSERT_EQUAL(image2, po->getOutput< data::Image >(IMAGEID2));
}

} //namespace ut
} //namespace sight::data
