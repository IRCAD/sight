/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_EDITOR_SMATRIXVIEWER_HPP__
#define __UITOOLS_EDITOR_SMATRIXVIEWER_HPP__

#include "uiTools/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QVector>

class QLabel;

namespace uiTools
{

namespace editor
{

/**
 * @brief  This class defines a viewer for a fwData::TransformationMatrix3D.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." impl="::uiTools::editor::SMatrixViewer" autoConnect="yes">
        <title>matrix name</title>
    </service>
   @endcode

 * @subsection Input Input:
 * - \\b matrix [::fwData::TransformationMatrix3D]: matrix to display.
 */

class UITOOLS_CLASS_API SMatrixViewer : public QObject,
                                        public ::gui::editor::IEditor
{
public:
    fwCoreServiceClassDefinitionsMacro( (SMatrixViewer)(::gui::editor::IEditor) );

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

    /**
     * @brief This method is used to configure the service parameters:
     */

    UITOOLS_API virtual void configuring() override;

    ///This method launches the ::fwGui::IGuiContainerSrv::create method.
    UITOOLS_API virtual void starting() override;

    ///This method launches the ::fwGui::IGuiContainerSrv::destroy method.
    UITOOLS_API virtual void stopping() override;

    /// Updates Slider value
    UITOOLS_API virtual void updating() override;

private:

    /// Update the editor when the matrix changes
    void updateFromMatrix();
    /// Clear datas
    void clearLabels();

    QPointer< QLabel > m_description; ///< description of camera series

    QVector< QPointer< QLabel > > m_matrixLabels; ///< Labels for matrix's elements

    std::string m_title; ///< name of the matrix (will be displayed)
};

} //namespace editor

} // namespace uiTools

#endif // __UITOOLS_EDITOR_SMATRIXVIEWER_HPP__
