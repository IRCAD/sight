/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/DownCastIterator.hpp>
#include "DownCastIteratorTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( DownCastIteratorTest );

void DownCastIteratorTest::constructeur()
{


    // process
    ::fwData::DownCastIterator * p1 = new ::fwData::DownCastIterator();

    // check
    CPPUNIT_ASSERT_EQUAL(,  );

}
