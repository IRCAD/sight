/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/File.h>

#include <iostream>

#include <arlcore/Misc.h>

arlCore::File::File( const std::string &fullFileName, const std::string &fileType ):
m_fileType(fileType),
m_fullFilename(fullFileName),
m_error(""),
m_version(0.0),
m_position(0),
m_ok(false)
{
    if(m_fullFilename=="") return;
    arlString::splitFilename( m_fullFilename, m_folder, m_filename, m_extension );
    m_file.open(m_fullFilename.c_str(), std::fstream::in);
    if(!m_file.is_open())
    {
        m_file.open(m_filename.c_str(), std::fstream::in);
        if(!m_file.is_open())
        {
            std::stringstream s;
            s<<m_fullFilename<<" doesn't exist\n";
            setError(s.str());
            return;
        }
    }
    m_ok = true;
}

arlCore::File::~File( void )
{
    close();
}

void arlCore::File::close( void )
{
    m_file.close();
}

bool arlCore::File::isOK( void )
{
    return m_ok;
}

bool arlCore::File::getEOF( void )
{
    return m_file.eof();
}


bool arlCore::File::rewind( void )
{
    if(!m_file.is_open()) return false;
    m_file.clear();
    m_file.seekg(0, std::ios::beg);
    return true;
}

bool arlCore::File::recordCurrentPosition( void )
{
    if(!m_file.is_open()) return false;
    m_position = m_file.tellg();
    return true;
}


bool arlCore::File::rewindRecordedPosition( void )
{
    if(!m_file.is_open() || m_position==0) return false;
    m_file.seekg(m_position);
    return true;
}

std::string arlCore::File::getError( void )
{
    return m_error;
}

double arlCore::File::getVersion( void )
{
    return m_version;
}

void arlCore::File::setError( const std::string &error )
{
    const bool Verbose = true;
    m_error = error;
    if(Verbose) std::cerr<<m_error<<"\n";
}

std::string arlCore::File::getFullFilename( void )
{
    return m_fullFilename;
}

std::string arlCore::File::getFilename( void )
{
    return m_filename;
}

std::string arlCore::File::getFolder( void )
{
    return m_folder;
}

std::string arlCore::File::getExtension( void )
{
    return m_extension;
}

double arlCore::File::readHeader( void )
{
    if(!isOK()) return 0.0;
    std::stringstream s;
    std::string token, text;
    unsigned int n=0;
    do
    {
        m_file>>token;
        if(token=="FileType")
        {
            m_file>>text;
            if(text!=m_fileType)
            {
                s<<"Filetype '"<<text<<"' unexpected ("<<m_fileType<<" requested)";
                setError(s.str());
                return m_version;
            }
            n=(n|1);
        }
        if(token=="Version") {m_file>>m_version; n=(n|2);}
        if(m_file.eof())
        {
            s<<"End of file unexpected : "<<m_filename;
            setError(s.str());
            return m_version;
        }
    } while(n!=3);
    return m_version;
}

bool arlCore::File::getToken( std::string& token )
{
    m_file>>token;
    return !m_file.eof();
}

std::string arlCore::File::getString( void )
{
    std::string value;
    m_file>>value;
    return value;
}

bool arlCore::File::getString( std::string &value )
{
    m_file>>value;
    return true;
}

std::string arlCore::File::getPath( void )
{
    std::stringstream s;
    std::string value;
    m_file>>value;
    s<<getFolder()<<"/"<<value;
    return s.str();
}

std::fstream& arlCore::File::getStream( void )
{
    return m_file;
}

std::string arlCore::File::getFiletype( const std::string &fileName )
{
    std::string token, fileType = "";
    std::fstream file;
    file.open(fileName.c_str(), std::fstream::in);
    if(file.is_open())
    {
        file>>token;
        if(token=="FileType")
            file>>fileType;
    }
    return fileType;
}
