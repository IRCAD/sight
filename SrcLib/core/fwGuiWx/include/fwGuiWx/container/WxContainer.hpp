///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_WXCONTAINER_HPP_
#define _FWGUIWX_WXCONTAINER_HPP_

#include <wx/window.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwContainer.hpp>

#include "fwGuiWX/config.hpp"

namespace fwGuiWX
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
class FWGUIWX_CLASS_API WxContainer : public ::fwGui::fwContainer
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxContainer)(::fwGui::fwContainer), (()), new WxContainer );

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

private :

    wxWindow* m_container;
};

} // namespace container
} // namespace fwGuiWX

#endif /*_FWGUIWX_WXCONTAINER_HPP_*/


