/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/writer/IObjectWriter.hpp"

#include "fwDataIO/writer/factory/new.hpp"


namespace fwDataIO
{
namespace writer
{

namespace factory
{

::fwDataIO::writer::IObjectWriter::sptr New( const ::fwDataIO::writer::registry::KeyType & classname )
{
    return ::fwDataIO::writer::registry::get()->create(classname);
}

} // namespace factory

} // namespace writer

} // namespace fwDataIO


