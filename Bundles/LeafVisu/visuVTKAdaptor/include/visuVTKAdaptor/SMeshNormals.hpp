/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SMESHNORMALS_HPP__
#define __VISUVTKADAPTOR_SMESHNORMALS_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkActor;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API MeshNormals : public ::fwRenderVTK::IAdaptor
{

public:

    typedef enum
    {
        NONE,
        POINT_NORMAL,
        CELL_NORMAL
    } NormalRepresentation;

    fwCoreServiceClassDefinitionsMacro( (MeshNormals)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API MeshNormals() noexcept;

    VISUVTKADAPTOR_API virtual ~MeshNormals() noexcept;

    VISUVTKADAPTOR_API void setPolyData(vtkSmartPointer< vtkPolyData > polydata);

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VERTEX_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateVertexSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdatePointNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateCellNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_POINT_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> ShowPointNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_CELL_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> ShowCellNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_HIDE_NORMALS_SLOT;
    typedef ::fwCom::Slot<void ()> HideNormalsSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_NORMAL_MODE_SLOT;
    typedef ::fwCom::Slot<void (std::uint8_t)> UpdateNormalModeSlotType; ///< normal mode (0: none, 1: point, 2: cell)

    /// Slot: used to update normal mode (0: none, 1: point, 2: cell)
    void updateNormalMode(std::uint8_t mode);
    /** @} */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_VERTEX_MODIFIED_SIG to this::s_UPDATE_VERTEX_SLOT
     * Connect mesh::s_POINT_NORMALS_MODIFIED_SIG to this::s_UPDATE_POINT_NORMALS_SLOT
     * Connect mesh::s_CELL_NORMALS_MODIFIED_SIG to this::s_UPDATE_CELL_NORMALS_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doStop();

    vtkActor* getActor() const;
    void updateMeshNormals();

    /**
     * @name Slots methods
     * @{
     */

    /// Slot: used to update mesh vertex
    void updateVertex();

    ///Slot: used to update point normals
    void updatePointNormals();

    /// Slot: used to update cell normals
    void updateCellNormals();

    /// Slot: used to show point normals
    void showPointNormals();

    /// Slot: used to show cell normals
    void showCellNormals();

    /// Slot: used to hide normals
    void hideNormals();

    /**
     * @}
     */

private:

    NormalRepresentation m_normalRepresentation;
    vtkSmartPointer< vtkPolyData > m_polyData;
    vtkSmartPointer< vtkActor >    m_actor;
    static std::map< std::string, NormalRepresentation > m_normalRepresentationConversion;

    /**
     * @name Slots attributes
     * @{
     */
    UpdateVertexSlotType::sptr m_slotUpdateVertex; ///< slot used to update mesh vertex
    UpdatePointNormalsSlotType::sptr m_slotUpdatePointNormals; ///< slot used to update point normals
    UpdateCellNormalsSlotType::sptr m_slotUpdateCellNormals; ///< slot used to update cell normals
    ShowPointNormalsSlotType::sptr m_slotShowPointNormals; ///< slot used to show point normals
    ShowCellNormalsSlotType::sptr m_slotShowCellNormals; ///< slot used to show cell normals
    HideNormalsSlotType::sptr m_slotHideNormals; ///< slot used to hide normals
    /// slot used to update normal mode (0: none, 1: point, 2: cell)
    UpdateNormalModeSlotType::sptr m_slotUpdateNormalMode;
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SMESHNORMALS_HPP__
