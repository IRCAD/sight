/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NETWORKLOGGER_TYPES_HPP__
#define __NETWORKLOGGER_TYPES_HPP__

#include <istream>
#include <boost/function.hpp>

namespace networkLogger
{
/**
 * @struct LoggerFunction
 *
 * @tparam Socket type of socket to use
 *
 * @brief only a struct define a logger function type with a unknown type(like type alias functionnality- in c++11)
 *
 * @see http://en.cppreference.com/w/cpp/language/type_alias
 */
template<typename Socket>
struct LoggerFunction
{
    /**
     * @typedef FunctionType
     *
     * @brief function type of a logger
     */
    typedef ::boost::function<void (std::ostream&, Socket&)> FunctionType;
};
}

#endif // __NETWORKLOGGER_TYPES_HPP__