/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_LOADGENERATOR_HPP_
#define _CODEGENERATOR_LOADGENERATOR_HPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

//  A
//  std::string _sBirthdate;
//  ar &  boost::serialization::make_nvp( "Birthdate" , _sBirthdate );
//  _patient.setBirthdate(_sBirthdate);
void generateLoadTypeA( const std::string & field, const std::string & type, const std::string & parameterName )
{
    std::string methodName = fieldToMethodName( field );
    std::cout << "ar &  boost::serialization::make_nvp( \"";
    std::cout << methodName << "\" , " << parameterName << ".get" <<  methodName;
    std::cout << "() );" << std::endl;
}

//------------------------------------------------------------------------------

//  B
//  std::string _sBirthdate;
//  ar &  boost::serialization::make_nvp( "Birthdate" , _sBirthdate );
//  _patient.setCRefBirthdate(_sBirthdate);
void generateLoadTypeB( const std::string & field, const std::string & type, const std::string & parameterName )
{
    std::string methodName = fieldToMethodName( field );
    std::cout << "ar &  boost::serialization::make_nvp( \"";
    std::cout << methodName << "\" , " << parameterName << ".getRef" <<  methodName;
    std::cout << "() );" << std::endl;
}

//------------------------------------------------------------------------------

void generateLoad( const std::string & field, const std::string & type, const std::string & parameterName)
{
    if ( isASharedPtr(type) )
    {
        generateLoadTypeA( field, type, parameterName );
    }
    else if ( isAnObject(type) || isAContainair(type) )
    {
        generateLoadTypeB( field, type, parameterName );
    }
    else
    {
        generateLoadTypeB( field, type, parameterName );
    }
}

//------------------------------------------------------------------------------

void generateLoad( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & parameterName )
{
    
    for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
            field != fieldAndType.end();
            field ++ )
    {
        std::string name = field->first;
        std::string type = field->second;
    
        generateLoad(name,type,parameterName);
        
    }

}

//------------------------------------------------------------------------------
    
} // codeGenerator


#endif // _CODEGENERATOR_LOADGENERATOR_HPP_
