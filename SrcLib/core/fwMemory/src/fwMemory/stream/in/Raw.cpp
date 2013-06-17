/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwMemory/stream/in/Raw.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

SPTR(std::istream) Raw::get()
{
    SPTR(::boost::filesystem::ifstream) fs
        = ::boost::make_shared< ::boost::filesystem::ifstream>(m_path, std::ios::in|std::ios::binary);
    return fs;
}

} // namespace in
} // namespace stream
} // namespace fwMemory

