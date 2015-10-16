/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Resection.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Resection );

namespace fwData
{

//------------------------------------------------------------------------------

Resection::Resection (::fwData::Object::Key key) : m_isSafePart(true),
                                                   m_isValid(false),
                                                   m_isVisible(true)
{
    m_planeList = ::fwData::PlaneList::New();
}

//------------------------------------------------------------------------------

Resection::~Resection ()
{
}

//------------------------------------------------------------------------------

void Resection::shallowCopy(const Object::csptr &_source )
{
    Resection::csptr other = Resection::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = other->m_planeList;
    m_vInputs    = other->m_vInputs;
    m_vOutputs   = other->m_vOutputs;
}

//------------------------------------------------------------------------------

void Resection::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Resection::csptr other = Resection::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_name       = other->m_name;
    m_isSafePart = other->m_isSafePart;
    m_isValid    = other->m_isValid;
    m_isVisible  = other->m_isVisible;
    m_planeList  = ::fwData::Object::copy(other->m_planeList, cache);

    this->m_vInputs.clear();
    for(const ResectionInputs::value_type &resec : other->m_vInputs)
    {
        m_vInputs.push_back( ::fwData::Object::copy(resec, cache) );
    }

    this->m_vOutputs.clear();
    for(const ResectionOutputs::value_type &resec : other->m_vOutputs)
    {
        m_vOutputs.push_back( ::fwData::Object::copy(resec, cache) );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


