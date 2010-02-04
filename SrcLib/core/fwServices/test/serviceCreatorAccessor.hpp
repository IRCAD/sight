/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICES_TEST_SERVICECREATORACCESSOR_HPP_
#define FWSERVICES_TEST_SERVICECREATORACCESSOR_HPP_
#include <iostream>
#include <sstream>
#include <vector>
#include <fwTools/UUID.hpp>
#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>
#include <fwData/Object.hpp>
#include <fwData/Image.hpp>
#include <fwData/Factory.hpp>


class ITestService : public ::fwServices::IService
{

public :
	
	ITestService() throw() {};
	virtual ~ITestService() throw() {};
	virtual void configuring() throw( ::fwTools::Failed ) {};	
	virtual void starting() throw(::fwTools::Failed) {};
	virtual void stopping() throw(::fwTools::Failed) {};
	virtual void updating() throw(::fwTools::Failed) {};
	virtual void info(std::ostream &_sstream ) {_sstream << "ITestService" ;};
};

class ImplementationATestService : public ITestService
{

public :
	
	ImplementationATestService() throw() {};
	virtual ~ImplementationATestService() throw() {};
	virtual void configuring() throw( ::fwTools::Failed ) {OSLM_INFO("CONFIGURE : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };	
	virtual void starting() throw(::fwTools::Failed) {OSLM_INFO("START : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void stopping() throw(::fwTools::Failed) {OSLM_INFO("STOP : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void updating() throw(::fwTools::Failed) {OSLM_INFO("UPDATE : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {OSLM_INFO("MANAGE MODIFICATION : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void info(std::ostream &_sstream ) {_sstream << "ImplementationATestService" ;};
};

REGISTER_SERVICE( ::ITestService , ::ImplementationATestService , ::fwTools::Object ) ;
//REGISTER_SERVICE( ::ITestService , ::ImplementationATestService , ::fwData::Image ) ;

int testServiceAdd()
{
	::boost::shared_ptr< ::fwTools::Object > obj = ::fwData::Factory::New< ::fwTools::Object >();	
	 ::boost::shared_ptr< ::fwServices::IService > service ;
	
	/// Without id
	assert( ::fwServices::support( obj , "::ITestService" ) );
	service = ::fwServices::add( obj , "::ITestService" , "::ImplementationATestService" ) ; 
	OSLM_INFO( "First added service : " << *service );
	service = ::fwServices::add( obj , "::ITestService" , "::ImplementationATestService" ) ;
	OSLM_INFO( "Second added service : " << *service );
	OSLM_INFO( "Number of service of type ITestService : " << ::fwServices::getServices( obj , "::ITestService" ).size() );
	::fwServices::eraseServices( obj , "::ITestService" ) ;
	OSLM_INFO( "Number of service of type ITestService : " << ::fwServices::getServices( obj , "::ITestService" ).size() );

	
	/// With id	
	service = ::fwServices::add( obj , "::ITestService" , "::ImplementationATestService" , "myFirstOne") ;	
	OSLM_INFO( "First added service : " << *service << " - with ID : " << ::fwTools::UUID::get< ::fwServices::IService >( service , ::fwTools::UUID::SIMPLE ) );
	service = ::fwServices::add< ::ITestService >( obj , "::ImplementationATestService" , "mySecondOne") ;	
	OSLM_INFO( "Second added service : " << *service << " - with ID : " << ::fwTools::UUID::get< ::fwServices::IService >( service , ::fwTools::UUID::SIMPLE ) );
	
	service = ::fwServices::get( obj , "::ITestService" , "mySecondOne") ;
	OSLM_INFO( "Get service ::ITestService mySecondOne exists : " << (service != ::boost::shared_ptr< ::fwServices::IService >() ? "OK" : "NOK") );
	service = ::fwServices::get< ::ITestService >( obj , "mySecondOne") ;
	OSLM_INFO( "Get service ::ITestService mySecondOne exists : " << (service != ::boost::shared_ptr< ::fwServices::IService >() ? "OK" : "NOK") );
	service = ::fwServices::get( obj , "::ITestService" , "myPipo") ;
	OSLM_INFO( "Get service ::ITestService myPipo does not exists : " << (service == ::boost::shared_ptr< ::fwServices::IService >() ? "OK" : "NOK" ) );
		
	
    return 1 ;
}

#endif /*XMLXSD_HPP_*/
