/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
