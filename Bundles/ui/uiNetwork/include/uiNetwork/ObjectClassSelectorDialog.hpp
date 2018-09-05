/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_OBJECTCLASSSELECTORDIALOG_HPP__
#define __UINETWORK_OBJECTCLASSSELECTORDIALOG_HPP__

#include "uiNetwork/config.hpp"

#include <QDialog>
#include <QComboBox>
#include <QString>
#include <string>

namespace uiNetwork
{
/**
 *
 * @brief object class selector dialog when user can choose object to add
 */
class UINETWORK_CLASS_API ObjectClassSelectorDialog : public QDialog
{

Q_OBJECT
public:
    /// constructor
    ObjectClassSelectorDialog();

    /// destructor
    ~ObjectClassSelectorDialog();

    /**
     * @brief get classname selected
     */
    std::string const& getClassnameSelected() const;

Q_SIGNALS:
    /**
     * @brief signal to emit when user confirm the selection
     */
    void classnameSelected(std::string const& classname);
private Q_SLOTS:

    /**
     * @brief when user confirm selection then emit classnameSelected signal
     */
    void    onValidateSelection();


private:
    /// classname selected
    std::string m_classnameSelected;

    /// list of fwData::Object classname available
    QComboBox* m_classnameBox;
};
} //namespace uiNetwork

#endif // __UINETWORK_OBJECTCLASSSELECTORDIALOG_HPP__
