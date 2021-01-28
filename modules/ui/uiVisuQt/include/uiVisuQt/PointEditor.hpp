/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "uiVisuQt/config.hpp"

#include <core/tools/Failed.hpp>

#include <data/Point.hpp>
#include <data/tools/PickingInfo.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

#include <ui/base/editor/IEditor.hpp>

namespace uiVisuQt
{

/**
 * @brief   PointEditor service allows to display point information.
 */
class UIVISUQT_CLASS_API PointEditor : public QObject,
                                       public sight::ui::base::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(PointEditor, sight::ui::base::editor::IEditor)

    /// Constructor. Do nothing.
    UIVISUQT_API PointEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~PointEditor() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() override;

    ///This method launches the IEditor::stopping method.
    virtual void stopping() override;

    void updating() override;

    void swapping() override;

    void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

private:

    /// Slot: get the interaction information
    void getInteraction(data::tools::PickingInfo info);

    QPointer< QLineEdit >   m_textCtrl_x;
    QPointer< QLineEdit >   m_textCtrl_y;
    QPointer< QLineEdit >   m_textCtrl_z;

};

} // uiData
