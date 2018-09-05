/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiTools/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QVector>

class QLabel;

namespace uiTools
{

namespace editor
{

/**
 * @brief  This class defines a viewer for a ::fwData::TransformationMatrix3D.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::uiTools::editor::SMatrixViewer">
        <in key="matrix" uid="..." />
        <title>matrix name</title>
    </service>
   @endcode

 * @subsection Input Input
 * - \b matrix [::fwData::TransformationMatrix3D]: matrix to display
 *
 * @subsection Configuration Configuration
 * - \b title (optional): defines the displayed title on top of the matrix viewer (default: matrix).
 */

class UITOOLS_CLASS_API SMatrixViewer : public QObject,
                                        public ::fwGui::editor::IEditor
{
public:
    fwCoreServiceClassDefinitionsMacro( (SMatrixViewer)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UITOOLS_API SMatrixViewer() noexcept;

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SMatrixViewer() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect TransformationMatrix3D::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    UITOOLS_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Configures the title of the matrix viewer
    UITOOLS_API virtual void configuring() override;

    /// Initializes the layout of the matrix viewer
    UITOOLS_API virtual void starting() override;

    /// Destroys the layout
    UITOOLS_API virtual void stopping() override;

    /// Updates the matrix values to display
    UITOOLS_API virtual void updating() override;

private:

    /// Updates the view when the matrix changes
    void updateFromMatrix();

    /// Clears matrix values
    void clearLabels();

    std::string m_title; ///< Title of the matrix that will be displayed

    QVector< QPointer< QLabel > > m_matrixLabels; ///< Labels for matrix's elements
};

} //namespace editor

} // namespace uiTools
