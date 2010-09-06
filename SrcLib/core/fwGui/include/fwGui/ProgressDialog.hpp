/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_ProgressDialog_HPP_
#define _FWGUI_ProgressDialog_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/IProgressDialog.hpp"
namespace fwGui
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   ProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ProgressDialog : public IProgressDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ProgressDialog)(::fwGui::IProgressDialog), (()), new ProgressDialog );

    /// will instanciate the concrete implementation
    FWGUI_API ProgressDialog( const std::string &title = std::string() ,  const std::string &msg = std::string() );

    /// override
    FWGUI_API void setTitle(const std::string &title);

    /// override
    FWGUI_API void setMessage(const std::string &message);

    /// override
    FWGUI_API void operator()(float percent,std::string msg);


protected :

    ::fwGui::IProgressDialog::sptr m_implementation;

};

} // namespace fwGui

#endif /*_FWGUI_ProgressDialog_HPP_*/


