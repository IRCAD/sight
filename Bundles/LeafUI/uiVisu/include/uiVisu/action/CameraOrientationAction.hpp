/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISU_CAMERA_ORIENTATION_ACTION_HPP_
#define _UIVISU_CAMERA_ORIENTATION_ACTION_HPP_

#include <fwTools/Failed.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiVisu/config.hpp"

namespace uiVisu
{
namespace action
{

/**
 * @brief   This action allows change 3D camera orientation (axial, frontal, sagittal)
 * @class   CameraOrientationAction.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISU_CLASS_API CameraOrientationAction : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (CameraOrientationAction)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    UIVISU_API CameraOrientationAction() throw() ;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~CameraOrientationAction() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Configure action.
     * @verbatim
    <service type="::fwGui::IActionSrv" implementation="::uiData::action::CameraOrientationAction" autoComChannel="no">
        <orientation>frontal</orientation>
    </service>
    @endverbatim
     * - \b orientation : must be axial, frontal or sagittal
     */
    void configuring() throw(fwTools::Failed);

    /// Starts action
    virtual void starting() throw(::fwTools::Failed);

    /// Stops action
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Notifies message to change camera orientation
    void updating() throw(::fwTools::Failed);

    /// Does nothing
    void swapping() throw(::fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

private:

    std::string m_orientation;
};

} // action
} // uiVisu

#endif /*_UIVISU_CAMERA_ORIENTATION_ACTION_HPP_*/


