/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_SAMPLE_DUMMYTEST_HPP_
#define _GUI_SAMPLE_DUMMYTEST_HPP_

#include "gui/export.hpp"
#include "gui/sample/ITestService.hpp"

namespace gui
{
namespace sample
{

class DummyAction ;
/**
 * @brief	Defines a dummy service using a view and implementing a callback to a menu action
 * @note for example purpose
 * @author	IRCAD (Research and Development Team).
 */
class GUI_CLASS_API DummyTest : public ::gui::sample::ITestService
{

public :

	GUI_API DummyTest() throw() ;
	GUI_API virtual ~DummyTest() throw() ;
	GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;
	GUI_API virtual void starting() throw(::fwTools::Failed);
	GUI_API virtual void stopping() throw(::fwTools::Failed);
	GUI_API virtual void updating() throw(::fwTools::Failed);
	GUI_API virtual void info(std::ostream &_sstream ) ;

	GUI_API void changeColor() ;

private :

	int m_wxContainerId ;
	int m_actionId ;
	std::string m_color ;
	::boost::shared_ptr< DummyAction > m_localAction ;

};



} // namespace sample
} // namespace gui


#endif /*_GUI_SAMPLE_DUMMYTEST_HPP_*/
