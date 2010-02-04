/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>


#include "fwData/DebugObject.hpp"

namespace fwData
{

//------------------------------------------------------------------------------

DebugObject::DebugObject()
{
}

//------------------------------------------------------------------------------

DebugObject::~DebugObject()
{
}

//------------------------------------------------------------------------------

void DebugObject::print( std::ostream &os, fwTools::Object *object, int level )
{
	if ( object==NULL )
	{
		os << std::string(" ",level) << "EMPTY_OBJECT" << std::endl;
		return;
	}

	os << std::string(" ",level) << "ClassName=[" << object->className() << "]" << std::endl;
	os << std::string(" ",level) << "Nb Fields=" << object->children().size() << std::endl;
	os << std::string(" ",level) << "Fields=" << std::endl;
	for  (fwTools::Object::ChildContainer::iterator i= object->children().begin(); i!= object->children().end()  ; ++i )
	{
		if ( ::fwTools::Field::dynamicCast( (*i) ) )
		{
			os <<  std::string(" ",level+2) << "Key=" << (::fwTools::Field::dynamicCast( (*i) )->label());
			os << std::endl;
		}
		DebugObject::print( os, i->get() , level + 2);
		os << std::endl;
	}
}

//------------------------------------------------------------------------------

} // namespace fwData
