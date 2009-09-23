/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/Deleter.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"

namespace fwServices
{

DefaultObjectDeleter::DefaultObjectDeleter() {};
DefaultObjectDeleter::~DefaultObjectDeleter() {};
void DefaultObjectDeleter::Delete(::fwTools::Object *obj)
{
	bool objExpired=false;
	try
	{
		obj->getSptr(); // must be an echec since object expired
	}
	catch (const ::boost::bad_weak_ptr&)
	{
		objExpired=true;
	}

	assert ( objExpired ); // if we pass here then the obj must be expired (in Deleter)
	fwServices::ObjectServiceRegistry::getDefault()->unregisterServices( ::fwTools::Object::sptr()  ); // obj expired in OSR : will be removed
}


}

