/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MESH_HPP__
#define __VISUVTKADAPTOR_MESH_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;
class vtkDepthSortPolyData;
class vtkAlgorithm;
class vtkAlgorithmOutput;
class vtkPolyDataNormals;
class vtkPlaneCollection;
class vtkActorCollection;
class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTransform;

namespace fwData
{
class Mesh;
class Material;
}

namespace visuVTKAdaptor
{

class Transform;

class VISUVTKADAPTOR_CLASS_API MeshVtkCommand;

/**
 * @brief   Display a ::fwData::Mesh in the generic scene
 *
 * @section Slots Slots
 * - \b textureApplied() : a texture was applied.
 * - \b updateVisibility(bool) : change the visibility of the mesh.
 * - \b updatePointColors() : the mesh points colors were modified.
 * - \b updateCellColors() : the mesh cells colors were modified.
 * - \b updatePointNormals() : the mesh points normals were modified.
 * - \b updateCellNormals() : the mesh cells normals were modified.
 * - \b updatePointTexCoords() : the mesh points texture coordinates were modified.
 * - \b updateCellTexCoords() : the mesh cells texture coordinates were modified.
 * - \b showPointColors() : display the points colors (instead of cell colors).
 * - \b showCellColors() : display the cells colors (instead of point colors).
 * - \b hideColors() : hide the cells colors.
 * - \b updateColorMode(std::uint8_t) : update the color mode (0 : hide, 1 : points colors, 2 : cells colors).

 * @section XML XML Configuration
 *
 * @code{.xml}
        <adaptor type="::visuVTKAdaptor::Mesh">
            <config renderer="default" picker="" texture="TextureAdaptor" />
       </adaptor>
   @endcode
 * @subsection Configuration Configuration:
 * - \b color : hexadecimal color code of the diffuse material (i.e RGB "#ff00ff" or RGBA "#ff224508"). This overrides
 * the current diffuse color of the mesh material.
 * - \b unclippedcolor : hexadecimal color code of the diffuse material of the unclipped part.
 * - \b autoresetcamera : reset the camera point of view when the mesh is modified ("yes" or "no", default: "yes") .
 * - \b uvgen : generate UV coordinates ("sphere", "cylinder" or "plane").
 * - \b texture : id of the optional texture adaptor, used to map a texture on the mesh. The mesh needs a valid UV
 * coordinates layer, or to generate uv on-the-fly with the \b uvgen parameter.
 * - \b shadingMode : "ambient", "flat", "gouraud" or "phong" (default: phong).
 */
class VISUVTKADAPTOR_CLASS_API Mesh : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (Mesh)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API Mesh() noexcept;
    VISUVTKADAPTOR_API virtual ~Mesh() noexcept;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwData::Material) ) > TextureAppliedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_APPLIED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_COLORS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePointColorsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_COLORS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateCellColorsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VERTEX_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateVertexSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePointNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateCellNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_TEX_COORDS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePointTexCoordsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_TEX_COORDS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateCellTexCoordsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_POINT_COLORS_SLOT;
    typedef ::fwCom::Slot<void ()> ShowPointColorsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_CELL_COLORS_SLOT;
    typedef ::fwCom::Slot<void ()> ShowCellColorsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_HIDE_COLORS_SLOT;
    typedef ::fwCom::Slot<void ()> HideColorsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_COLOR_MODE_SLOT;
    typedef ::fwCom::Slot<void (std::uint8_t)> UpdateColorModeSlotType; ///< color mode (0: none, 1: point, 2: cell)

    /// Slot: update mesh visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );
    /** @} */

    VISUVTKADAPTOR_API SPTR(::fwData::Material) getMaterial() const;
    VISUVTKADAPTOR_API void setMaterial(SPTR(::fwData::Material) material);

    VISUVTKADAPTOR_API SPTR(::fwData::Material) getUnclippedMaterial() const;
    VISUVTKADAPTOR_API void setUnclippedPartMaterial(SPTR(::fwData::Material) material);

    VISUVTKADAPTOR_API void setShowClippedPart ( bool show );
    VISUVTKADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::SRender::VtkObjectIdType id );

    VISUVTKADAPTOR_API void setVtkClippingPlanes               ( vtkPlaneCollection* planes );
    VISUVTKADAPTOR_API void setActorPropertyToUnclippedMaterial( bool opt );

    /// Active/Inactive automatic reset on camera. By default =true.
    VISUVTKADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    VISUVTKADAPTOR_API bool getVisibility() const;

    /// Creates normals adaptor if material has NORMALS, else remove normals adaptor.
    VISUVTKADAPTOR_API void updateOptionsMode();

    /// Used to update normal mode (0: none, 1: point, 2: cell)
    VISUVTKADAPTOR_API void updateNormalMode(std::uint8_t mode);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_POINT_COLORS_MODIFIED_SIG to this::s_UPDATE_POINT_COLORS_SLOT
     * Connect mesh::s_CELL_COLORS_MODIFIED_SIG to this::s_UPDATE_CELL_COLORS_SLOT
     * Connect mesh::s_VERTEX_MODIFIED_SIG to this::s_UPDATE_VERTEX_SLOT
     * Connect mesh::s_POINT_NORMALS_MODIFIED_SIG to this::s_UPDATE_POINT_NORMALS_SLOT
     * Connect mesh::s_CELL_NORMALS_MODIFIED_SIG to this::s_UPDATE_CELL_NORMALS_SLOT
     * Connect mesh::s_POINT_TEX_COORDS_MODIFIED_SIG to this::s_UPDATE_POINT_TEX_COORDS_SLOT
     * Connect mesh::s_CELL_TEX_COORDS_MODIFIED_SIG to this::s_UPDATE_CELL_TEX_COORDS_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart    ();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doStop     ();
    VISUVTKADAPTOR_API void doSwap   ();
    VISUVTKADAPTOR_API void doUpdate   ();

    /**
     * @name Slots methods
     * @{
     */

    /// Slot:  used to update point colors
    void updatePointColors();

    /// Slot: used to update cell colors
    void updateCellColors();

    /// Slot: used to update mesh vertex
    void updateVertex();

    ///Slot: used to update point normals
    void updatePointNormals();

    /// Slot: used to update cell normals
    void updateCellNormals();

    /// Slot: used to update point tex coords
    void updatePointTexCoords();

    /// Slot: used to update cell tex coords
    void updateCellTexCoords();

    /// Slot: used to show point colors
    void showPointColors();

    /// Slot: used to show cell colors
    void showCellColors();

    /// Slot: used to hide colors
    void hideColors();

    /// Slot: used to update color mode (0: none, 1: point, 2: cell)
    void updateColorMode(std::uint8_t mode);
    /**
     * @}
     */

    vtkActor* newActor();
    void buildPipeline();

    void updateMesh( SPTR(::fwData::Mesh) mesh );

    void setServiceOnMaterial(::fwRenderVTK::IAdaptor::sptr &srv,
                              SPTR(::fwData::Material) material);

    void removePlaneCollectionShifterCommand();
    void removeServicesStarterCommand();
    void createServicesStarterCommand();

    void createTransformService();
    void createNormalsService();
    void removeNormalsService();

    bool m_showClippedPart;
    bool m_autoResetCamera;

    vtkPolyData* m_polyData;
    vtkPolyDataMapper* m_mapper;
    vtkActor* m_actor;

    vtkPlaneCollection* m_clippingPlanes;
    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    MeshVtkCommand* m_planeCollectionShifterCallback;
    MeshVtkCommand* m_servicesStarterCallback;

    SPTR(::fwData::Material)       m_material;
    SPTR(::fwData::Material)       m_unclippedPartMaterial;

    ::fwRenderVTK::IAdaptor::wptr m_materialService;
    ::fwRenderVTK::IAdaptor::wptr m_unclippedPartMaterialService;
    ::fwRenderVTK::IAdaptor::wptr m_normalsService;

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

    /**
     * @brief Type for generation of texture coordinates
     */
    enum UvGenType
    {
        NONE,
        SPHERE,
        CYLINDER,
        PLANE,
    };

    UvGenType m_uvgen;
    std::string m_textureAdaptorUID;

    /// Signal to emit when a texture must be applied on a material.
    TextureAppliedSignalType::sptr m_sigTextureApplied;

