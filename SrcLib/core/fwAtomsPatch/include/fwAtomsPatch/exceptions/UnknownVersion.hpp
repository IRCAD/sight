/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_EXCEPTIONS_UNKNOWNVERSION_HPP__
#define __FWATOMSPATCH_EXCEPTIONS_UNKNOWNVERSION_HPP__

#include "fwAtomsPatch/exceptions/Base.hpp"

namespace fwAtomsPatch
{

namespace exceptions
{

/**
 * @brief Thrown when a given object version is unknown and can't be processed.
 */
class UnknownVersion : public fwAtomsPatch::exceptions::Base
{
public:
    UnknownVersion(const std::string& message);
    virtual ~UnknownVersion() noexcept
    {
    }
};

} // namespace exceptions
} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_EXCEPTIONS_UNKNOWNVERSION_HPP__ */
