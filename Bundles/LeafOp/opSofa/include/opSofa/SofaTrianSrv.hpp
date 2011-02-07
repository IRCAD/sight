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
    void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

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
