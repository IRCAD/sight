/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/ProcessObject.hpp>
#include "ProcessObjectTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ProcessObjectTest );

void ProcessObjectTest::constructeur()
{
        const std::string IMAGEID1 = "myImage1";
        const std::string IMAGEID2 = "myImage2";
        const std::string FIELDID1 = "myField1";
        const std::string FIELDID2 = "myField2";
        ::fwData::Image::sptr image1 = ::fwData::Image::New();
        ::fwData::Image::sptr image2 = ::fwData::Image::New();
        ::fwData::Integer::sptr field1 (new ::fwData::Integer(3));
        ::fwData::Integer::sptr field2 (new ::fwData::Integer(8));

        // process
        ::fwData::ProcessObject * po = new ::fwData::ProcessObject();
        po->setInputValue(IMAGEID1, image1);
        po->setInputValue(FIELDID1, field1);
        po->setInputValue(FIELDID2, field2);
        po->setOutputValue(IMAGEID2, image2);

        // check
        CPPUNIT_ASSERT_EQUAL(image1, po->getInput< ::fwData::Image >(IMAGEID1));
        CPPUNIT_ASSERT_EQUAL(field1, po->getInput< ::fwData::Integer >(FIELDID1));
        CPPUNIT_ASSERT_EQUAL(field2, po->getInput< ::fwData::Integer >(FIELDID2));
        CPPUNIT_ASSERT_EQUAL(image2, po->getOutput< ::fwData::Image >(IMAGEID2));
}
