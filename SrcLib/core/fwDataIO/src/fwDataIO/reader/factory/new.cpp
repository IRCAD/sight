/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/reader/IObjectReader.hpp"

#include "fwDataIO/reader/factory/new.hpp"


namespace fwDataIO
{
namespace reader
{

namespace factory
{

::fwDataIO::reader::IObjectReader::sptr New( const ::fwDataIO::reader::registry::KeyType & classname )
{
    return ::fwDataIO::reader::registry::get()->create(classname);
}

} // namespace factory

} // namespace reader

} // namespace fwDataIO


