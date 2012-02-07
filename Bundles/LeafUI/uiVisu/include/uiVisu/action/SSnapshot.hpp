/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISU_ACTION_SSNAPSHOT_HPP__
#define __UIVISU_ACTION_SSNAPSHOT_HPP__

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
 * @brief   This action allows to snapshot a generic scene
 * @class   SSnapshot.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 *
 * Send a CompositeMsg with SNAP event containing the uid of the scene to the scene's composite.
 */
class UIVISU_CLASS_API SSnapshot : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SSnapshot)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    UIVISU_API SSnapshot() throw() ;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~SSnapshot() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Starts action
    virtual void starting() throw(::fwTools::Failed);

    /// Stops action
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Show a file dialog and notify the scene which must be printed.
    void updating() throw(::fwTools::Failed);

    /// Does nothing
    void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure action.
     * @verbatim
    <service type="::fwGui::IActionSrv" implementation="::uiData::action::SSnapshot" autoComChannel="no">
        <snap>
            <scene uid="genericScene" />
        </snap>
    </service>
    @endverbatim
     * - \b genericScene is the uid of the ::fwRenderVTK::VtkRenderService representing the generic scene which will be printed.
     */
    void configuring() throw(fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;

private:
    std::string requestFileName();

    std::vector< std::string > m_scenesUID;

};

} // action
} // uiVisu

#endif // __UIVISU_ACTION_SSNAPSHOT_HPP__


