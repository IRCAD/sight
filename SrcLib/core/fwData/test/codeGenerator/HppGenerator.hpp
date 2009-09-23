/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_HPPGENERATOR_HPP_
#define _CODEGENERATOR_HPPGENERATOR_HPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

// void setName( const TYPE _typeName );
void generateHppSetTypeA( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << nameMaccroApi << " void set" << methodName << "( const " << type << " " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

// API void setCRefName( const TYPE & _typeName );
void generateHppSetTypeB( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << nameMaccroApi << " void setCRef" << methodName << "( const " << type << " & " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

// API void setName( TYPE _typeName );
void generateHppSetTypeC( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << nameMaccroApi << " void set" << methodName << "( " << type << " " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

// API const type getName() const;
void generateHppGetTypeA( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << nameMaccroApi << " const " << type << " get" << methodName << "() const;" << std::endl;
}

//------------------------------------------------------------------------------

// API const type & getCRefName() const;
void generateHppGetTypeB( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << nameMaccroApi << " const " << type << " & getCRef" << methodName << "() const;" << std::endl;
}

//------------------------------------------------------------------------------

// API type & getRefName();
void generateHppGetTypeC( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << nameMaccroApi << " " << type << " & getRef" << methodName << "();" << std::endl;
}

//------------------------------------------------------------------------------

// API UndefinedReturnType getName() const;
void generateHppGetTypeD( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << nameMaccroApi << " UndefinedReturnType get" << methodName << "() const;" << std::endl;
}

//------------------------------------------------------------------------------

// API type getName();
void generateHppGetTypeE( const std::string & field, const std::string & type, const std::string & nameMaccroApi )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << nameMaccroApi << " " << type << " get" << methodName << "();" << std::endl;
}

//------------------------------------------------------------------------------

void generateHPPSet( const std::string & field, const std::string & type, const std::string & nameMaccroApi)
{
	if ( isASharedPtr(type) )
	{
		// API void setName( TYPE _typeName );
		generateHppSetTypeC( field, type, nameMaccroApi );
	}
	else if ( isAnObject(type) || isAContainair(type) )
	{
		// API void setCRefName( const TYPE & _typeName );
		generateHppSetTypeB( field, type, nameMaccroApi );
		
	}
	else // Simple Object
	{
		// void setName( const TYPE _typeName );
		generateHppSetTypeA( field, type, nameMaccroApi );
		
		// API void setCRefName( const TYPE & _typeName );
		generateHppSetTypeB( field, type, nameMaccroApi );
	}
}

//------------------------------------------------------------------------------

void generateHPPGet( const std::string & field, const std::string & type, const std::string & nameMaccroApi)
{
	if ( isASharedPtr(type) )
	{
		// API UndefinedReturnType getName() const;
		generateHppGetTypeD( field, type, nameMaccroApi );
		
		// API TYPE getName();
		generateHppGetTypeE( field, type, nameMaccroApi );
	}
	else // ( isAnObject(type) || isAContainair(type) || Objet Simple )
	{
		// API const TYPE getName() const;
		generateHppGetTypeA( field, type, nameMaccroApi );
		
		// API const TYPE & getName() const;
		generateHppGetTypeB( field, type, nameMaccroApi );
				
		// API TYPE & getName();
		generateHppGetTypeC( field, type, nameMaccroApi );
	}
}

//------------------------------------------------------------------------------

void generateHPP( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & nameMaccroApi )
{
	
	for( 	std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
			field != fieldAndType.end();
			field ++ )
	{
		std::string name = field->first;
		std::string type = field->second;
		
		std::cout << std::endl;
	
		generateHPPSet(name,type,nameMaccroApi);
		
		generateHPPGet(name,type,nameMaccroApi);
		
	}

}

//------------------------------------------------------------------------------
	
} // codeGenerator


#endif // _CODEGENERATOR_HPPGENERATOR_HPP_
