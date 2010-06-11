/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_FRAME_IFRAME_HPP_
#define _GUI_FRAME_IFRAME_HPP_

#include <fwServices/IService.hpp>

#include "gui/export.hpp"

namespace gui
{

/**
 * @namespace ::gui::frame
 * @brief This namespace contains the classes which define container.
 */
namespace frame
{

/**
 * @brief   Defines the service interface managing a frame.
 * @class   IFrame.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 *
 */
class GUI_CLASS_API IFrame : public ::fwServices::IService
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro ( (IFrame)( ::fwServices::IService) ) ;

protected:

    /**
    * @brief Constructor. Do nothing (Just initialize parameters).
    */
    GUI_API IFrame() throw() ;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~IFrame() throw() ;

    static const std::map<std::string, long> FWSTYLE_TO_WXSTYLE;

};



} // namespace frame
} // namespace gui


#endif /*_GUI_FRAME_IFRAME_HPP_*/
