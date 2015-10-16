/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_FACTORY_MESSAGE_NEW_HPP__
#define __FWSERVICES_FACTORY_MESSAGE_NEW_HPP__

#include <string>


#include <fwTools/macros.hpp>

#include "fwServices/config.hpp"
#include "fwServices/registry/message/detail.hpp"

namespace fwServices
{

class ObjectMsg;

namespace factory
{

namespace message
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to ObjectMsg construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR( CLASSNAME ) fwServices::factory::message::New();

Key()
{
}
};


FWSERVICES_API SPTR( ::fwServices::ObjectMsg ) New( const ::fwServices::registry::message::KeyType & classname );


template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) objMsg = std::make_shared< CLASSNAME >( Key() );
    return objMsg;
}

} // namespace message

} // namespace factory

} // namespace fwServices

#endif /* __FWSERVICES_FACTORY_MESSAGE_NEW_HPP__ */


