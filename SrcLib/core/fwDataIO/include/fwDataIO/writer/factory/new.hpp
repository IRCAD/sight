/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_FACTORY_NEW_HPP__
#define __FWDATAIO_WRITER_FACTORY_NEW_HPP__

#include <string>

#include <boost/make_shared.hpp>

#include <fwTools/macros.hpp>
#include <fwTools/DynamicAttributes.hxx>

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/registry/detail.hpp"

namespace fwDataIO
{
namespace writer
{

class IObjectWriter;

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
    friend SPTR( CLASSNAME ) fwDataIO::writer::factory::New();

    Key(){};
};


FWDATAIO_API SPTR( ::fwDataIO::writer::IObjectWriter ) New( const ::fwDataIO::writer::registry::KeyType & classname );


template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = ::boost::make_shared< CLASSNAME >( Key() );

    return obj;
}

} // namespace factory

} // namespace writer

} // namespace fwDataIO

#endif /* __FWDATAIO_WRITER_FACTORY_NEW_HPP__ */


