/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/ogre/factory/new.hpp"

#include "viz/ogre/ILight.hpp"
#include "viz/ogre/IWindowInteractor.hpp"

namespace sight::viz::ogre
{

namespace factory
{

//------------------------------------------------------------------------------

viz::ogre::IWindowInteractor::sptr New( const viz::ogre::registry::KeyType& classname )
{
    return viz::ogre::registry::get()->create(classname);
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

//------------------------------------------------------------------------------

viz::ogre::IWindowInteractor::sptr New(const viz::ogre::registry::KeyType& classname,
                                       std::pair<unsigned int, unsigned int> _dims)
{
    return viz::ogre::registry::getOffscreenMgr()->create(classname, _dims);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

//------------------------------------------------------------------------------

viz::ogre::ILight::sptr New( const viz::ogre::registry::KeyType& classname)
{
    return viz::ogre::registry::getLightRegistry()->create(classname);
}

} // namespace lightFactory

} // namespace sight::viz::ogre
