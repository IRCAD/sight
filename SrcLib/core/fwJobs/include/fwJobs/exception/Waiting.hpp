/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWJOBS_EXCEPTION_WAITING_HPP__
#define __FWJOBS_EXCEPTION_WAITING_HPP__

#include "fwJobs/config.hpp"

#include <fwCore/Exception.hpp>

namespace fwJobs
{
namespace exception
{

/// No worker exception.
struct Waiting : ::fwCore::Exception
{
    FWJOBS_API Waiting ( const std::string &err );
};

} // namespace exception


} // namespace fwJobs

#endif /* __FWJOBS_EXCEPTION_WAITING_HPP__ */



