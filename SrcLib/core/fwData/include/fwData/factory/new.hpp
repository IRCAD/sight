/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_FACTORY_NEW_HPP__
#define __FWDATA_FACTORY_NEW_HPP__

#include "fwData/config.hpp"
#include "fwData/registry/detail.hpp"

#include <string>

namespace fwData
{

class Object;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
private:
    template<typename CLASSNAME>
    friend SPTR( CLASSNAME ) fwData::factory::New();

    Key()
    {
    }
};

FWDATA_API SPTR( ::fwData::Object ) New( const ::fwData::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );
    return obj;
}

} // namespace factory

} // namespace fwData

#endif /* __FWDATA_FACTORY_NEW_HPP__ */

