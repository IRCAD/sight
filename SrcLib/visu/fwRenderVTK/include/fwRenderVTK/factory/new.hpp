/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_FACTORY_NEW_HPP__
#define __FWRENDERVTK_FACTORY_NEW_HPP__

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/registry/detail.hpp"

#include <string>

namespace fwRenderVTK
{

class IVtkRenderWindowInteractorManager;

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
friend SPTR( CLASSNAME ) fwRenderVTK::factory::New();

Key()
{
}
};

FWRENDERVTK_API SPTR( ::fwRenderVTK::IVtkRenderWindowInteractorManager ) New(
    const ::fwRenderVTK::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace factory

} // namespace fwRenderVTK

#endif /* __FWRENDERVTK_FACTORY_NEW_HPP__ */

