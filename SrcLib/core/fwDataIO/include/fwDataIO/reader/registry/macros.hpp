/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_READER_REGISTRY_MACROS_HPP__
#define __FWDATAIO_READER_REGISTRY_MACROS_HPP__

#include "fwData/registry/detail.hpp"

namespace fwDataIO
{
namespace reader
{
namespace registry
{

#define fwDataIOReaderRegisterMacro( classname )                                  \
    static ::fwDataIO::reader::IObjectReader::Registrar< classname > s__factory__record__ ## __LINE__;

} // end namespace registry
} // end namespace reader
} // end namespace fwDataIO

#endif /*__FWDATAIO_READER_REGISTRY_MACROS_HPP__*/
