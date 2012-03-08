/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"


#include "fwData/Composite.hpp"


fwDataRegisterMacro( ::fwData::Composite );

namespace fwData
{


Composite::Composite()
{
    SLM_TRACE_FUNC();
}


Composite::~Composite()
{
    SLM_TRACE_FUNC();
}


Composite &Composite::getRefMap()
{
    return *this;
}


Composite const &Composite::getRefMap() const
{
    return *this;
}

//------------------------------------------------------------------------------

void Composite::shallowCopy( Composite::csptr _source )
{
    this->::fwData::Object::fieldShallowCopy( _source );
    this->clear();

     for(    Composite::Container::const_iterator iter = _source->begin();
             iter != _source->end();
             ++iter )
     {
         (*this)[ iter->first ] = iter->second;
     }
}

//------------------------------------------------------------------------------

void Composite::deepCopy( Composite::csptr _source )
{
    this->::fwData::Object::fieldDeepCopy( _source );

    this->clear();

    for(    Composite::Container::const_iterator iter = _source->begin();
            iter != _source->end();
            ++iter )
    {
        ::fwData::Object::sptr newObj = ::fwData::Factory::New( iter->second->getClassname() );
        newObj->deepCopy( iter->second );
        (*this)[ iter->first ] = newObj;
    }
}

//------------------------------------------------------------------------------

}
