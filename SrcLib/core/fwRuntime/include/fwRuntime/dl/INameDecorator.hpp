/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_INAMEDECORATOR_HPP_
#define _FWRUNTIME_DL_INAMEDECORATOR_HPP_


#include <string>


namespace fwRuntime
{
namespace dl
{



/**
 * @brief   Defines the interface of library name decorators.
 * @struct  INameDecorator
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct INameDecorator
{

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~INameDecorator();

    /**
     * @brief       Retrieves the native library file name for the given abstract file name.
     *
     * The native name can be a pattern and will be used for string matching.
     *
     * @param[in]   abstractName        the abstract library file name to convert to native library file name
     */
    virtual const std::string getNativeName( const std::string & abstractName ) const throw() = 0;

};


} // namespace dl

} // namespace fwRuntime



#endif /*_FWRUNTIME_DL_INAMEDECORATOR_HPP_*/
