/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
