/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPSOFA_SOFATRIANSRV_HPP_
#define _OPSOFA_SOFATRIANSRV_HPP_

#include <fwGui/IActionSrv.hpp>
#include "config.hpp"
#include "opSofa/SofaBusiness.hpp"

namespace opSofa
{


/**
 * @brief Implements the opSofa bundle
 */
class SofaTrianSrv : public ::fwGui::IActionSrv
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SofaTrianSrv)(::fwGui::IActionSrv) );

    SofaTrianSrv() throw() ;
    virtual ~SofaTrianSrv() throw() ;

protected:

    void configuring() throw ( ::fwTools::Failed );
    void starting() throw ( ::fwTools::Failed );
    void stopping() throw ( ::fwTools::Failed );
    void updating() throw ( ::fwTools::Failed );
    void receiving( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

private :

    /**
     * @brief Triangular mesh identifier
     */
    std::string m_meshUID;

    /**
     * @brief SofaBusiness pointer
     */
    SofaBusiness *sofa;

};


} // namespace opSofa


#endif // _OPSOFA_SOFATRIANSRV_HPP_
