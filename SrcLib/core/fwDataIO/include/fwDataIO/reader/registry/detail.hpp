/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATAIO_READER_REGISTRY_DETAIL_HPP__
#define __FWDATAIO_READER_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwDataIO/config.hpp"

namespace fwDataIO
{
namespace reader
{

class IObjectReader;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwDataIO::reader::IObjectReader) (), KeyType > Type;

FWDATAIO_API SPTR(Type) get();

} // namespace registry

} // namespace reader

} // namespace fwDataIO

#endif /* __FWDATAIO_READER_REGISTRY_DETAIL_HPP__ */


