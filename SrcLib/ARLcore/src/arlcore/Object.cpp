/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Object.h>

#include <time.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <arlcore/Misc.h>

unsigned int arlCore::Object::m_counter[arlCore::ARLCORE_CLASS_NBTYPES];
arlCore::ARLCORE_LOG_VERBOSE arlCore::Object::m_staticVerboseLevel=arlCore::ARLCORE_LOG_VERBOSE_NBTYPES;

arlCore::Object::Object( ARLCORE_CLASS c, const std::string &name ):
m_updateIndex( FIRSTUPDATEINDEX ),
m_date( 0 ),
m_time( 0 ),
m_class( c ),
m_name(name),
m_ok( false ),
m_writeMutex( false ),
m_readMutex( 0 ),
m_verboseLevel( ARLCORE_LOG_MUTE )
{
    m_no=++m_counter[m_class];
}

arlCore::Object::Object( const Object& o ):
m_writeMutex( false ),
m_readMutex( 0 ),
m_verboseLevel( ARLCORE_LOG_MUTE )
{
    copy(o);
    m_no=++m_counter[m_class];
}

arlCore::Object& arlCore::Object::operator=( const Object& o )
{
    copy(o);
    return *this;
}

void arlCore::Object::copy( const Object& o )
{
    if(this!=&o)
    {
        m_verboseLevel=o.m_verboseLevel;
        m_date=o.m_date;
        m_time=o.m_time;
        m_class=o.m_class;
        m_name=o.m_name;
        m_ok=o.m_ok;
        m_updateIndex+=o.m_updateIndex;
    }
}

arlCore::Object::~Object( void )
{
    assert(m_class<ARLCORE_CLASS_NBTYPES);
    --m_counter[m_class];
}

arlCore::ARLCORE_CLASS arlCore::Object::getType( void ) const
{
    return m_class;
}

bool arlCore::Object::print( void ) const
{
    std::string s = getString(); 
    std::cout<<s;
    return true;
}

std::string arlCore::Object::getString( void ) const
{
    std::string c="UNKNOWN";
    if(m_class<ARLCORE_CLASS_NBTYPES)
        c=ARLCORE_CLASS_NAMES[m_class];
    std::stringstream s;
    s<<"_______________________________________________________________________________\n";
    s<<"\""<<m_name<<"\" "<<c<<" class (#"<<m_no<<" / "<<m_counter[m_class]<<" object";
    if(m_counter[m_class]>1) s<<"s";
    s<<")";
    if(m_ok) s<<"\nOK"; else s<<"\nKO";
    s<<" - Timestamp : "<<m_date<<"-"<<m_time<<" ("<<this<<")";
//  s<<" - Timestamp : "<<m_time<<" ("<<this<<")";
    s<<"\nVerbose level : "<<m_verboseLevel;
    s<<" - Update index : "<<m_updateIndex;
    s<<"\nMutex : Read("<<m_readMutex<<") Write("<<m_writeMutex<<")\n";
    s<<"_______________________________________________________________________________\n";
    return s.str();
}

bool arlCore::Object::load( const std::string &fileName )
{
    return false;
}

bool arlCore::Object::save( const std::string &fileName, bool overwrite ) const
{
    if(arlFile::fileExist(fileName) && !overwrite) return false;
    return false;
}

bool arlCore::Object::save( void ) const
{
    return save( getFileName() );
}

std::string arlCore::Object::serialize( void ) const
{
    const unsigned int Precision = 15;
    std::stringstream ss;
    ss<<std::setprecision(Precision)<<m_ok<<" "<<m_date<<" "<<m_time<<" ";
    return ss.str();
}

bool arlCore::Object::unserialize( std::stringstream& s )
{
    s>>m_ok>>m_date>>m_time;
    update();
    return true;
}
bool arlCore::Object::unserialize( const std::string &s )
{
    const unsigned int Precision = 15;
    std::stringstream ss;
    ss<<std::setprecision(Precision)<<s;
    return unserialize( ss );
}

bool arlCore::Object::setOK( bool b )
{
    if(b!=m_ok)
    {
        m_ok = b;
        update();
    }
    return m_ok;
}

bool arlCore::Object::isOK( void ) const
{
    return m_ok;
}

unsigned int arlCore::Object::getCounter( void ) const
{
    return m_counter[m_class];
}

unsigned int arlCore::Object::getNo( void ) const
{
    return m_no;
}

void arlCore::Object::setName( const std::string &name )
{
    update();
    m_name = name;
}

std::string arlCore::Object::getName( void ) const
{
    return m_name;
}

std::string arlCore::Object::getFileName( void )const
{
    std::stringstream s;
    s<<"c:/"<<m_class<<"-"<<getNo();
    s<<"-"<<m_date<<"-"<<m_time;
    s<<".txt";
    return s.str();
}

long int arlCore::Object::getUpdateIndex( void ) const
{
    return m_updateIndex;
}

void arlCore::Object::update( void )
{
    ++m_updateIndex;
}

bool arlCore::Object::getTime( long int &date, long int &time ) const
{
    date = m_date;
    time = m_time;
    return (date+time)!=0;
}

long int arlCore::Object::getDate( void ) const
{
    return m_date;
}

long int arlCore::Object::getTime( void ) const
{
    return m_time;
}

