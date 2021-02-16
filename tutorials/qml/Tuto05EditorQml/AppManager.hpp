/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto05EditorQml/config.hpp"

#include <data/String.hpp>

#include <service/AppManager.hpp>

#include <QObject>

namespace Tuto05EditorQml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO05EDITORQML_CLASS_API AppManager : public QObject,
                                             public sight::service::AppManager
{

Q_OBJECT;
public:
    /// Constructor.
    TUTO05EDITORQML_API AppManager() noexcept;

    /// Destructor. Do nothing.
    TUTO05EDITORQML_API ~AppManager() noexcept;

public Q_SLOTS:

    /// Initialize the manager
    void initialize();

    /// Uninitialize the manager
    void uninitialize();

    /// get the created qmlEditor and register it
    void onServiceCreated(const QVariant& obj);

private:

    sight::data::String::sptr m_string;
};

} // namespace Tuto05EditorQml
