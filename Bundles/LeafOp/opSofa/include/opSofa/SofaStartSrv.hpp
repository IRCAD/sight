/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFASTARTSRV_HPP__
#define __OPSOFA_SOFASTARTSRV_HPP__

#include <iostream>

#include <fwTools/Failed.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <fwGui/IActionSrv.hpp>

namespace opSofa
{

/**
 * @brief Service allow to start or stop Sofa
 */
class SofaStartSrv : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SofaStartSrv)(::fwGui::IActionSrv ) );

    SofaStartSrv() throw();
    virtual ~SofaStartSrv() throw();
    void configuring() throw ( ::fwTools::Failed );
    void starting() throw ( ::fwTools::Failed );
    void stopping() throw ( ::fwTools::Failed );
    void updating() throw ( ::fwTools::Failed );
    void receiving( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed );
    void info( std::ostream & ostr );

};

}

#endif // __OPSOFA_SOFASTARTSRV_HPP__

