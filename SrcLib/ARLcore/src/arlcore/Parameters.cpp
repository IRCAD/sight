/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Parameters.h>

#include <sstream>
#include <fstream>
#include <iostream>

arlCore::Parameters::Parameters( const std::string &name, const std::string &fileName ):
m_name(name),
m_paramFile(fileName)
{}

arlCore::Parameters::~Parameters( void )
{
    clear();
}


void arlCore::Parameters::clear( void )
{
    m_paramNames.clear();
    m_paramValues.clear();
    m_paramDefaults.clear();
    m_index.clear();
}

bool arlCore::Parameters::printParameters( void ) const
{
    std::cout<<getString();
    return true;
}

bool arlCore::Parameters::reloadParameters( void )
{
    return load()>0;
}

bool arlCore::Parameters::reloadParameters( const std::string &fileName )
{
    m_paramFile = fileName;
    return load()>0;
}

std::string arlCore::Parameters::getString( void ) const
{
    // TODO Afficher le type et (true,false) si booleen
    unsigned int i;
    std::stringstream ss;
    ss<<"_______________________________________________________________________________\n\n";
    ss<<"Parameter list '"<<m_name<<"' : "<<getNbParameters()<<" parameters\n";
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(double) )
            ss<<" "<<m_paramNames[i]<<" = "<<boost::any_cast<double>(m_paramValues[i])<<" [Dft:"<<boost::any_cast<double>(m_paramDefaults[i])<<"]\n";
        else
            if( m_paramValues[i].type() == typeid(bool) )
                ss<<" "<<m_paramNames[i]<<" = "<<boost::any_cast<bool>(m_paramValues[i])<<" [Dft:"<<boost::any_cast<bool>(m_paramDefaults[i])<<"]\n";
            else ss<<" Unknown type\n";
    ss<<"_______________________________________________________________________________\n";
    std::string s = ss.str();
    return s;
}

bool arlCore::Parameters::getType( unsigned int no, const std::type_info &type ) const
{
    assert( no<m_paramValues.size() );
    if( no>=m_paramValues.size() ) return false;
    return (m_paramValues[no].type()==type);
}

bool arlCore::Parameters::getIndex( const std::string &name, unsigned int &index ) const
{
    std::map< std::string, unsigned int >::const_iterator it;
    it = m_index.find(name);
    assert( it!=m_index.end() );
    if( it==m_index.end() ) return false;
    index = it->second;
    return true;
}

unsigned int arlCore::Parameters::getNbParameters( void ) const
{
    return (unsigned int)m_paramValues.size();
}

unsigned int arlCore::Parameters::getNbDoubleParameters( void ) const
{
    unsigned int i, n=0;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(double) ) ++n;
    return n;
}

unsigned int arlCore::Parameters::getNbBoolParameters( void ) const
{
    unsigned int i, n=0;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(bool) ) ++n;
    return n;
}

bool arlCore::Parameters::saveParameters( const std::string &fileName, bool overwrite ) const
{
    //! @todo
    return false;
}

unsigned int arlCore::Parameters::setAllDefault( void )
{
    unsigned int i;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        m_paramValues[i] = m_paramDefaults[i];
    return i;
}

unsigned int arlCore::Parameters::setAllBoolParameters( bool v )
{
    unsigned int i;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(bool) )
            m_paramValues[i] = v;
    return i;
}

unsigned int arlCore::Parameters::resetAllBoolParameters( void )
{
    unsigned int i;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(bool) )
            m_paramValues[i] = false;
    return i;
}

unsigned int arlCore::Parameters::setAllDoubleParameters( double v )
{
    unsigned int i;
    for( i=0 ; i<m_paramValues.size() ; ++i )
        if( m_paramValues[i].type() == typeid(double) )
            m_paramValues[i] = v;
    return i;
}

//  protected:
bool arlCore::Parameters::getBool( unsigned int no ) const
{
    if( !getType( no, typeid(bool) ) ) return false;
    return boost::any_cast<bool>(m_paramValues[no]);
}

bool arlCore::Parameters::getBool( const std::string &name ) const
{
    unsigned int no;
    if( !getIndex(name, no) ) return false;
    if( !getType( no, typeid(bool) ) ) return false;
    return boost::any_cast<bool>(m_paramValues[no]);
}

double arlCore::Parameters::getDouble( unsigned int no ) const
{
    if( !getType( no, typeid(double) ) ) return 0.0;
    return boost::any_cast<double>(m_paramValues[no]);
}

double arlCore::Parameters::getDouble( const std::string &name ) const
{
    unsigned int no;
    if( !getIndex(name, no) ) return 0.0;
    if( !getType( no, typeid(double) ) ) return 0.0;
    return boost::any_cast<double>(m_paramValues[no]);
}

bool arlCore::Parameters::setBool( unsigned int no, bool value )
{
    if( !getType( no, typeid(bool) ) ) return false;
    m_paramValues[no] = value;
    return true;
}

bool arlCore::Parameters::setBool( const std::string &name, bool value )
{
    unsigned int no;
    if( !getIndex(name, no) ) return false;
    if( !getType( no, typeid(bool) ) ) return false;
    m_paramValues[no] = value;
    return true;
}

bool arlCore::Parameters::setDouble( unsigned int no, double value )
{
    if( !getType( no, typeid(double) ) ) return false;
    m_paramValues[no] = value;
    return true;
}

bool arlCore::Parameters::setDouble( const std::string &name, double value )
{
    unsigned int no;
    if( !getIndex(name, no) ) return false;
    if( !getType( no, typeid(double) ) ) return false;
    m_paramValues[no] = value;
    return true;
}

unsigned int arlCore::Parameters::load( void )
{
    clear();
    if(m_paramFile=="") return 0;
    unsigned int n=0;
    std::ifstream file;
    file.open (m_paramFile.c_str(), std::fstream::in);
    if(!file.is_open())
    {
        std::cerr<<"Parameters Loading error : "<<m_paramFile<<"\n";
        return n;
    }
    std::string name, type, desc;
    bool valueBool;
    double valueDouble;
    boost::any value;
    while(!file.eof())
    {
        file>>name>>type;
        if(type=="bool")
        {
            file>>valueBool;
            m_paramValues.push_back(valueBool);
            m_paramDefaults.push_back(valueBool);
        }
        if(type=="double")
        {
            file>>valueDouble;
            m_paramValues.push_back(valueDouble);
            m_paramDefaults.push_back(valueDouble);
        }
        m_index[name]=n;
        std::getline(file, desc,'>');
        std::getline(file, desc);
        //file>>desc;
        m_paramNames.push_back(desc);
        ++n;
    }
    file.close();
    return n;
}

unsigned int arlCore::Parameters::init( const std::string names[], const boost::any values[], unsigned int nb )
{
    unsigned int i;
    for( i=0 ; i<nb ; ++i )
    {
        m_paramNames.push_back(names[i]);
        m_paramValues.push_back(values[i]);
        m_paramDefaults.push_back(values[i]);
    }
    return i;
}
