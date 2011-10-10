/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "scene2D/data/Event.hpp"

namespace scene2D
{
namespace data
{

//-----------------------------------------------------------------------------

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::scene2D::data::Event, ::scene2D::data::Event );

//-----------------------------------------------------------------------------

Event::Event() throw()
:   m_type(NoType),
    m_button(NoButton),
    m_modifier(NoModifier),
    m_accepted(false)
{}

//-----------------------------------------------------------------------------

Event::~Event() throw()
{}

//-----------------------------------------------------------------------------

} // namespace data
} // namespace scene2D



