#ifndef _OPKINECT_REPRESENTATIONSRV_HPP_
#define _OPKINECT_REPRESENTATIONSRV_HPP_

#include "config.hpp"
#include <fwGui/IActionSrv.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>

namespace uiAcquisition
{

/**
 * @brief Service allow to manage opKinect bundle
 */
class RepresentationSrv : public ::fwGui::IActionSrv
{
public :
    fwCoreServiceClassDefinitionsMacro ( (RepresentationSrv)(::fwGui::IActionSrv) );

    RepresentationSrv() throw() ;
    virtual ~RepresentationSrv() throw() ;

protected:
    void configuring() throw ( ::fwTools::Failed );
    void starting() throw ( ::fwTools::Failed );
    void stopping() throw ( ::fwTools::Failed );
    void updating() throw ( ::fwTools::Failed );
    void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );
    void info ( std::ostream &_sstream ) ;
    void fillReconstructionVector(::fwData::Acquisition::sptr);

private:
    std::vector<fwData::Reconstruction::sptr> m_reconstructions;
};

} // namespace opKinect


#endif // _OPKINECT_REPRESENTATIONSRV_HPP_
