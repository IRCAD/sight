/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __FWATOMSPATCH_EXCEPTIONS_BASE_HPP__
#define __FWATOMSPATCH_EXCEPTIONS_BASE_HPP__

#include <fwCore/Exception.hpp>

namespace fwAtomsPatch
{

namespace exceptions
{

/**
 * @brief Base class of fwAtomsPatch exceptions
 */
class Base : public fwCore::Exception
{
public:
    Base(const std::string& message);
    virtual ~Base() noexcept
    {
    }
};


} // namespace exceptions
} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_EXCEPTIONS_BASE_HPP__ */

