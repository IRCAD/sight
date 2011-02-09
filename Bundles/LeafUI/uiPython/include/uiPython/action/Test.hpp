/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPYTHON_ACTION_QUITACTION_HPP_
#define _UIPYTHON_ACTION_QUITACTION_HPP_


#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiPython/config.hpp"

namespace uiPython
{
namespace action
{

/**
 * @brief   This action tries to close the window and reset root object.
 * @class   Test.
 * @author  IRCAD (Research and Development Team).

 * @date    2009-2010.
 */
class UIPYTHON_CLASS_API Test : public ::fwGui::IActionSrv
{

public :

     fwCoreServiceClassDefinitionsMacro ( (Test)(::fwGui::IActionSrv) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    UIPYTHON_API Test() throw() ;

    /**
    * @brief Destructor. Do nothing.
    */
    UIPYTHON_API virtual ~Test() throw() ;

protected:

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    UIPYTHON_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or stopped
     */
    UIPYTHON_API void configuring() throw( ::fwTools::Failed );

    UIPYTHON_API virtual void starting() throw(::fwTools::Failed);

    UIPYTHON_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method tries to close the window and reset root object.
     */
    UIPYTHON_API virtual void updating() throw(::fwTools::Failed);

    /*
     * @brief This method gives information about the class.
     */
    UIPYTHON_API virtual void info(std::ostream &_sstream ) ;

};


} // namespace action
} // namespace uiPython


#endif /*_UIPYTHON_ACTION_QUITACTION_HPP_*/
