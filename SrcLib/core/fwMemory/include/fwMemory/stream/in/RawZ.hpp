/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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


