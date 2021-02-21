/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <QObject>
#include <QPointer>
#include <QVector>

#include <ui/base/IEditor.hpp>

class QLabel;

namespace sight::module::ui::qt
{

namespace viz
{

/**
 * @brief  This class defines a viewer for a data::Matrix4.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::sight::module::ui::qt::viz::SMatrixViewer">
        <in key="matrix" uid="..." />
        <title>matrix name</title>
    </service>
   @endcode

 * @subsection Input Input
 * - \b matrix [sight::data::Matrix4]: matrix to display
 *
 * @subsection Configuration Configuration
 * - \b title (optional): defines the displayed title on top of the matrix viewer (default: matrix).
 */

class MODULE_UI_QT_CLASS_API SMatrixViewer : public QObject,
                                             public sight::ui::base::IEditor
{
public:
    fwCoreServiceMacro(SMatrixViewer, sight::ui::base::IEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SMatrixViewer() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SMatrixViewer() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Matrix4::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Configures the title of the matrix viewer
    MODULE_UI_QT_API virtual void configuring() override;

    /// Initializes the layout of the matrix viewer
    MODULE_UI_QT_API virtual void starting() override;

    /// Destroys the layout
    MODULE_UI_QT_API virtual void stopping() override;

    /// Updates the matrix values to display
    MODULE_UI_QT_API virtual void updating() override;

private:

    /// Updates the view when the matrix changes
    void updateFromMatrix();

    /// Clears matrix values
    void clearLabels();

    std::string m_title; ///< Title of the matrix that will be displayed

    QVector< QPointer< QLabel > > m_matrixLabels; ///< Labels for matrix's elements
};

} //namespace viz

} // namespace sight::module::ui::qt
