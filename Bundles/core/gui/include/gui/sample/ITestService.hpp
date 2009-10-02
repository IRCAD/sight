/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_SAMPLE_ITESTSERVICE_HPP_
#define _GUI_SAMPLE_ITESTSERVICE_HPP_

#include <fwServices/IService.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace sample
{
/**
 * @brief	Defines the service interface managing the rendering service for object
 * @author	IRCAD (Research and Development Team).
 */
class GUI_CLASS_API ITestService : public fwServices::IService
{

public :
	
	GUI_API ITestService() throw() ;
	GUI_API virtual ~ITestService() throw() ;
	GUI_API virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;	
	GUI_API virtual void info(std::ostream &_sstream ) ;
};



} // namespace sample
} // namespace gui


#endif /* _GUI_SAMPLE_ITESTSERVICE_HPP_ */
