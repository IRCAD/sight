/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwRenderOgre/factory/Text.hpp"
#include "fwRenderOgre/Text.hpp"

namespace fwRenderOgre
{

namespace factory
{

//-----------------------------------------------------------------------------

const ::Ogre::String Text::FACTORY_TYPE_NAME = "Text";

//-----------------------------------------------------------------------------

const ::Ogre::String& Text::getType(void) const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

::Ogre::MovableObject* Text::createInstanceImpl( const ::Ogre::String& name, const ::Ogre::NameValuePairList* )
{
    return new ::fwRenderOgre::Text(name);
}

//-----------------------------------------------------------------------------

void Text::destroyInstance( ::Ogre::MovableObject* obj)
{
    delete obj;
}

} // namespace factory

} // namespace fwRenderOgre
