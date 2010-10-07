/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <boost/lexical_cast.hpp>

#include <fwCore/Demangler.hpp>

#include "fwTools/fwID.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/Failed.hpp"

namespace fwTools
{

fwID::CategorizedCounter fwID::m_CategorizedCounter;
fwID::Dictionary fwID::m_dictionary;

//-----------------------------------------------------------------------------

fwID::~fwID()
{
    resetID();
}

//-----------------------------------------------------------------------------

bool fwID::exist( IDType _id)
{
    return m_dictionary.find( _id ) != m_dictionary.end();
}

//-----------------------------------------------------------------------------

bool fwID::hasID( ) const
{
    return !m_id.empty();
}

//-----------------------------------------------------------------------------

void fwID::setID( IDType newID )
{
    resetID();
    // note we use a static cast for a down cast because we do not use the classical polyvi morphic approach
    // m_dictionary[ newID ] = (static_cast< Object *>(this))->getSptr();
    m_dictionary[ newID ] = ((Object*)(this))->getSptr();
    m_id = newID;
}

//-----------------------------------------------------------------------------

fwID::IDType fwID::getID( Policy policy) const
{
    if ( m_id.empty() ) // no id set
    {
        if ( policy==GENERATE )
        {
            IDType newID = generate();
            const_cast<fwID *>(this)->setID(newID);
        }
        else if  ( policy == EMPTY )
        { /* nothing to do*/ }
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
    std::string prefix = this->getRootedClassname();
    do
    {
        newID = prefix  + "-" + boost::lexical_cast<std::string>( m_CategorizedCounter[prefix]++ );
    }
    while ( exist(newID ) );
    return newID;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr fwID::getObject( fwID::IDType requestID )
{
    Dictionary::iterator it = m_dictionary.find(requestID);
    if ( it!=m_dictionary.end() )
    {
        SLM_ASSERT(  "expired object in fwID::Dictionary for id=" + requestID ,  !it->second.expired() );
        return it->second.lock();
    }
    else
    {
        return ::fwTools::Object::NewSptr();
    }
}

//-----------------------------------------------------------------------------

void fwID::resetID()
{
    if ( !m_id.empty() )
    {
        m_dictionary.erase(m_id);
    }
    m_id.clear();
}

//-----------------------------------------------------------------------------

void fwID::swapID( ::fwTools::Object::sptr   obj2 )
{
    IDType id1 = getID(GENERATE);
    IDType id2 = obj2->getID(GENERATE);
    resetID();
    obj2->resetID();
    setID(id2);
    obj2->setID(id1);
}

//-----------------------------------------------------------------------------

}
