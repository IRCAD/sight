/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_CPPGENERATOR_CPP_
#define _CODEGENERATOR_CPPGENERATOR_CPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

void generateCPPSetCorpse( const std::string & field )
{
	std::string paramName = fieldToParamName( field );
	std::cout << "{" << std::endl;
	std::cout << "\t" << field << " = " << paramName << ";" << std::endl;
	std::cout << "}" << std::endl;
}

//------------------------------------------------------------------------------

void generateCPPGetCorpse( const std::string & field )
{
	std::cout << "{" << std::endl;
	std::cout << "\treturn " << field << ";" << std::endl;
	std::cout << "}" << std::endl;
}

//------------------------------------------------------------------------------

void generateCPPSeparator()
{
	std::cout << std::endl;
	std::cout << "//------------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}

//------------------------------------------------------------------------------

// void setName( const TYPE _typeName );
void generateCPPSetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << "void " << className << "::set" << methodName << "( const " << type << " " << paramName << " )" << std::endl;
	generateCPPSetCorpse( field );
}

//------------------------------------------------------------------------------

// API void setCRefName( const TYPE & _typeName );
void generateCPPSetTypeB( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << "void " << className << "::setCRef" << methodName << "( const " << type << " & " << paramName << " )" << std::endl;
	generateCPPSetCorpse( field );
}

//------------------------------------------------------------------------------

// API void setName( TYPE _typeName );
void generateCPPSetTypeC( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::string paramName = fieldToParamName( field );
	std::cout << "void " << className << "::set" << methodName << "( " << type << " " << paramName << " )" << std::endl;
	generateCPPSetCorpse( field );
}

//------------------------------------------------------------------------------

// API const type getName() const;
void generateCPPGetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << "const " << type << " " << className << "::get" << methodName << "() const" << std::endl;
	generateCPPGetCorpse( field );
}

//------------------------------------------------------------------------------

// API const type & getCRefName() const;
void generateCPPGetTypeB( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << "const " << type << " & " << className << "::getCRef" << methodName << "() const" << std::endl;
	generateCPPGetCorpse( field );
}

//------------------------------------------------------------------------------

// API type & getRefName();
void generateCPPGetTypeC( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << type << " & " << className << "::getRef" << methodName << "()" << std::endl;
	generateCPPGetCorpse( field );
}

//------------------------------------------------------------------------------

// API UndefinedReturnType getName() const;
void generateCPPGetTypeD( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << "UndefinedReturnType " << className << "::get" << methodName << "() const" << std::endl;
	generateCPPGetCorpse( field );
}

//------------------------------------------------------------------------------

// API type getName();
void generateCPPGetTypeE( const std::string & field, const std::string & type, const std::string & className )
{
	std::string methodName = fieldToMethodName( field );
	std::cout << type << " " << className << "::get" << methodName << "()" << std::endl;
	generateCPPGetCorpse( field );
}

//------------------------------------------------------------------------------

void generateCPPSet( const std::string & field, const std::string & type, const std::string & className)
{
	if ( isASharedPtr(type) )
	{
		// API void setName( TYPE _typeName );
		generateCPPSetTypeC( field, type, className );
		generateCPPSeparator();
	}
	else if ( isAnObject(type) || isAContainair(type) )
	{
		// API void setCRefName( const TYPE & _typeName );
		generateCPPSetTypeB( field, type, className );
		generateCPPSeparator();
		
	}
	else // Simple Object
	{
		// void setName( const TYPE _typeName );
		generateCPPSetTypeA( field, type, className );
		generateCPPSeparator();
		
		// API void setCRefName( const TYPE & _typeName );
		generateCPPSetTypeB( field, type, className );
		generateCPPSeparator();
	}
}

//------------------------------------------------------------------------------

void generateCPPGet( const std::string & field, const std::string & type, const std::string & className)
{
	if ( isASharedPtr(type) )
	{
		// API UndefinedReturnType getName() const;
		generateCPPGetTypeD( field, type, className );
		generateCPPSeparator();
		
		// API TYPE getName();
		generateCPPGetTypeE( field, type, className );
		generateCPPSeparator();
	}
	else // ( isAnObject(type) || isAContainair(type) || Objet Simple )
	{
		// API const TYPE getName() const;
		generateCPPGetTypeA( field, type, className );
		generateCPPSeparator();
		
		// API const TYPE & getName() const;
		generateCPPGetTypeB( field, type, className );
		generateCPPSeparator();
				
		// API TYPE & getName();
		generateCPPGetTypeC( field, type, className );
		generateCPPSeparator();
	}
}

//------------------------------------------------------------------------------

void generateCPP( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & className )
{
	
	for( 	std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
			field != fieldAndType.end();
			field ++ )
	{
		std::string name = field->first;
		std::string type = field->second;
	
		generateCPPSet(name,type,className);
		
		generateCPPGet(name,type,className);
		
	}

}

//------------------------------------------------------------------------------
	
} // codeGenerator


#endif // _CODEGENERATOR_CPPGENERATOR_CPP_
