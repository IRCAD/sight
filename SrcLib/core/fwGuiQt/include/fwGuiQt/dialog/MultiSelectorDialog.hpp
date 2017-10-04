/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_DIALOG_MULTISELECTORDIALOG_HPP__
#define __FWGUIQT_DIALOG_MULTISELECTORDIALOG_HPP__

#include "fwGuiQt/config.hpp"

#include <fwGui/dialog/IMultiSelectorDialog.hpp>

#include <QDialog>
#include <QString>
#include <QWidget>

#include <vector>

namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   MultiSelectorDialog allowing the choice of an element among severals (_selections)
 */

class FWGUIQT_CLASS_API MultiSelectorDialog : public ::fwGui::dialog::IMultiSelectorDialog,
                                              public QDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog),
                                            (()),
                                            ::fwGui::factory::New< MultiSelectorDialog > );

    FWGUIQT_API MultiSelectorDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~MultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIQT_API virtual void setSelections(Selections _selections) override;

    /**
     * @brief Sets the selector title.
     */
    FWGUIQT_API virtual void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUIQT_API virtual Selections show() override;

    FWGUIQT_API virtual void setMessage(const std::string& msg) override;

private:

    Selections m_selections;
    /// Dialog box message
    std::string m_message;
    std::string m_title;
};

} // namespace dialog
} // namespace fwGuiQt

#endif /*__FWGUIQT_DIALOG_MULTISELECTORDIALOG_HPP__*/
