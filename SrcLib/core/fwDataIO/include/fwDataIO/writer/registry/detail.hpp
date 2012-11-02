/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_REGISTRY_DETAIL_HPP__
#define __FWDATAIO_WRITER_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwDataIO/config.hpp"

namespace fwDataIO
{
namespace writer
{

class IObjectWriter;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwDataIO::writer::IObjectWriter) () , KeyType > Type;

FWDATAIO_API SPTR(Type) get();

} // namespace registry

} // namespace writer

} // namespace fwDataIO

#endif /* __FWDATAIO_WRITER_REGISTRY_DETAIL_HPP__ */


