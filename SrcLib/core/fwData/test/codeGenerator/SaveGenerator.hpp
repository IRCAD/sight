/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_SAVEGENERATOR_HPP_
#define _CODEGENERATOR_SAVEGENERATOR_HPP_

#include <string>

#include "FieldParser.hpp"

namespace codeGenerator
{

//------------------------------------------------------------------------------

// A : ar &  boost::serialization::make_nvp( "Param" , _patient.getParam() );
void generateSaveTypeA( const std::string & field, const std::string & parameterName )
{
    std::string methodName = fieldToMethodName( field );
    std::cout << "ar &  boost::serialization::make_nvp( \"";
    std::cout << methodName << "\" , " << parameterName << ".get" <<  methodName;
    std::cout << "() );" << std::endl;
}

//------------------------------------------------------------------------------

// A : ar &  boost::serialization::make_nvp( "Param" , _patient.getCRefParam() );
void generateSaveTypeB( const std::string & field, const std::string & parameterName )
{
    std::string methodName = fieldToMethodName( field );
    std::cout << "ar &  boost::serialization::make_nvp( \"";
    std::cout << methodName << "\" , " << parameterName << ".getCRef" <<  methodName;
    std::cout << "() );" << std::endl;
}

//------------------------------------------------------------------------------

void generateSave( const std::string & field, const std::string & type, const std::string & parameterName)
{
    if ( isASharedPtr(type) )
    {
        generateSaveTypeA( field, parameterName );
    }
    else // if ( isAnObject(type) || isAContainair(type) ) & simple object
    {
        generateSaveTypeB( field, parameterName );
    }
}

//------------------------------------------------------------------------------

void generateSave( const std::vector< std::pair< std::string, std::string > > & fieldAndType, const std::string & parameterName )
{
    
    for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
            field != fieldAndType.end();
            field ++ )
    {
        std::string name = field->first;
        std::string type = field->second;
    
        generateSave(name,type,parameterName);
        
    }

}

//------------------------------------------------------------------------------
    
} // codeGenerator


#endif // _CODEGENERATOR_SAVEGENERATOR_HPP_
