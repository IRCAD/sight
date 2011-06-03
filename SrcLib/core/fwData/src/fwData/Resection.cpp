/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Resection.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Resection, ::fwData::Resection );

namespace fwData
{

//------------------------------------------------------------------------------

Resection::Resection ()
: m_isSafePart(true),
  m_isVisible(true),
  m_isValid(false)
{
    SLM_TRACE_FUNC();
    m_planeList = ::fwData::PlaneList::New();
}

//------------------------------------------------------------------------------

Resection::~Resection ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Resection::shallowCopy( Resection::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_name = _source->m_name;
    this->m_isSafePart = _source->m_isSafePart;
    this->m_isValid = _source->m_isValid;
    this->m_isVisible = _source->m_isVisible;
    this->m_planeList = _source->m_planeList;
    this->m_vInputs = _source->m_vInputs;
    this->m_vOutputs = _source->m_vOutputs;
}

//------------------------------------------------------------------------------

void Resection::deepCopy( Resection::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_name = _source->m_name;
    this->m_isSafePart = _source->m_isSafePart;
    this->m_isValid = _source->m_isValid;
    this->m_isVisible = _source->m_isVisible;
    this->m_planeList->deepCopy( _source->m_planeList );

    this->m_vInputs.clear();
    for (   Resection::ResectionInputs::const_iterator iter = _source->m_vInputs.begin();
            iter != _source->m_vInputs.end();
            ++iter )
    {
        Reconstruction::NewSptr newObj;
        newObj->deepCopy( *iter );
        this->m_vInputs.push_back( newObj );
    }

    this->m_vOutputs.clear();
    for (   Resection::ResectionOutputs::const_iterator iter = _source->m_vOutputs.begin();
            iter != _source->m_vOutputs.end();
            ++iter )
    {
        Reconstruction::NewSptr newObj;
        newObj->deepCopy( *iter );
        this->m_vOutputs.push_back( newObj );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


