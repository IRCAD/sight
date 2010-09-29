/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIQT_SELECTORDIALOG_HPP_
#define FWGUIQT_SELECTORDIALOG_HPP_

#include <vector>

#include <QDialog>
#include <QString>
#include <QWidget>

#include <fwGui/dialog/ISelectorDialog.hpp>
#include "fwGuiQt/config.hpp"


namespace fwGuiQt
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

class FWGUIQT_CLASS_API SelectorDialog : public ::fwGui::dialog::ISelectorDialog,  public QDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (SelectorDialog)(::fwGui::dialog::ISelectorDialog), (()), new SelectorDialog );

    /// Default constructor.
    FWGUIQT_API SelectorDialog() ;

    FWGUIQT_API virtual ~SelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIQT_API virtual void setSelections(std::vector< std::string > _selections);

    /**
     * @brief Sets the selector title.
     */
    FWGUIQT_API virtual void setTitle(std::string _title);

    /**
     * @brief Show the selector and return the selection.
     * @param[in] _parent Parent container for the selector.
     */
    FWGUIQT_API virtual std::string show();

private :

    std::vector< std::string > m_selections;

    std::string m_title;
};

} // namespace dialog
} // namespace fwGuiQt

#endif /*FWGUIQT_SELECTORDIALOG_HPP_*/
