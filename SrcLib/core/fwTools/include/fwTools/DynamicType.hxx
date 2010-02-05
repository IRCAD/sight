/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <boost/mpl/vector.hpp>

#include "fwTools/StringKeyTypeMapping.hpp"
#include "fwTools/Dispatcher.hpp"

namespace fwTools {


template<class TYPE>
DynamicType makeDynamicType()
{
	DynamicType d;
	d.template setType<TYPE>();
	return d;
}




struct TypeSetter
{
	template<class TYPE>
	void operator()( DynamicType & dynamicType )
	{
		dynamicType.setType<TYPE>();
	}
};



template<class KEYTYPE>
DynamicType makeDynamicType(const KEYTYPE &keyType)
{
	typedef boost::mpl::vector< signed char, unsigned char, signed short, unsigned short,  signed int, unsigned int, float, double >:: type SupportedTypes;

	DynamicType d;

	Dispatcher<SupportedTypes,TypeSetter >::invoke(keyType,d);

	assert ( d != DynamicType() ); // a type must be found !!

	return d;
}





template< class TYPE>
void DynamicType::setType() throw(std::invalid_argument)
{
	 std::list< std::string>::const_iterator  supportedTypesIter;

	 supportedTypesIter = m_managedTypes.begin();
	 while ( supportedTypesIter !=  m_managedTypes.end() )
	 {
		 if ( isMapping<TYPE>( *supportedTypesIter) )
		 {
			 m_value  = *supportedTypesIter;
			 m_sizeof = sizeof(TYPE);
			 return;

		 }
		 ++supportedTypesIter;
	 }

	 throw std::invalid_argument("DynamicType::setType<TYPE> incorrect TYPE");
}




template< class TYPE>
bool DynamicType::isType() const
{
	return isMapping<TYPE>(m_value);
}




template<class TYPE>
const std::string DynamicType::string()
{
	DynamicType d;
	d.setType<TYPE>();
	return d.string();
}




template<class NEWTYPE>
void DynamicType::registerNewType(const std::string &newKey) throw(std::invalid_argument)
{
	// ensure isMapping present and well defined
	// if prog trap here it is because is Mapping is not well defined !!!
	if ( isMapping<NEWTYPE>(newKey) == false )
	{
		throw std::invalid_argument("Dynamic::registerNewType misconception with isMapping");
	}

	// ensure that newKey is not already used
	if ( std::find( m_managedTypes.begin(),m_managedTypes.end(), newKey ) != m_managedTypes.end() )
	{
		throw std::invalid_argument("Dynamic::registerNewType newKey already used");
	}

	// ensure that no other mapping respond to newkey
	try
	{
		DynamicType dummy;
		dummy.setType<NEWTYPE>();
	}
	catch ( std::exception )
	{
		// OK it is really a new type, insert it
		m_managedTypes.push_back(newKey);
		return;
	}

	throw std::invalid_argument("Dynamic::registerNewType another isMapping is responding");
}


} //end namespace fwTools {
