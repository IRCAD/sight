/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/data/Viewport.hpp"

#include <fwData/registry/macros.hpp>

fwDataRegisterMacro(::fwRenderQt::data::Viewport);

namespace fwRenderQt
{
namespace data
{

//------------------------------------------------------------------------------

void Viewport::shallowCopy(const ::fwData::Object::csptr& _source )
{
    Viewport::csptr other = Viewport::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;
}

//------------------------------------------------------------------------------

inline void Viewport::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache )
{
    Viewport::csptr other = Viewport::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;
}

} // namespace data
} // namespace fwRenderQt

