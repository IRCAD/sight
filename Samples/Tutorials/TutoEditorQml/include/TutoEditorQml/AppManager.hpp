/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "TutoEditorQml/config.hpp"

#include <fwData/String.hpp>

#include <fwServices/AppManager.hpp>

#include <QObject>

namespace TutoEditorQml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTOEDITORQML_CLASS_API AppManager : public QObject,
                                           public ::fwServices::AppManager
{

Q_OBJECT;
public:
    /// Constructor.
    TUTOEDITORQML_API AppManager() noexcept;

    /// Destructor. Do nothing.
    TUTOEDITORQML_API ~AppManager() noexcept;

public Q_SLOTS:

    /// Initialize the manager
    void initialize();

    /// Uninitialize the manager
    void uninitialize();

    /// get the created qmlEditor and register it
    void onServiceCreated(const QVariant& obj);

private:

    ::fwData::String::sptr m_string;
};

} // namespace TutoEditorQml
