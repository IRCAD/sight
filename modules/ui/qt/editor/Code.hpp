/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <core/tools/Failed.hpp>

#include <QObject>
#include <QPointer>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextEdit>

#include <ui/base/editor/IEditor.hpp>

namespace sight::modules::ui::qt
{

namespace editor
{

/**
 * @brief   This service displays a code editor and works on a data::String.
 *
 * It provides highlighting for python and C++.
 *
 * @section XML XML Configuration
 *  @code{.xml}
    <service uid="codeEditor" type="::sight::modules::ui::qt::editor::Code" autoConnect="yes">
        <inout key="string" uid="..." />
        <config>
            <language name="Python" />
        </config>
    </service>
    @endcode
 * @subsection In-Out In-Out
 * - \b string [sight::data::String]: string containing the code
 * @subsection Configuration Configuration
 * - \b language name: can be "Python" or "Cpp"
 */
class MODULE_UI_QT_CLASS_API Code : public QObject,
                                    public sight::ui::base::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceMacro(Code, sight::ui::base::editor::IEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API Code() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~Code() noexcept;

protected:

    /// Install the layout.
    virtual void starting() override;

    /// Destroy the layout
    virtual void stopping() override;

    /// Update the value from the String object.
    virtual void updating() override;

    /// Update the value from the String object.
    virtual void swapping() override;

    /// Parses the configuration
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

protected Q_SLOTS:
    /**
     * @brief This method is called when the value change.
     *
     * Update the String object value and notify the modification.
     */
    void onModifyValue();

private:

    static const std::string s_PYTHON;
    static const std::string s_CPP;

    std::string m_language;
    QPointer< QTextEdit >          m_valueCtrl;
    QPointer< QSyntaxHighlighter > m_highlighter;
};

} // namespace editor

} // namespace sight::modules::ui::qt
