/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_EXCEPTION_NOWORKDER_HPP__
#define __FWCOM_EXCEPTION_NOWORKDER_HPP__

#include <fwCore/Exception.hpp>

namespace fwCom
{
namespace exception
{

struct NoWorker : ::fwCore::Exception
{
    FWCOM_API NoWorker ( const std::string &err ) : ::fwCore::Exception(err) {} ;
};

} // namespace exception


} // namespace fwCom

#endif /* __FWCOM_EXCEPTION_NOWORKDER_HPP__ */



