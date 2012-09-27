/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/registry/macros.hpp>

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

//-----------------------------------------------------------------------------

} // namespace data
} // namespace scene2D



