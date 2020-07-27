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

#include "TutoSceneQt3DQml/config.hpp"

#include <fwRenderQt3D/core/GenericScene.hpp>
#include <fwRenderQt3D/data/Mesh.hpp>

#include <fwServices/AppManager.hpp>

#include <QObject>
#include <QPointer>

namespace TutoSceneQt3DQml
{

/**
 * @brief This class manages the services used by the Application. It is launched by the qml file 'ui.qml'.
 */
class TUTOSCENEQT3DQML_CLASS_API AppManager : public QObject,
                                              public ::fwServices::AppManager
{

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(::fwRenderQt3D::data::Mesh* mesh READ getMesh WRITE setMesh NOTIFY meshChanged)
Q_PROPERTY(::fwRenderQt3D::core::GenericScene* scene READ getScene WRITE setScene NOTIFY sceneChanged)

public:

    /// Creates the app manager.
    TUTOSCENEQT3DQML_API AppManager();

    /// Destroys the app manager.
    TUTOSCENEQT3DQML_API ~AppManager() override;

    /// @returns the mesh converted by the mesh adaptor. Needed for QML property.
    TUTOSCENEQT3DQML_API ::fwRenderQt3D::data::Mesh* getMesh();

    /// @returns the scene created by SRender service. Needed for QML property.
    TUTOSCENEQT3DQML_API ::fwRenderQt3D::core::GenericScene* getScene();

    /// Updates the mesh associated to the app manager. Needed for QML property.
    TUTOSCENEQT3DQML_API void setMesh(::fwRenderQt3D::data::Mesh* _mesh);

    /// Updates the scene associated to the app manager. Needed for QML property.
    TUTOSCENEQT3DQML_API void setScene(::fwRenderQt3D::core::GenericScene* _mesh);

Q_SIGNALS:

    /// Signal emitted when the mesh is modified.
    void meshChanged();

    /// Signal emitted when the scene is modified.
    void sceneChanged();

public Q_SLOTS:

    /// Calls to create the app manager.
    void initialize();

    /// Calls to stop the app manager.
    void uninitialize();

    /// Calls to open a mesh reader and update m_mesh.
    void onOpenModel();

private:

    /// Contains the mesh associated to the app manager.
    QPointer< ::fwRenderQt3D::data::Mesh > m_mesh;

    /// Contains the scene associated to the app manager.
    QPointer< ::fwRenderQt3D::core::GenericScene > m_scene;

};

} // namespace TutoSceneQt3DQml.
