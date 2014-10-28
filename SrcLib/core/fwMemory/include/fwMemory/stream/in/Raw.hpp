/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_STREAM_IN_RAW_HPP__
#define __FWMEMORY_STREAM_IN_RAW_HPP__

#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

#include "fwMemory/stream/in/IFactory.hpp"
#include "fwMemory/FileHolder.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

class FWMEMORY_CLASS_API Raw : public IFactory
{
public:
    Raw(const ::boost::filesystem::path &path):
        m_path(path)
    {};

    Raw(const ::fwMemory::FileHolder &path):
        m_path(path)
    {};

protected:

    FWMEMORY_API SPTR(std::istream) get();

    ::fwMemory::FileHolder m_path;
};



} // namespace in
} // namespace stream
} // namespace fwMemory

#endif // __FWMEMORY_STREAM_IN_RAW_HPP__


