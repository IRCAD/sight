/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_READER_FACTORY_NEW_HPP__
#define __FWDATAIO_READER_FACTORY_NEW_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/registry/detail.hpp"

#include <string>

namespace fwDataIO
{
namespace reader
{

class IObjectReader;

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
friend SPTR( CLASSNAME ) fwDataIO::reader::factory::New();

Key()
{
}
};

FWDATAIO_API SPTR( ::fwDataIO::reader::IObjectReader ) New( const ::fwDataIO::reader::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );

    return obj;
}

} // namespace factory

} // namespace reader

} // namespace fwDataIO

#endif /* __FWDATAIO_READER_FACTORY_NEW_HPP__ */

