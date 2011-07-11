/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Tags.h>

#include <arlcore/Misc.h>

//const long int ARLCORE_TAG_RANGES[2]={1000,100000};
//const unsigned int MAXNBMARKERS=10;

arlCore::Tags::Tags( arlCore::PlaneSystem &universe, const std::string &name ):
Object(arlCore::ARLCORE_CLASS_TAGS,"Unknown PLANESYSTEM"),
m_universe(universe)
{
    setName(name);
    setOK(true);
}

arlCore::Tags::Tags( const Tags& t ):
Object(t),
m_universe(t.m_universe)
{
    copy(t);
}

arlCore::Tags& arlCore::Tags::operator=( const Tags& t )
{
    copy(t);
    return *this;
}

arlCore::Tags::~Tags( void )
{
    reset();
}

bool arlCore::Tags::reset( void )
{
    unsigned int i;
    for( i=0 ; i<m_tags.size() ; ++i )
    {
        assert(m_tags[i]!=0);
        delete(m_tags[i]);
    }
    m_tags.clear();
    return true;
}

bool arlCore::Tags::copy(  const Tags& t )
{
    if(this==&t) return false;
    reset();
    arlCore::Object *a=this;
    const arlCore::Object *b=&t;
    *a = *b;
    unsigned int i;
    for( i=0 ; i<t.m_tags.size() ; ++i )
        m_tags.push_back(new arlCore::Tag(*t.m_tags[i]));
    return true;
}

unsigned int arlCore::Tags::size( void ) const
{
    return (unsigned int)m_tags.size();
}

std::string arlCore::Tags::getString( void ) const
{
    std::stringstream s;
    unsigned int i;
    s<<this->Object::getString();
    for( i=0 ; i<m_tags.size() ; ++i )
        s<<m_tags[i]->getString();
    return s.str();
}

bool arlCore::Tags::load( const std::string &fileName )
{
    if(fileName=="") return false;
    return false;
}

bool arlCore::Tags::save( const std::string &fileName, bool overwrite ) const
{
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    // TODO
    return false;
}

const arlCore::Tag* arlCore::Tags::getTag( unsigned int index ) const
{
    assert(index>=0 && index<m_tags.size());
    if(index<0 || index>=m_tags.size()) return 0;
    else return m_tags[index];
}

arlCore::Tag* arlCore::Tags::getTag( unsigned int index )
{
    assert(index>=0 && index<m_tags.size());
    if(index<0 || index>=m_tags.size()) return 0;
    else return m_tags[index];
}

bool arlCore::Tags::delTag( const arlCore::Tag* T )
{
    if(T==0) return false;
    std::vector< arlCore::Tag* >::iterator it=m_tags.begin();
    while( it!=m_tags.end() )
        if(*it==T)
        {
            delete (T);
            m_tags.erase(it);
            return true;
        }else ++it;
    return false;
}

/*const arlCore::Tag* arlCore::Tags::addTag( const std::string &fileName )
{
    arlCore::Tag *T=new arlCore::Tag(m_universe, fileName);
    if(addTag(T)==0)
    {
        delete T;
        m_log<<"Loading Tag \""<<fileName<<" failed";
        log(ARL_LOG_WARNING);
        return 0;
    }
    m_log<<"Tag \""<<fileName<<" is loaded";
    log(ARL_LOG_INFO);
    return T;
}*/

arlCore::Tag* arlCore::Tags::addTag( const arlCore::PointList& pl )
{
    arlCore::Tag *T=new arlCore::Tag(m_universe, pl);
    if(addTag(T)==0)
    {
        delete T;
        return 0;
    }
    return T;
}

const arlCore::Tag* arlCore::Tags::addTag( arlCore::Tag *T )
{
    bool b=(T!=0);
    if(b) b=T->isOK();
    if(!b) return 0;
    m_tags.push_back(T);
    return T;
}
