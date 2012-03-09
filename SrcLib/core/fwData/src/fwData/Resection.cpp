/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Resection.hpp"

fwDataRegisterMacro( ::fwData::Resection );

namespace fwData
{

//------------------------------------------------------------------------------

Resection::Resection ()
: m_isSafePart(true),
  m_isValid(false),
  m_isVisible(true)
{
    m_planeList = ::fwData::PlaneList::New();
}

//------------------------------------------------------------------------------

Resection::~Resection ()
{}

//------------------------------------------------------------------------------

void Resection::shallowCopy( Resection::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_name = _source->m_name;
    m_isSafePart = _source->m_isSafePart;
    m_isValid = _source->m_isValid;
    m_isVisible = _source->m_isVisible;
    m_planeList = _source->m_planeList;
    m_vInputs = _source->m_vInputs;
    m_vOutputs = _source->m_vOutputs;
}

//------------------------------------------------------------------------------

void Resection::deepCopy( Resection::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_name = _source->m_name;
    m_isSafePart = _source->m_isSafePart;
    m_isValid = _source->m_isValid;
    m_isVisible = _source->m_isVisible;
    m_planeList->deepCopy( _source->m_planeList );

    this->m_vInputs.clear();
    BOOST_FOREACH(ResectionInputs::value_type resec, _source->m_vInputs)
    {
        Reconstruction::NewSptr newObj;
        newObj->deepCopy( resec );
        m_vInputs.push_back( newObj );
    }

    this->m_vOutputs.clear();
    BOOST_FOREACH(ResectionOutputs::value_type resec, _source->m_vOutputs)
    {
        Reconstruction::NewSptr newObj;
        newObj->deepCopy( resec );
        m_vOutputs.push_back( newObj );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


