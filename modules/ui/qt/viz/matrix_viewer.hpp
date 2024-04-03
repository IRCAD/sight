/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <data/matrix4.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>
#include <QVector>

class QLabel;

namespace sight::module::ui::qt::viz
{

/**
 * @brief  This class defines a viewer for a data::matrix4.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::ui::qt::viz::matrix_viewer">
        <in key="matrix" uid="..." />
        <title>matrix name</title>
    </service>
   @endcode

 * @subsection Input Input
 * - \b matrix [sight::data::matrix4]: matrix to display
 *
 * @subsection Configuration Configuration
 * - \b title (optional): defines the displayed title on top of the matrix viewer (default: matrix).
 */

class matrix_viewer : public QObject,
                      public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(matrix_viewer, sight::ui::editor);

    /// Constructor. Do nothing.
    matrix_viewer() noexcept;

    /// Destructor. Do nothing.
    ~matrix_viewer() noexcept override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Matrix4::MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

protected:

    /// Configures the title of the matrix viewer
    void configuring() override;

    /// Initializes the layout of the matrix viewer
    void starting() override;

    /// Destroys the layout
    void stopping() override;

    /// Updates the matrix values to display
    void updating() override;

private:

    /// Updates the view when the matrix changes
    void update_from_matrix();

    /// Clears matrix values
    void clear_labels();

    std::string m_title; ///< Title of the matrix that will be displayed

    QVector<QPointer<QLabel> > m_matrix_labels; ///< Labels for matrix's elements

    static constexpr std::string_view MATRIX = "matrix";

    data::ptr<data::matrix4, data::access::in> m_matrix {this, MATRIX, true};
};

} // namespace sight::module::ui::qt::viz
