/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_SELECTORDIALOG_HPP_
#define FWGUIWX_SELECTORDIALOG_HPP_

#include <vector>

#include <fwGui/dialog/IMultiSelectorDialog.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   MultiSelectorDialog allowing the choice of an element among severals (_selections)
 * @class   MultiSelectorDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class FWGUIWX_CLASS_API MultiSelectorDialog : public ::fwGui::dialog::IMultiSelectorDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog), (()), new MultiSelectorDialog );

    /// Default constructor.
    FWGUIWX_API MultiSelectorDialog() ;

    FWGUIWX_API virtual ~MultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIWX_API virtual void setSelections(Selections _selections);

    /**
     * @brief Sets the selector title.
     */
    FWGUIWX_API virtual void setTitle(std::string _title);

    /**
     * @brief Show the selector and return the selection.
     * @param[in] _parent Parent container for the selector.
     */
    FWGUIWX_API virtual Selections show();

    FWGUIWX_API virtual void setMessage(const std::string &msg);

private :

    Selections m_selections;
    /// Dialog box message
    std::string m_message;
    std::string m_title;
};

} // namespace dialog
} // namespace fwGuiWx

#endif /*FWGUIWX_SELECTORDIALOG_HPP_*/
