/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Dictionary.hpp>

#include "DictionaryTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DictionaryTest );

void DictionaryTest::setUp()
{
    // Set up context before running a test.

}
void DictionaryTest::tearDown()
{
    // Clean up after the test run.
}

void DictionaryTest::methode1()
{
    ::boost::uint32_t VALUE = 0 ;
    std::string DICTIONARYORGANNAME = "organName23";

    // process
    ::fwData::Dictionary::NewSptr p1;
    ::fwData::DictionaryOrgan::sptr defaultOrgan ; //NULL shared_ptr
    ::fwData::DictionaryOrgan::sptr dictionaryOrgan(new ::fwData::DictionaryOrgan);
    dictionaryOrgan->setCRefStructureType(DICTIONARYORGANNAME) ;


    // check
    CPPUNIT_ASSERT_EQUAL(p1-> hasDictionaryOrgan(DICTIONARYORGANNAME),  false);
    CPPUNIT_ASSERT_EQUAL(p1-> getDictionaryOrgan(DICTIONARYORGANNAME), defaultOrgan);

    //process
    p1->setDictionaryOrgan(dictionaryOrgan) ;

    //re-check changes
    CPPUNIT_ASSERT_EQUAL(p1-> hasDictionaryOrgan(DICTIONARYORGANNAME),  true);
    CPPUNIT_ASSERT_EQUAL(p1-> getDictionaryOrgan(DICTIONARYORGANNAME),  dictionaryOrgan);



}
