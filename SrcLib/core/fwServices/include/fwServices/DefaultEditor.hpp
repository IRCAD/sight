/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_DEFAULTEDITOR_HPP_
#define _FWSERVICES_DEFAULTEDITOR_HPP_

#include "fwServices/config.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

/**
 * @brief	Implementation of basic service of type edition service. ::boost::shared_ptr< const fwServices::ObjectMsg::sptr )
 * @class	DefaultEditor.
 * @author IRCAD (Research and Development Team).

 * @date	2007-2009.
 */
class FWSERVICES_CLASS_API DefaultEditor : public ::fwServices::IEditionService
{

public :

	fwCoreServiceClassDefinitionsMacro ( (DefaultEditor)(::fwServices::IEditionService) ) ;

	/// Constructor.  Do nothing.
	FWSERVICES_API DefaultEditor() throw() ;

	/// Destructor. Do nothing.
	FWSERVICES_API virtual ~DefaultEditor() throw() ;

	/// Implements starting method derived from IService. Do nothing.
	FWSERVICES_API virtual void starting()  throw ( ::fwTools::Failed );

	/// Implements stopping method derived from IService. Do nothing.
	FWSERVICES_API virtual void stopping()  throw ( ::fwTools::Failed );

	/// Implements configuring method derived from IService. Do nothing.
	FWSERVICES_API virtual void configuring()  throw ( ::fwTools::Failed );

	/// Implements reconfiguring method derived from IService. Do nothing.
	FWSERVICES_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

	/// Implements updating method derived from IService. Do nothing.
	FWSERVICES_API virtual void updating() throw ( ::fwTools::Failed );

	/// Implements info method derived from IService. Print classname.
	FWSERVICES_API virtual void info( std::ostream &_sstream );

	/**
	 * @brief React on modification, as an observer
	 *
	 * This is usefull automatically integrate and notify modifications of sub-object composing the object to which this service is attached to.
	 * One assumes that message event is related to a modification of a object composing IService::m_associatedObject.
	 * This does not install communications, it only embedds modification message into another one and notify it.
	 * @note could be moved to the IEditionService class as it is generic
	 */
	FWSERVICES_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
};

}

#endif // _FWSERVICES_DEFAULTEDITOR_HPP_
