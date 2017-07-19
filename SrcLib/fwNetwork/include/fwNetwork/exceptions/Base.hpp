/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_EXCEPTIONS_BASE_HPP__
#define __FWNETWORK_EXCEPTIONS_BASE_HPP__

#include <fwCore/Exception.hpp>

namespace fwNetwork
{

namespace exceptions
{

/**
 * @brief Base class of fwNetwork Exception
 */
class Base : public fwCore::Exception
{
public:
    Base(const std::string& message);
    virtual ~Base() noexcept;
};


} // namespace exceptions
} // namespace fwNetwork

#endif /* __FWNETWORK_EXCEPTIONS_BASE_HPP__ */

