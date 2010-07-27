/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CONTAINER_WXTOOLBARCONTAINER_HPP_
#define _FWGUIWX_CONTAINER_WXTOOLBARCONTAINER_HPP_

#include <wx/toolBar.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwToolBar.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace container
{

/**
 * @brief   Defines the wxWidgets toolBar bar container for IHM.
 * @class   WxToolBarContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API WxToolBarContainer : public ::fwGui::fwToolBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxToolBarContainer)(::fwGui::fwToolBar), (()), new WxToolBarContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API WxToolBarContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~WxToolBarContainer() throw() ;


    FWGUIWX_API virtual void clean();
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setWxToolBar(wxToolBar* toolBar);
    FWGUIWX_API virtual wxToolBar* getWxToolBar();

private :

    wxToolBar* m_toolBar;
};

} // namespace container
} // namespace fwGuiWx

#endif /*_FWGUIWX_CONTAINER_WXTOOLBARCONTAINER_HPP_*/


