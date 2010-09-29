/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TEST_TU_PROCESSOBJECTMSGTEST_HPP_
#define _FWCOMED_TEST_TU_PROCESSOBJECTMSGTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IService.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

/**
 * @brief Test ProcessObject building from a ConfigurationElement.
 * @see IProcessObjectParser
 */
class ProcessObjectParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ProcessObjectParserTest );
    CPPUNIT_TEST( testBuildProcessObject );
    CPPUNIT_TEST_SUITE_END();

protected:

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests

    /// Test building a ProcessObject with a ConfigurationElement
    void testBuildProcessObject();

private:
    /// Create a configurationElement to build the composite
    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildConfig();
};

/**
 * @brief   Service interface for test.
 */
class TestService : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) ) ;
    TestService() throw()
    :   m_isUpdated(false),
        m_isUpdatedMessage(false)
        {};

    virtual ~TestService() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {};
    virtual void updating() throw(::fwTools::Failed) {};
    virtual void info(std::ostream &_sstream ) {_sstream << "TestService" ;};

    bool getIsUpdated() { return m_isUpdated; };
    ::fwServices::ObjectMsg::sptr getMessage() { return m_compoMsg; };

protected:
    bool m_isUpdated;
    bool m_isUpdatedMessage;
    ::fwServices::ObjectMsg::sptr m_compoMsg;
};

/**
 * @brief   Service implementation for test.
 */
class TestServiceImplementation : public TestService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementation)(::TestService) ) ;
    TestServiceImplementation() throw() {};
    virtual ~TestServiceImplementation() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {};
    virtual void updating() throw(::fwTools::Failed) { m_isUpdated = true; };
    virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {};

    virtual void info(std::ostream &_sstream ) {_sstream << "TestServiceImplementation" ;};
};

REGISTER_SERVICE( TestService , ::TestServiceImplementation , ::fwTools::Object ) ;

#endif // _FWCOMED_TEST_TU_PROCESSOBJECTMSGTEST_HPP_