private:

    /**
     * @name Slots attributes
     * @{
     */
    UpdateVisibilitySlotType::sptr m_slotUpdateVisibility; ///< slot used to change the mesh visibility
    UpdatePointColorsSlotType::sptr m_slotUpdatePointColors; ///< slot used to update point colors
    UpdateCellColorsSlotType::sptr m_slotUpdateCellColors; ///< slot used to update cell colors
    UpdateVertexSlotType::sptr m_slotUpdateVertex; ///< slot used to update mesh vertex
    UpdatePointNormalsSlotType::sptr m_slotUpdatePointNormals; ///< slot used to update point normals
    UpdateCellNormalsSlotType::sptr m_slotUpdateCellNormals; ///< slot used to update cell normals
    UpdatePointTexCoordsSlotType::sptr m_slotUpdatePointTexCoords; ///< slot used to update point tex coords
    UpdateCellTexCoordsSlotType::sptr m_slotUpdateCellTexCoords; ///< slot used to update cell tex coords
    ShowPointColorsSlotType::sptr m_slotShowPointColors; ///< slot used to show point colors
    ShowCellColorsSlotType::sptr m_slotShowCellColors; ///< slot used to show cell colors
    HideColorsSlotType::sptr m_slotHideColors; ///< slot used to hide colors
    /// slot used to update color mode (0: none, 1: point, 2: cell)
    UpdateColorModeSlotType::sptr m_slotUpdateColorMode;
    /**
     * @}
     */

public:

    //------------------------------------------------------------------------------

    vtkActor* getActor() const
    {
        return m_actor;
    }

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MESH_HPP__
