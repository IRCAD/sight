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

#pragma once

#include "viz/ogre/config.hpp"
#include "viz/ogre/registry/detail.hpp"

#include <string>

namespace sight::viz::ogre
{

class IWindowInteractor;
class ILight;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR( CLASSNAME ) viz::ogre::factory::New();

Key()
{
}
};

VIZ_OGRE_API SPTR( viz::ogre::IWindowInteractor ) New(
    const viz::ogre::registry::KeyType& classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

template <class CLASSNAME> SPTR(CLASSNAME) New(std::pair<unsigned int, unsigned int> _dims);

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template <typename CLASSNAME>
friend SPTR(CLASSNAME) viz::ogre::offscreenInteractorMgrFactory::New(std::pair<unsigned int, unsigned int>);

Key()
{
}
};

VIZ_OGRE_API SPTR(viz::ogre::IWindowInteractor) New(
    const viz::ogre::registry::KeyType& classname, std::pair<unsigned int, unsigned int> _dims);

template <class CLASSNAME> SPTR(CLASSNAME) New(std::pair<unsigned int, unsigned int> _dims)
{
    return std::make_shared<CLASSNAME>(Key(), _dims.first, _dims.second);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR( CLASSNAME ) viz::ogre::lightFactory::New();

Key()
{
}
};

VIZ_OGRE_API SPTR( viz::ogre::ILight ) New(
    const viz::ogre::registry::KeyType& classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace lightFactory

} // namespace sight::viz::ogre
