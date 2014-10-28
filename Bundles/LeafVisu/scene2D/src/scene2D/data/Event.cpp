/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "scene2D/data/Event.hpp"

namespace scene2D
{
namespace data
{

//-----------------------------------------------------------------------------

fwDataRegisterMacro( ::scene2D::data::Event );

//-----------------------------------------------------------------------------

Event::Event(::fwData::Object::Key key)
:   m_type(NoType),
    m_button(NoButton),
    m_modifier(NoModifier),
    m_accepted(false),
    m_key(Qt::Key_unknown)
{}

//-----------------------------------------------------------------------------

Event::~Event()
{}


//------------------------------------------------------------------------------

void Event::shallowCopy(const ::fwData::Object::csptr &_source )
{
    Event::csptr other = Event::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    Coord m_coord;
    Size m_size;
    Size m_oldSize;

    m_type = other->m_type;
    m_button = other->m_button;
    m_modifier = other->m_modifier;
    m_accepted = other->m_accepted;
    m_key = other->m_key;
    m_coord.setX(other->m_coord.getX());
    m_coord.setY(other->m_coord.getY());
    m_size.setHeight(other->m_size.getHeight());
    m_size.setWidth(other->m_size.getWidth());
    m_oldSize.setHeight(other->m_oldSize.getHeight());
    m_oldSize.setWidth(other->m_oldSize.getWidth());
}

//------------------------------------------------------------------------------

void Event::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache )
{
    Event::csptr other = Event::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_type = other->m_type;
    m_button = other->m_button;
    m_modifier = other->m_modifier;
    m_accepted = other->m_accepted;
    m_key = other->m_key;
    m_coord.setX(other->m_coord.getX());
    m_coord.setY(other->m_coord.getY());
    m_size.setHeight(other->m_size.getHeight());
    m_size.setWidth(other->m_size.getWidth());
    m_oldSize.setHeight(other->m_oldSize.getHeight());
    m_oldSize.setWidth(other->m_oldSize.getWidth());
}
//-----------------------------------------------------------------------------

} // namespace data
} // namespace scene2D



