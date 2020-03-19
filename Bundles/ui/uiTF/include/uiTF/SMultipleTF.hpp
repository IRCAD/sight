/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "uiTF/config.hpp"

#include <fwCore/macros.hpp>

#include <fwData/Composite.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include <filesystem>

namespace uiTF
{

class UITF_CLASS_API SMultipleTF final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(::uiTF::SMultipleTF, ::fwGui::editor::IEditor)

    UITF_API SMultipleTF();

    UITF_API virtual ~SMultipleTF() noexcept;

private:

    virtual void configuring() override;

    virtual void starting() override;

    virtual void updating() override;

    virtual void stopping() override;

    virtual void swapping(const KeyType& _key) override;

    virtual KeyConnectionsMap getAutoConnections() const override;

    bool hasPoolName(const std::string& _name) const;

    std::string createPoolName(const std::string& _basename) const;

    void initializePools();

    void updatePoolsPreset();

    void presetChoice(int _index);

    void setCurrentPool();

    void deletePool();

    void newPool();

    void copyPool();

    void reinitializePools();

    void renamePool();

    bool m_tfPerPath { false };

    /// Stores path were looking for TF presets.
    std::vector< std::filesystem::path > m_paths;

    /// Contains the list of all TF preset.
    QComboBox* m_tfPoolsPreset;

    /// Contains the delete TF pool button.
    QPushButton* m_deleteButton;

    /// Contains the new TF pool button.
    QPushButton* m_newButton;

    /// Contains the copy TF pool button.
    QPushButton* m_copyButton;

    /// Contains the reset TF pool button.
    QPushButton* m_reinitializeButton;

    /// Contains the rename TF pool button.
    QPushButton* m_renameButton;

    /// Contains the current selected TF pool.
    ::fwData::Composite::sptr m_currentTFPool;

};

}
