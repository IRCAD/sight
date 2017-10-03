/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SMESHESBOXWIDGET_HPP__
#define __VISUVTKADAPTOR_SMESHESBOXWIDGET_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Composite.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#ifndef ANDROID

class vtkBoxWidget2;
class vtkCommand;
class vtkPolyData;
class vtkAssembly;
class vtkActor;

namespace visuVTKAdaptor
{

/**
 * @brief   Create a Box widget around the meshes contained in the composite.
 *
 * @section Slots Slots
 * - \b updateMatrices() : updates the matrices from the meshes
 * - \b addObject(::fwData::Composite::ContainerType) : updates the box widget with the added meshes
 * - \b changeObject(::fwData::Composite::ContainerType, ::fwData::Composite::ContainerType) : updates the box widget
 *      with the current meshes
 * - \b removeObjects(::fwData::Composite::ContainerType) : updates the box widget with the current meshes
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SMeshesBoxWidget" autoConnect="yes">
       <in key="composite" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b composite [::fwData::Composite]: Composite containing meshes, the box widget will be around this meshes and when
 *   the box is moved, the meshes transform (in a field) will be modified.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the mesh is displayed
 */
class VISUVTKADAPTOR_CLASS_API SMeshesBoxWidget : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMeshesBoxWidget)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SMeshesBoxWidget() noexcept;

    VISUVTKADAPTOR_API virtual ~SMeshesBoxWidget() noexcept;

    /// Updates meshes transformation matrix from vtk box widget transform
    void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Updates vtk transformation from data meshes
    void updateMeshTransform();

    /// Updates map from composite meshes
    void updateMeshMapFromComposite(::fwData::Composite::ContainerType objects);

private:

    /**
     * @name Slots methods
     * @{
     */
    /// Slot: Updates the matrices
    void updateMatrices();

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);
    /**
     * @}
     */

    typedef std::map< std::string, ::fwCom::Connection > ConnectionMapType;
    typedef std::map< std::string, vtkActor* > MeshMapType;

    MeshMapType m_meshMap;
    ConnectionMapType m_connections;

    vtkAssembly* m_assembly;
    vtkCommand* m_boxWidgetCommand;
    vtkBoxWidget2* m_vtkBoxWidget;
};

} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_SMESHESBOXWIDGET_HPP__
