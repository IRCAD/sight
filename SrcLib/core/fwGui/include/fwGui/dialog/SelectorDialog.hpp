/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_SELECTORDIALOG_HPP_
#define _FWGUI_SELECTORDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/ISelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   SelectorDialog allowing the choice of an element among severals (_selections)
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   SelectorDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API SelectorDialog : public ISelectorDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (SelectorDialog)(::fwGui::dialog::ISelectorDialog), (()), new SelectorDialog );

    /// will instanciate the concrete implementation
    FWGUI_API SelectorDialog();

    /// override
    FWGUI_API void setTitle(std::string title);

    /// override
    FWGUI_API virtual void setSelections(std::vector< std::string > _selections);

    /// override
    FWGUI_API std::string show();

protected :

    ::fwGui::dialog::ISelectorDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_SELECTORDIALOG_HPP_*/


