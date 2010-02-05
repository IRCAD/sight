/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_MFOTODATACONVERTGENERATOR_HPP_
#define _CODEGENERATOR_MFOTODATACONVERTGENERATOR_HPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

//      TYPE _param = mfoPatient->GetParam();
void generateMfoToDataConvertGetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
        std::string methodName = fieldToMethodName( field );
        std::string paramName = fieldToParamName( field );
        
        std::cout << type << " " << paramName << " = mfo" << className << "->Get" << methodName << "();" << std::endl; 
}
//------------------------------------------------------------------------------

//      A
//      dataPatient->setName(name);
void generateMfoToDataConvertSetTypeA( const std::string & field, const std::string & type, const std::string & className )
{
        std::string methodName = fieldToMethodName( field );
        std::string paramName = fieldToParamName( field );
        
        std::cout << "data" << className << "->set" << methodName << "( " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

//      B
//      dataPatient->setCRefName(name);
void generateMfoToDataConvertSetTypeB( const std::string & field, const std::string & type, const std::string & className )
{
        std::string methodName = fieldToMethodName( field );
        std::string paramName = fieldToParamName( field );
        
        std::cout << "data" << className << "->setCRef" << methodName << "( " << paramName << " );" << std::endl;
}

//------------------------------------------------------------------------------

void generateMfoToDataConvertSet( const std::string & field, const std::string & type, const std::string & parameterName)
{
        if ( isASharedPtr(type) )
        {
                generateMfoToDataConvertSetTypeA( field, type, parameterName );
        }
        else if ( isAnObject(type) || isAContainair(type) )
        {
                generateMfoToDataConvertSetTypeB( field, type, parameterName );
        }
        else
        {
                generateMfoToDataConvertSetTypeA( field, type, parameterName );
        }
}

//------------------------------------------------------------------------------

void generateMfoToDataConvertGet( const std::string & field, const std::string & type, const std::string & parameterName)
{
        if ( isASharedPtr(type) )
        {
                generateMfoToDataConvertGetTypeA( field, type, parameterName );
        }
        else if ( isAnObject(type) || isAContainair(type) )
        {
                generateMfoToDataConvertGetTypeA( field, type, parameterName );
        }
        else
        {
                generateMfoToDataConvertGetTypeA( field, type, parameterName );
        }
}

//------------------------------------------------------------------------------

void generateMfoToDataConvert( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & parameterName )
{
        
        for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
                        field != fieldAndType.end();
                        field ++ )
        {
                std::string name = field->first;
                std::string type = field->second;
        
                generateMfoToDataConvertGet(name,type,parameterName);
        }

        for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
                        field != fieldAndType.end();
                        field ++ )
        {
                std::string name = field->first;
                std::string type = field->second;
        
                generateMfoToDataConvertSet(name,type,parameterName);
        }
}

//------------------------------------------------------------------------------
        
} // codeGenerator


#endif // _CODEGENERATOR_MFOTODATACONVERTGENERATOR_HPP_
