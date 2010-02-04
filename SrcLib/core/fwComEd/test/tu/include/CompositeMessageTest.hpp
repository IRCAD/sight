/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_
#define _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwRuntime/EConfigurationElement.hpp>


/**
 * @brief 	Test ProcessObject building from a ConfigurationElement and composite message.
 * @see 	ICompositeParser
 * @see 	CompositeEditor
 * @see 	CompositeMsg
 */
class CompositeMessageTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CompositeMessageTest );
	CPPUNIT_TEST( methodeBuildComposite );
	CPPUNIT_TEST( methodeCompositeMessage );
//	CPPUNIT_TEST( methodeMessageNotification );
	CPPUNIT_TEST_SUITE_END();

public:
	// interface
	void setUp();
	void tearDown();

	// fonctions de tests
	/// Test the composite building
	void methodeBuildComposite();

	/// Test the compositeMsg with sending and receiving the message in the test services
	void methodeCompositeMessage();

	/// Test sending and receiving message from a composite's object.
	void methodeMessageNotification();

private:
	/// Create a configurationElement to build the composite
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildConfig();
};

/**
 * @brief 	Service interface for test.
 */
class TestService : public ::fwServices::IService
{

public :

	fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService::Baseclass) ) ;
	TestService() throw()
	: 	m_isUpdated(false),
		m_isUpdatedMessage(false)
		{};

	virtual ~TestService() throw() {};

	virtual void configuring() throw( ::fwTools::Failed ) {};
	virtual void starting() throw(::fwTools::Failed) {};
	virtual void stopping() throw(::fwTools::Failed) {};
	virtual void updating() throw(::fwTools::Failed) {};
	virtual void info(std::ostream &_sstream ) {_sstream << "TestService" ;};

	/// return true if the service is updated with updating() method
	bool getIsUpdated() { return m_isUpdated; };

	/// return true if the service is updated with updating(msg) method
	bool getIsUpdatedMessage() { return m_isUpdatedMessage; };

	/// return the message receiving in updating(msg) method
	::fwServices::ObjectMsg::sptr getMessage() { return m_compoMsg; };

protected:
	bool m_isUpdated;
	bool m_isUpdatedMessage;
	::fwServices::ObjectMsg::sptr m_compoMsg;
};

/**
 * @brief 	Test service implementation for composite
 */
class TestServiceImplementationComposite : public TestService
{

public :
	fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationComposite)(::TestService::Baseclass) ) ;
	TestServiceImplementationComposite() throw() {};
	virtual ~TestServiceImplementationComposite() throw() {};

	virtual void configuring() throw( ::fwTools::Failed ) {};
	virtual void starting() throw(::fwTools::Failed) {};
	virtual void stopping() throw(::fwTools::Failed) {
		SLM_TRACE_FUNC();
	};
	virtual void updating() throw(::fwTools::Failed) { m_isUpdated = true; };
	virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
	{
		::fwComEd::CompositeMsg::csptr compositeMessage = ::fwComEd::CompositeMsg::dynamicConstCast( _msg );
		if (compositeMessage && compositeMessage->hasEvent(::fwComEd::CompositeMsg::MODIFIED_FIELDS))
		{
			// if receiving a compositeMsg : tag service is updated
			m_isUpdatedMessage = true;
			m_compoMsg = ::boost::const_pointer_cast< ::fwServices::ObjectMsg >( _msg ) ;
		}
	};
};

/**
 * @brief 	Test service implementation for image
 */
class TestServiceImplementationImage : public TestService
{

public :
	fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationImage)(::TestService::Baseclass) ) ;
	TestServiceImplementationImage() throw() {};
	virtual ~TestServiceImplementationImage() throw() {};

	virtual void configuring() throw( ::fwTools::Failed ) {};
	virtual void starting() throw(::fwTools::Failed) {};
	virtual void stopping() throw(::fwTools::Failed) {};
	virtual void updating() throw(::fwTools::Failed) { m_isUpdated = true; };
	virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
	{
		::fwComEd::ImageMsg::csptr imageMessage = ::fwComEd::ImageMsg::dynamicConstCast( _msg );
		if (imageMessage && imageMessage->hasEvent(::fwComEd::ImageMsg::WINDOWING))
		{
			// if receiving a imageMsg : tag service is updated
			m_isUpdatedMessage = true;
			m_compoMsg = ::boost::const_pointer_cast< ::fwServices::ObjectMsg >( _msg ) ;
		}
	};
};

REGISTER_SERVICE( TestService , ::TestServiceImplementationComposite , ::fwData::Composite ) ;
REGISTER_SERVICE( TestService , ::TestServiceImplementationImage , ::fwData::Image ) ;

#endif // _FWCOMED_TEST_TU_COMPOSITEMESSAGETEST_HPP_
