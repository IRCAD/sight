/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CONTAINER_WXCONTAINER_HPP_
#define _FWGUIWX_CONTAINER_WXCONTAINER_HPP_

#include <wx/window.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwContainer.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace container
{

/**
 * @brief   Defines the wxWidgets container for IHM.
 * @class   WxContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API WxContainer : public ::fwGui::container::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxContainer)(::fwGui::container::fwContainer), (()), new WxContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API WxContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~WxContainer() throw() ;


    FWGUIWX_API virtual void clean();
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setWxContainer(wxWindow* container);
    FWGUIWX_API virtual wxWindow* getWxContainer();
    FWGUIWX_API virtual bool isShownOnScreen();

    FWGUIWX_API virtual void setVisible(bool isVisible);

private :

    wxWindow* m_container;
};

} // namespace container
} // namespace fwGuiWx

#endif /*_FWGUIWX_CONTAINER_WXCONTAINER_HPP_*/


