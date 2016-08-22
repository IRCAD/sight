/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_EXCEPTIONS_CONTENTNOTFOUND_HPP__
#define __FWNETWORK_EXCEPTIONS_CONTENTNOTFOUND_HPP__

#include "fwNetwork/exceptions/Base.hpp"

namespace fwNetwork
{

namespace exceptions
{

/**
 * @brief Implements exception for HTTP content not found errors.
 */
class ContentNotFound : public fwNetwork::exceptions::Base
{
public:
    /**
     * @name Constructor/Destructor.
     * @{ */
    ContentNotFound(const std::string& message);
    virtual ~ContentNotFound() throw();
    /**  @} */
};

} /* namespace exceptions */
} /* namespace fwNetwork */

#endif /* __FWNETWORK_EXCEPTIONS_CONTENTNOTFOUND_HPP__ */

