/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFASCENEREADERSRV_HPP__
#define __OPSOFA_SOFASCENEREADERSRV_HPP__

#include <iostream>

#include <fwTools/Failed.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

namespace opSofa
{

/**
 * @brief Service allow to read file scn
 */
class SofaSceneReaderSrv : public ::io::IReader
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SofaSceneReaderSrv)(::io::IReader) );

    SofaSceneReaderSrv() noexcept;
    virtual ~SofaSceneReaderSrv() noexcept;
    void configuring();
    void starting();
    void stopping();
    void updating();
    void receiving( ::fwServices::ObjectMsg::csptr msg );
    void info( std::ostream & ostr );
    void configureWithIHM();

};

}

#endif // __OPSOFA_SOFASCENEREADERSRV_HPP__

