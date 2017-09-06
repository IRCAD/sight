/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_EXCEPTIONS_IMPOSSIBLECONVERSION_HPP__
#define __FWATOMSPATCH_EXCEPTIONS_IMPOSSIBLECONVERSION_HPP__

#include "fwAtomsPatch/exceptions/Base.hpp"

namespace fwAtomsPatch
{

namespace exceptions
{

/**
 * @brief Reports an impossible conversion between data objects.
 */
class ImpossibleConversion : public fwAtomsPatch::exceptions::Base
{
public:
    ImpossibleConversion(const std::string& message);
    virtual ~ImpossibleConversion() noexcept
    {
    }
};

} // namespace exceptions
} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_EXCEPTIONS_IMPOSSIBLECONVERSION_HPP__ */
