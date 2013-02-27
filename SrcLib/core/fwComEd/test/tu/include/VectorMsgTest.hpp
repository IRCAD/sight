/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_TU_VECTORMSGTEST_HPP__
#define __FWCOMED_TU_VECTORMSGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include "ConfigParserTest.hpp"

namespace fwComEd
{
namespace ut
{

/**
 * @brief   Tests for VectorMsg.
 */
class VectorMsgTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( VectorMsgTest );
    CPPUNIT_TEST( basicTest );
    CPPUNIT_TEST( vectorHelperTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();

    void vectorHelperTest();

};

class SVectorTest : public ::fwComEd::ut::TestService
{

public:
    virtual ~SVectorTest() throw() {};

    fwCoreServiceClassDefinitionsMacro ( (SVectorTest)(::fwComEd::ut::TestService) ) ;

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        m_isUpdated = true;
        m_vectMsg = ::fwComEd::VectorMsg::dynamicConstCast( _msg ) ;
    }

    ::fwComEd::VectorMsg::csptr m_vectMsg;
};

} //namespace ut
} //namespace fwComEd

#endif // __FWCOMED_TU_VECTORMSGTEST_HPP__
