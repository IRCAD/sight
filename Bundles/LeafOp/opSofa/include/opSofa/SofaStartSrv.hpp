#ifndef _opSofa_SOFASSTARTSRV_HPP_
#define _opSofa_SOFASSTARTSRV_HPP_

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
    public :
        fwCoreServiceClassDefinitionsMacro ( (SofaStartSrv)(::fwGui::IActionSrv ) ) ;

        SofaStartSrv() throw();
        virtual ~SofaStartSrv() throw();
        void configuring() throw ( ::fwTools::Failed );
        void starting() throw ( ::fwTools::Failed );
        void stopping() throw ( ::fwTools::Failed );
        void updating() throw ( ::fwTools::Failed );
        void updating( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed );
        void info( std::ostream & ostr );

};

}

#endif // _opSofa_SOFASSTARTSRV_HPP_
