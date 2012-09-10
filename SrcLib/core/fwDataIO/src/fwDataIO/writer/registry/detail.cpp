/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwDataIO/writer/registry/detail.hpp"


namespace fwDataIO
{
namespace writer
{

namespace registry
{

struct FwDataIOWriterRegistryInstantiatorTag {} ;

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwDataIOWriterRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry

} // namespace writer

} // namespace fwDataIO


