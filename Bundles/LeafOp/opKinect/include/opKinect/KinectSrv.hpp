/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPKINECT_KINECTSRV_HPP_
#define _OPKINECT_KINECTSRV_HPP_

#include "config.hpp"
#include <fwGui/IActionSrv.hpp>
#include "opKinect/Kinect.hpp"

namespace opKinect
{

/**
 * @brief Service allow to manage opKinect bundle
 */
class KinectSrv : public ::fwGui::IActionSrv
{
public :
    fwCoreServiceClassDefinitionsMacro ( (KinectSrv)(::fwGui::IActionSrv) );

    KinectSrv() throw() ;
    virtual ~KinectSrv() throw() ;

protected:
    void configuring() throw ( ::fwTools::Failed );
    void starting() throw ( ::fwTools::Failed );
    void stopping() throw ( ::fwTools::Failed );
    void updating() throw ( ::fwTools::Failed );
    void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );
    void info ( std::ostream &_sstream ) ;
    
private :
    Kinect *kinect;
};

} // namespace opKinect


#endif // _OPKINECT_KINECTSRV_HPP_
