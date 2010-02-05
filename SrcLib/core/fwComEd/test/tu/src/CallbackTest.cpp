/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>

#include <fwComEd/DispatchMessage.hxx>
#include <fwComEd/Dictionary.hpp>


#include "CallbackTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CallbackTest );

REGISTER_CALLBACK( ::fwComEd::Dictionary::m_windowMinId , CallbackTest , testCallback );
REGISTER_CALLBACK( ::fwComEd::Dictionary::m_windowMaxId , CallbackTest , testCallback2 );

//------------------------------------------------------------------------------

void CallbackTest::setUp()
{
    // Set up context before running a test.
    this->m_isCallback1 = false;
    this->m_isCallback2 = false;
}

//------------------------------------------------------------------------------

void CallbackTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CallbackTest::methodeCallback()
{
    ::fwData::Integer::sptr minField = ::fwData::Integer::New(15);
    ::fwData::Integer::sptr maxField = ::fwData::Integer::New(150);

    // message with event corresponding to callback
    ::fwServices::ObjectMsg::NewSptr objMessage;
    objMessage->addEvent(::fwComEd::Dictionary::m_windowMinId, minField);
    objMessage->addEvent(::fwComEd::Dictionary::m_windowMaxId, maxField);

    // dispatch the message to callback
    ::fwComEd::DispatchMessage< ::CallbackTest >(objMessage, this);

    // check if the callbacks have received the message
    CPPUNIT_ASSERT(this->m_isCallback1);
    CPPUNIT_ASSERT(this->m_isCallback2);
}

void CallbackTest::testCallback( ::fwServices::ObjectMsg::csptr _msg )
{
    // check receive message with minField
    CPPUNIT_ASSERT( _msg->hasEvent( ::fwComEd::Dictionary::m_windowMinId ) );
    ::fwData::Integer::csptr minField = ::fwData::Integer::dynamicConstCast(_msg->getDataInfo( ::fwComEd::Dictionary::m_windowMinId ));

    CPPUNIT_ASSERT( 15, minField->value());

    this->m_isCallback1 = true;
}

void CallbackTest::testCallback2( ::fwServices::ObjectMsg::csptr _msg )
{
    // check receive message with maxField
    CPPUNIT_ASSERT( _msg->hasEvent( ::fwComEd::Dictionary::m_windowMaxId ) );
    ::fwData::Integer::csptr maxField = ::fwData::Integer::dynamicConstCast(_msg->getDataInfo( ::fwComEd::Dictionary::m_windowMaxId ));

    CPPUNIT_ASSERT( 150, maxField->value());

    this->m_isCallback2 = true;
}

//------------------------------------------------------------------------------
