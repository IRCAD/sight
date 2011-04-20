/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISU_CROSSTYPE_ACTION_HPP_
#define _UIVISU_CROSSTYPE_ACTION_HPP_

#include <string>
#include <map>

#include <fwTools/Failed.hpp>
#include <fwGui/IActionSrv.hpp>

#include "uiVisu/config.hpp"

namespace uiVisu
{
namespace action
{

/**
 * @brief   This action allows change the cross type
 * @class   CameraOrientationAction.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISU_CLASS_API CrossTypeAction : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (CrossTypeAction)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    UIVISU_API CrossTypeAction() throw() ;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~CrossTypeAction() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Starts action
    virtual void starting() throw(::fwTools::Failed);

    /// Stops action
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Notifies message to change cross type
    void updating() throw(::fwTools::Failed);

    /// Does nothing
    void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure action.
     * @verbatim
    <service type="::fwGui::IActionSrv" implementation="::uiData::action::CrossTypeAction" autoComChannel="no">
        <crossType>hide</crossType>
    </service>
    @endverbatim
     * - \b crossType : must be full, half or hide
     */
    void configuring() throw(fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;

private:

    static std::map< std::string, float > m_scaleConversion;
    std::string m_crossType;
};

} // action
} // uiVisu

#endif /*_UIVISU_CROSSTYPE_ACTION_HPP_*/


