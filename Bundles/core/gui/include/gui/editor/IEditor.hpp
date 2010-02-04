/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_EDITOR_IEDITOR_HPP_
#define _GUI_EDITOR_IEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>
#include "gui/export.hpp"

class wxWindow;

namespace gui
{

namespace editor
{

/**
 * @brief	Defines the service interface managing the editor service for object.
 * @class	IEditor.
 * @author	IRCAD (Research and Development Team).

 * @date	2009.
 *
 * @todo ACH : Some services of IEditor type do not need a container (example: IOSelectorService that open a dialogue box).
 * 	The problem is that the IEditor::starting method create another useless container in this case.
 */

class GUI_CLASS_API IEditor : public fwServices::IService
{

public :
	fwCoreServiceClassDefinitionsMacro ( (IEditor)(::fwServices::IService::Baseclass) ) ;

	/**
	 * @brief	Constructor. Do nothing (Just initialize parameters).
	 *
	 * By default, the container ID is not defined and the container is not locally created.
	 */
	GUI_API IEditor() throw() ;

	///	Destructor. Do nothing.
	GUI_API virtual ~IEditor() throw() ;

	/**
	 * @brief Specify container within which visualization will appear.
	 * @todo Question: Should this method be imposed to class child declarations ??. It is interesting to check if the container is really needed.
	 */
	GUI_API virtual void setWxContainer( wxWindow * container ) ;

	/// Returns visualization wxContainer.
	GUI_API virtual wxWindow * getWxContainer() ;

protected :

	/** @name Service methods ( override from ::fwServices::IService )
	 * @{
	 */

	/// This method is used to find the container ID if it is defined in the configuration element.
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

	/**
	 * @brief This method retrieves or builds the container.
	 *
	 * If the container Id does not exist, the container is created (m_isContainerLocallyCreated = true).
	 * Else, the method retrieves and sets the container thanks to wxWidgets API (FindWindowById).
	 */
	GUI_API virtual void starting() throw(::fwTools::Failed);

	/// Stops the service and destroys the m_container if it has been locally created.
	GUI_API virtual void stopping() throw(::fwTools::Failed);

	/**
	 * @brief This method is used to give information about the service. Do nothing.
	 * @todo Must be not implemented and class child declaration should be imposed.
	 */
	GUI_API virtual void info(std::ostream &_sstream ) ;

	/**
	 * @brief Update/refresh/execute the service on an observation/notification. Do nothing.
	 * @todo Must be not implemented and class child declaration should be imposed.
	 */
	GUI_API virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;

	/**
	 * @brief Update/refresh/execute the service. Do nothing.
	 * @todo Must be not implemented and class child declaration should be imposed.
	 */
	GUI_API virtual void updating() throw(::fwTools::Failed) {} ;

	///@}

	/// container
	wxWindow * m_container;

	/**
	 * @brief Container ID.
	 *
	 * The first element of the std::pair is a boolean that is set to true if the configuration element is found.
	 * The second element of the std::pair is an int representing the identification number of a wxWidgets container. This number allows to retrieve the wxWidgets container and thus to initialize the m_container.
	 */
	std::pair< bool , int > m_guiContainerId ;

	/**
	 * @brief This boolean allows to know if a new container has been created inside the class instance in order to destroy it during the stopping function.
	 *
	 **/
	bool m_isContainerLocallyCreated ;

};

}
}

#endif /*_FWRENDER_RENDERINGSERVICE_HPP_*/