bool arlCore::Object::setTime( const long int &date, const long int &time )
{
    m_date = date;
    m_time = time;
    update();
    return (m_date+m_time)!=0;
}

bool arlCore::Object::setTime( const long int &time )
{
    m_time = time;
    update();
    return (m_date+m_time)!=0;
}

bool arlCore::Object::setMaxTime( long int date, long int time )
{
    if(date>m_date || date==m_date && time>m_time)
    {
        m_date = date;
        m_time = time;
        update();
        return true;
    }
    return false;
}

bool arlCore::Object::setMinTime( long int date, long int time )
{
    if((m_date==0 && m_time==0) || ((date<m_date || date==m_date && time<m_time) && (date!=0 || time!=0)))
    {   // Never change if(date, time) is null
        // Change always if object time is null or
        // change if(date, time) lesser than object time
        m_date = date;
        m_time = time;
        update();
        return true;
    }
    return false;
}

bool arlCore::Object::setTime( void )
{
    m_date = arlTime::getNowDate();
    m_time = arlTime::getNowTime();
    update();
    return (m_date+m_time)!=0;
}

bool arlCore::Object::startLap( void )
{
    m_lapDate = arlTime::getNowDate();
    m_lapTime = arlTime::getNowTime();
    update(); //?
    return true;
}

double arlCore::Object::getLap( void )
{   // Return lap in seconds
    double a = (double)m_lapDate;
    double b = (double)m_lapTime;
    startLap();
    double sec=0; //m_lapDate-a;
    if(b>m_lapTime)
        sec+=1.0-(b+(double)m_lapTime)/1000000.0; // FIXME
    else
        sec+=((double)m_lapTime-b)/((double)CLK_TCK*10); // FIXME
    update(); //?
    return sec;
}

double arlCore::Object::getItPerSec( void )
{
    return 1.0/getLap();
}

bool arlCore::Object::isChanged( const long int &date, const long int &time ) const
{
    return( date!=m_date || time!=m_time );
}

bool arlCore::Object::isEquivalent( const long int &date, const long int &time ) const
{
    return( date<=0  || time<0 || (m_date==0 && m_time==0) || (date==m_date && time==m_time) );
}

bool arlCore::Object::lockWriteMutex( void )
{
    if(isWriteLocked()) return false;
    m_writeMutex=true;
    return true;
}

bool arlCore::Object::unlockWriteMutex( void )
{
    setTime();
    m_writeMutex = false;
    return true;
}

unsigned int arlCore::Object::lockReadMutex( void )
{
    if(m_writeMutex) return false;
    ++m_readMutex;
    return m_readMutex;
}

unsigned int arlCore::Object::unlockReadMutex( void )
{
    if(m_readMutex>0) m_readMutex--;
    return m_readMutex;
}

bool arlCore::Object::isWriteLocked( void ) const
{
    return(m_writeMutex || m_readMutex>0);
}

bool arlCore::Object::isReadLocked( void ) const
{
    return(m_readMutex>0);
}

void arlCore::Object::setVerboseLevel( ARLCORE_LOG_VERBOSE level )
{
    m_verboseLevel = level;
}

arlCore::ARLCORE_LOG_VERBOSE arlCore::Object::getVerboseLevel( void )
{
    return m_verboseLevel;
}

void arlCore::Object::setStaticVerboseLevel( ARLCORE_LOG_VERBOSE level )
{
    m_staticVerboseLevel = level;
}

arlCore::ARLCORE_LOG_VERBOSE arlCore::Object::getStaticVerboseLevel( void )
{
    return m_staticVerboseLevel;
}

void arlCore::Object::resetStaticVerboseLevel( void )
{
    m_staticVerboseLevel = ARLCORE_LOG_VERBOSE_NBTYPES;
}

void arlCore::Object::log( ARLCORE_LOG_SERIOUSNESS level, const std::stringstream& text ) const
{
    log(level, text.str().c_str());
}

void arlCore::Object::log( ARLCORE_LOG_SERIOUSNESS level, const std::string& text ) const
{
    log(level, text.c_str());
}

void arlCore::Object::log( ARLCORE_LOG_SERIOUSNESS level )
{
    log(level, m_log);
    m_log.str("");
}

void arlCore::Object::log( ARLCORE_LOG_SERIOUSNESS level, const char* text ) const
{
    ARLCORE_LOG_VERBOSE verboseLevel;
    if(m_staticVerboseLevel<ARLCORE_LOG_VERBOSE_NBTYPES)
        verboseLevel=m_staticVerboseLevel;
    else verboseLevel=m_verboseLevel;

    if(verboseLevel<=level) return;
    if(level==ARLCORE_LOG_ERROR)
        std::cout<<"<ERROR>";
    if(level==ARLCORE_LOG_WARNING)
        std::cout<<"<WARNING>";
    if(level==ARLCORE_LOG_INFO_LEVEL2)
        std::cout<<"<INFO2>";
    if(level==ARLCORE_LOG_INFO_LEVEL1)
        std::cout<<"<INFO1>";
    std::cout<<"["<<ARLCORE_CLASS_NAMES[m_class]<<"]";
    std::cout<<"["<<this<<"] ";
    std::cout<<text<<"\n";
}
