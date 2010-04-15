/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_TESTACTION_HPP_
#define _GUI_ACTION_TESTACTION_HPP_

#include <fwServices/IService.hpp>

#include "gui/export.hpp"
#include "gui/action/IAction.hpp"

namespace gui
{
namespace action
{
class Timer;
class Next;

/**
 * @brief   Test action runs test configuration running.
 * @class   TestAction.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 */
class GUI_CLASS_API TestAction : public ::gui::action::IAction
{

public :
   
    fwCoreServiceClassDefinitionsMacro ( (TestAction)(::gui::action::IAction) ) ;
    

    /**
     * @brief Constructor. Do nothing.(Just initialize parameters).
     *
     * By default, m_isStarted and m_isAutomatic is false.
     */
    GUI_API TestAction() throw() ;

    /*
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~TestAction() throw() ;

protected:

    GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

    GUI_API virtual void stopping() throw( ::fwTools::Failed ) ;

    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

protected :
    
    ::boost::shared_ptr< Timer > m_timer ; 
    ::boost::shared_ptr< Next > m_next ;    
    bool m_isStarted ;
    bool m_isAutomatic ;
    int  m_rate ;
    std::map< std::string , std::string > m_testSequence ;
    std::map< std::string , std::string >::iterator m_current;
    
};

} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_TESTACTION_HPP_*/
