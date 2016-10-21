/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PLANELIST_HPP__
#define __VISUVTKADAPTOR_PLANELIST_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/MeshFactory.hpp"
#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkCommand;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API PlaneList : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PlaneList)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API PlaneList() throw();

    VISUVTKADAPTOR_API virtual ~PlaneList() throw();

    void setPlaneCollectionId(::fwRenderVTK::SRender::VtkObjectIdType id)
    {
        m_planeCollectionId = id;
    }

    ::fwRenderVTK::SRender::VtkObjectIdType getPlaneCollectionId() const
    {
        return m_planeCollectionId;
    }

protected:

    /// Type of signal emitted when plane selection changed
    typedef  ::fwCom::Signal< void (::fwData::Plane::sptr) > SelectedignalType;

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Slot: Update plane selection
    void updateSelection(::fwData::Plane::sptr plane);

    vtkCommand* m_rightButtonCommand;
    ::fwRenderVTK::SRender::VtkObjectIdType m_planeCollectionId;

    /// Store connections to planes adaptors.
    ::fwCom::helper::SigSlotConnection m_planeConnections;

private:
    /**
     * @name Slots
     * @{
     */
    /// Update planes (call doStop-soStart())
    void updatePlanes();

    /// Show/hide planes
    void showPlanes(bool visible);
    /**
     * @}
     */
};




} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_PLANELIST_HPP__
