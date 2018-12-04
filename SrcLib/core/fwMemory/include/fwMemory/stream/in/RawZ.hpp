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

#ifndef __FWMEMORY_STREAM_IN_RAWZ_HPP__
#define __FWMEMORY_STREAM_IN_RAWZ_HPP__

#include <boost/filesystem/path.hpp>
#include <fwCore/macros.hpp>

#include "fwMemory/stream/in/IFactory.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

class FWMEMORY_CLASS_API RawZ : public IFactory
{
public:
    RawZ(const boost::filesystem::path &path) :
        m_path(path)
    {
    }

protected:

    FWMEMORY_API SPTR(std::istream) get();

    boost::filesystem::path m_path;
};



} // namespace in
} // namespace stream
} // namespace fwMemory

#endif // __FWMEMORY_STREAM_IN_RAWZ_HPP__


