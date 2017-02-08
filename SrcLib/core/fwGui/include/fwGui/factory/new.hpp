/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_FACTORY_NEW_HPP__
#define __FWGUI_FACTORY_NEW_HPP__

#include "fwGui/config.hpp"
#include "fwGui/registry/detail.hpp"

#include <string>

namespace fwGui
{

class GuiBaseObject;

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
friend SPTR( CLASSNAME ) fwGui::factory::New();

Key()
{
}
};

FWGUI_API SPTR( ::fwGui::GuiBaseObject ) New( const ::fwGui::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) guiObj = std::make_shared< CLASSNAME >( Key() );
    return guiObj;
}

} // namespace factory

} // namespace fwGui

#endif /* __FWGUI_FACTORY_NEW_HPP__ */

