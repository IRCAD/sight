/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwTools/fwID.hpp"

#include "fwTools/Failed.hpp"
#include "fwTools/Object.hpp"

#include <fwCore/Demangler.hpp>

#include <boost/lexical_cast.hpp>

namespace fwTools
{

fwID::CategorizedCounter fwID::m_CategorizedCounter;
fwID::Dictionary fwID::m_dictionary;
::fwCore::mt::ReadWriteMutex fwID::s_dictionaryMutex;
::fwCore::mt::Mutex fwID::s_mutexCounter;

//-----------------------------------------------------------------------------

fwID::~fwID()
{
    resetID();
}

//-----------------------------------------------------------------------------

bool fwID::exist( IDType _id)
{
    ::fwCore::mt::ReadLock lock(s_dictionaryMutex);
    return fwID::isIdFound(_id);
}

//-----------------------------------------------------------------------------

bool fwID::isIdFound( IDType _id)
{
    return m_dictionary.find( _id ) != m_dictionary.end();
}

//-----------------------------------------------------------------------------

bool fwID::hasID( ) const
{
    ::fwCore::mt::ReadLock lock(m_idMutex);
    return !m_id.empty();
}

//-----------------------------------------------------------------------------

void fwID::setID( IDType newID )
{
    ::fwCore::mt::WriteLock lock(m_idMutex);
    this->addIDInDictionary(newID);
}

//-----------------------------------------------------------------------------

void fwID::addIDInDictionary( IDType newID )
{
    OSLM_FATAL_IF("Try to set an existing fwID = " << newID, isIdFound(newID));

    ::fwCore::mt::WriteLock lock(s_dictionaryMutex);
    fwID::removeIDfromDictionary(m_id);
    // note we use a static cast for a down cast because we do not use the classical polyvi morphic approach
    //m_dictionary[ newID ] = (static_cast< Object *>(this))->getSptr();
    m_dictionary[ newID ] = ((Object*)(this))->getSptr();
    m_id                  = newID;
}

//-----------------------------------------------------------------------------

fwID::IDType fwID::getID( Policy policy) const
{
    ::fwCore::mt::ReadToWriteLock lock(m_idMutex);
    if ( m_id.empty() ) // no id set
    {
        if ( policy == GENERATE )
        {
            IDType newID = generate();
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            const_cast<fwID*>(this)->addIDInDictionary(newID);
        }
        else if  ( policy == EMPTY )
        { /* nothing to do*/
        }
        else if ( policy == MUST_EXIST )
        {
            throw fwTools::Failed( "fwID::getID() no id set" );
        }
    }
    return m_id;
}

//-----------------------------------------------------------------------------

fwID::IDType fwID::generate() const
{
    IDType newID;
    std::string prefix = this->getClassname();
    do
    {
        ::fwCore::mt::ScopedLock lock(s_mutexCounter);
        newID = prefix  + "-" + boost::lexical_cast<std::string>( m_CategorizedCounter[prefix]++ );
    }
    while ( exist(newID ) );
    return newID;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr fwID::getObject( fwID::IDType requestID )
{
    ::fwCore::mt::ReadLock lock(s_dictionaryMutex);
    Dictionary::iterator it = m_dictionary.find(requestID);
    if ( it != m_dictionary.end() )
    {
        SLM_ASSERT(  "expired object in fwID::Dictionary for id=" + requestID,  !it->second.expired() );
        return it->second.lock();
    }
    else
    {
        return ::fwTools::Object::sptr();
    }
}

//-----------------------------------------------------------------------------

void fwID::resetID()
{
    ::fwCore::mt::WriteLock lock(m_idMutex);
    ::fwCore::mt::WriteLock dicoLock(s_dictionaryMutex);
    fwID::removeIDfromDictionary(m_id);
    m_id.clear();
}

//-----------------------------------------------------------------------------

void fwID::removeIDfromDictionary(IDType _id )
{
    if ( !_id.empty() )
    {
        m_dictionary.erase(_id);
    }
}

//-----------------------------------------------------------------------------

}
