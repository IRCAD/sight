/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/registry/detail.hpp"

#include <string>

namespace fwRenderOgre
{

class IRenderWindowInteractorManager;
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
friend SPTR( CLASSNAME ) fwRenderOgre::factory::New();

Key()
{
}
};

FWRENDEROGRE_API SPTR( ::fwRenderOgre::IRenderWindowInteractorManager ) New(
    const ::fwRenderOgre::registry::KeyType& classname );

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
friend SPTR(CLASSNAME) fwRenderOgre::offscreenInteractorMgrFactory::New(std::pair<unsigned int, unsigned int>);

Key()
{
}
};

FWRENDEROGRE_API SPTR(::fwRenderOgre::IRenderWindowInteractorManager) New(
    const ::fwRenderOgre::registry::KeyType& classname, std::pair<unsigned int, unsigned int> _dims);

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
friend SPTR( CLASSNAME ) fwRenderOgre::lightFactory::New();

Key()
{
}
};

FWRENDEROGRE_API SPTR( ::fwRenderOgre::ILight ) New(
    const ::fwRenderOgre::registry::KeyType& classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace lightFactory

namespace interactorFactory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

FWRENDEROGRE_API SPTR( ::fwRenderOgre::interactor::IInteractor ) New(
    const ::fwRenderOgre::registry::KeyType& classname, const std::string&);

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( );
    return obj;
}

} // namespace interactorFactory

} // namespace fwRenderOgre
