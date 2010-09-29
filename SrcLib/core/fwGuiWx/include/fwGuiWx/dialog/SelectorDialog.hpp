/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_SELECTORDIALOG_HPP_
#define FWGUIWX_SELECTORDIALOG_HPP_

#include <vector>

#include <fwGui/dialog/ISelectorDialog.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   SelectorDialog allowing the choice of an element among severals (_selections)
 * @class   SelectorDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class FWGUIWX_CLASS_API SelectorDialog : public ::fwGui::dialog::ISelectorDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (SelectorDialog)(::fwGui::dialog::ISelectorDialog), (()), new SelectorDialog );

    /// Default constructor.
    FWGUIWX_API SelectorDialog() ;

    FWGUIWX_API virtual ~SelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIWX_API virtual void setSelections(std::vector< std::string > _selections);

    /**
     * @brief Sets the selector title.
     */
    FWGUIWX_API virtual void setTitle(std::string _title);

    /**
     * @brief Show the selector and return the selection.
     * @param[in] _parent Parent container for the selector.
     */
    FWGUIWX_API virtual std::string show();

private :

    std::vector< std::string > m_selections;

    std::string m_title;
};

} // namespace dialog
} // namespace fwGuiWx

#endif /*FWGUIWX_SELECTORDIALOG_HPP_*/
