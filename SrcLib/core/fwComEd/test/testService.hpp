/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWCOMEDSERVICES_TEST_SERVICE_HPP_
#define FWCOMEDSERVICES_TEST_SERVICE_HPP_
#include <iostream>
#include <sstream>
#include <vector>
#include <fwTools/UUID.hpp>
#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwCore/base.hpp>
#include <fwData/Object.hpp>
#include <fwTools/Factory.hpp>


class ITestService : public ::fwServices::IService
{

public :
	
	ITestService() throw() {};
	virtual ~ITestService() throw() {};
	virtual void configure() throw( ::fwTools::Failed ) {};	
	virtual void start() throw(::fwTools::Failed) {};
	virtual void stop() throw(::fwTools::Failed) {};
	virtual void update() throw(::fwTools::Failed) {};
	virtual void info(std::ostream &_sstream ) {_sstream << "ITestService" ;};
};

class ImplementationATestService : public ITestService
{

public :
	
	ImplementationATestService() throw() {};
	virtual ~ImplementationATestService() throw() {};
	virtual void configure() throw( ::fwTools::Failed ) {OSLM_INFO("CONFIGURE : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };	
	virtual void start() throw(::fwTools::Failed) {OSLM_INFO("START : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void stop() throw(::fwTools::Failed) {OSLM_INFO("STOP : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void update() throw(::fwTools::Failed) {OSLM_INFO("UPDATE : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void manageModification( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) {OSLM_INFO("MANAGE MODIFICATION : implementation = " << this->className() << ( this->hasUUID() ? " - uuid = " + this->getUUID() : " - no uuid" ) ); };
	virtual void info(std::ostream &_sstream ) {_sstream << "ImplementationATestService" ;};
};

REGISTER_SERVICE( ::ITestService , ::ImplementationATestService , ::fwData::Object ) ;

#endif /*XMLXSD_HPP_*/
