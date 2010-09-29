/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_DATATOMFOCONVERTGENERATOR_HPP_
#define _CODEGENERATOR_DATATOMFOCONVERTGENERATOR_HPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

//  TYPE _param = dataPatient->getCRefParam();
void generateDataToMfoConvertGetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
    std::string methodName = fieldToMethodName( field );
    std::string paramName = fieldToParamName( field );
    
    std::cout << type << " " << paramName << " = data" << className << "->getCRef" << methodName << "();" << std::endl; 
}

//------------------------------------------------------------------------------

//  TYPE _param = dataPatient->getParam();
void generateDataToMfoConvertGetTypeB( const std::string & field, const std::string & type, const std::string & className )
{
    std::string methodName = fieldToMethodName( field );
    std::string paramName = fieldToParamName( field );
    
    std::cout << type << " " << paramName << " = data" << className << "->get" << methodName << "();" << std::endl; 
}

//------------------------------------------------------------------------------

//  TYPE _param = dataPatient->getConstParam();
void generateDataToMfoConvertGetTypeC( const std::string & field, const std::string & type, const std::string & className )
{
    std::string methodName = fieldToMethodName( field );
    std::string paramName = fieldToParamName( field );
    
    std::cout << type << " " << paramName << " = data" << className << "->getCRef" << methodName << "();" << std::endl; 
}

//------------------------------------------------------------------------------

//  A
//  dataPatient->setName(name);
void generateDataToMfoConvertSetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
    std::string methodName = fieldToMethodName( field );
    std::string paramName = fieldToParamName( field );
    
    std::cout << "mfo" << className << "->Set" << methodName << "( " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

void generateDataToMfoConvertSet( const std::string & field, const std::string & type, const std::string & parameterName)
{
    if ( isASharedPtr(type) )
    {
        generateDataToMfoConvertSetTypeA( field, type, parameterName );
    }
    else if ( isAnObject(type) || isAContainair(type) )
    {
        generateDataToMfoConvertSetTypeA( field, type, parameterName );
    }
    else
    {
        generateDataToMfoConvertSetTypeA( field, type, parameterName );
    }
}

//------------------------------------------------------------------------------

void generateDataToMfoConvertGet( const std::string & field, const std::string & type, const std::string & parameterName)
{
    if ( isASharedPtr(type) )
    {
        generateDataToMfoConvertGetTypeC( field, type, parameterName );
    }
    else if ( isAnObject(type) || isAContainair(type) )
    {
        generateDataToMfoConvertGetTypeA( field, type, parameterName );
    }
    else
    {
        generateDataToMfoConvertGetTypeB( field, type, parameterName );
    }
}

//------------------------------------------------------------------------------

void generateDataToMfoConvert( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & parameterName )
{
    
    for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
            field != fieldAndType.end();
            field ++ )
    {
        std::string name = field->first;
        std::string type = field->second;
    
        generateDataToMfoConvertGet(name,type,parameterName);
    }

    for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
            field != fieldAndType.end();
            field ++ )
    {
        std::string name = field->first;
        std::string type = field->second;
    
        generateDataToMfoConvertSet(name,type,parameterName);
    }
}

//------------------------------------------------------------------------------
    
} // codeGenerator


#endif // _CODEGENERATOR_DATATOMFOCONVERTGENERATOR_HPP_
