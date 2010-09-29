/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CODEGENERATOR_FIELDPARSER_HPP_
#define _CODEGENERATOR_FIELDPARSER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <string>

#include <fwCore/base.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

namespace codeGenerator
{

//------------------------------------------------------------------------------

bool isAContainair( const std::string & type )
{
    std::vector< std::string > keys;
    keys.push_back("vector");
    keys.push_back("std::vector");
    keys.push_back("list");
    keys.push_back("std::list");
    keys.push_back("map");
    keys.push_back("std::map");
    keys.push_back("pair");
    keys.push_back("std::pair");
        
    for (   std::vector< std::string >::const_iterator str = keys.begin();
            str != keys.end();
            ++str ) 
    {
        std::stringstream reg;
        reg << "([ \t]*)" << *str << "(.*)";
        boost::regex e (reg.str()); 
        if ( regex_match(type, e) )
        {
            OSLM_DEBUG( type << " is a container.");
            return true;
        }
    }
            
    return false;
}

//------------------------------------------------------------------------------

bool isASharedPtr( const std::string & type )
{
    std::vector< std::string > keys;
    keys.push_back("boost::shared_ptr");
    keys.push_back("shared_ptr");
        
    for (   std::vector< std::string >::const_iterator str = keys.begin();
            str != keys.end();
            ++str ) 
    {
        std::stringstream reg;
        reg << "([ \t]*)" << *str << "(.*)";
        boost::regex e (reg.str()); 
        if ( regex_match(type, e) )
        {
            OSLM_DEBUG( type << " is a shared pointer.");
            return true;
        }
    }
            
    return false;
}

//------------------------------------------------------------------------------

bool isAnObject( const std::string & type )
{
    std::vector< std::string > keys;
    
    keys.push_back("std::string");
    keys.push_back("string");
    
    keys.push_back("boost::filesystem::path");
    keys.push_back("filesystem::path");
    keys.push_back("path");
    
    keys.push_back("boost::posix_time::ptime");
    keys.push_back("posix_time::ptime");
    keys.push_back("ptime");
        
    for (   std::vector< std::string >::const_iterator str = keys.begin();
            str != keys.end();
            ++str ) 
    {
        std::stringstream reg;
        reg << "([ \t]*)" << *str << "(.*)";
        boost::regex e (reg.str()); 
        if ( regex_match(type, e) )
        {
            OSLM_DEBUG( type << " is an object.");
            return true;
        }
    }
            
    return false;
}

//------------------------------------------------------------------------------

std::string fieldToMethodName( const std::string & field )
{
    boost::regex e ("m_[0-9a-z]+([0-9a-zA-Z]+)");
    std::string format_name = "\\1";
    std::string name = regex_replace(field, e, format_name, boost::match_default | boost::format_sed);
    return name;
}

//------------------------------------------------------------------------------

std::string fieldToLittleMethodName( const std::string & field )
{
    boost::regex e ("m_[0-9a-z]+([0-9a-zA-Z]+)");
    std::string format_name = "\\1";
    std::string name = regex_replace(field, e, format_name, boost::match_default | boost::format_sed);
    name[0] = tolower(name[0]);
    return name;
}

//------------------------------------------------------------------------------

std::string fieldToParamName( const std::string & field )
{
    boost::regex e ("m(_[0-9a-z]+[0-9a-zA-Z]+)");
    std::string format_name = "\\1";
    std::string name = regex_replace(field, e, format_name, boost::match_default | boost::format_sed);
    return name;
}

//------------------------------------------------------------------------------

bool isValidLine( const std::string & line )
{
    boost::regex emptyLine ("([\t \n]*)");
    boost::regex commentLine1 ("([\t ]*)/(.*)");
    
    return  ! regex_match(line, emptyLine) &&
            ! regex_match(line, commentLine1);
}

//------------------------------------------------------------------------------

void interpretLine(const std::string & line , std::vector< std::pair< std::string, std::string > > & fieldAndType )
{
    boost::regex e ("[\t ]*([_0-9a-zA-Z<:> ,]+)[ \t]+(m_[0-9a-zA-Z]+)[ \t]*;[ \t]*");
    std::string format_type = "\\1";
    std::string format_name = "\\2";
    std::string type = regex_replace(line, e, format_type, boost::match_default | boost::format_sed);
    std::string name = regex_replace(line, e, format_name, boost::match_default | boost::format_sed);
        
    OSLM_DEBUG("TYPE = " << type);
    OSLM_DEBUG("NAME = " << name);
    std::pair < std::string, std::string > p (name,type);
    fieldAndType.push_back(p);
}

//------------------------------------------------------------------------------

std::vector< std::pair< std::string, std::string > >
fieldParser( const std::string & pathFile)
{
    std::vector< std::pair< std::string, std::string > > fieldAndType;
    
    const int lineSize = 200;
    char line [lineSize];

    std::ifstream infile ( pathFile.c_str(), std::ios::in );
    while( infile.getline (line, lineSize, '\n') )
    {
        SLM_DEBUG(line);
        if ( isValidLine(line) )
        {
            interpretLine( line, fieldAndType );
        }
        else
        {
            SLM_DEBUG("Not a good line");
        }
    }
    
    infile.close();
    
    return fieldAndType;
}

//------------------------------------------------------------------------------

void printFields(const std::vector< std::pair< std::string, std::string > > & fieldAndType)
{
    SLM_DEBUG("PRINT FIELDS");
    for(    std::vector < std::pair < std::string, std::string > >::const_iterator field = fieldAndType.begin();
            field != fieldAndType.end();
            field++ )
    {
        OSLM_INFO(" name = " << field->first << " et type = " << field->second);
    }
}

//------------------------------------------------------------------------------

} // codeGenerator


#endif // _CODEGENERATOR_FIELDPARSER_HPP_
