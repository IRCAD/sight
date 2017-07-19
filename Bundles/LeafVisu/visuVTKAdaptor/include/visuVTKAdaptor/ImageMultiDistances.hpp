/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP__
#define __VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP__


#include "visuVTKAdaptor/config.hpp"

#include <fwData/PointList.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkCommand;

namespace visuVTKAdaptor
{



/**
 * @brief Adaptor to display distance on an image
 */
class VISUVTKADAPTOR_CLASS_API ImageMultiDistances : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageMultiDistances)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API ImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API ~ImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API void setNeedSubservicesDeletion( bool _needSubservicesDeletion );

    VISUVTKADAPTOR_API virtual void show(bool showDistances = true);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_DISTANCE_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_DISTANCE_REMOVED_SIG to this::s_REMOVE_DISTANCE_SLOT
     * Connect Image::s_DISTANCE_DISPLAYED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doStop();

    void installSubServices( ::fwData::PointList::sptr pl );
    ::fwData::Point::sptr screenToWorld(int X,int Y);

    void createNewDistance( std::string sceneId );

    std::list< ::fwRenderVTK::IVtkAdaptorService::sptr > m_subServices;

    vtkCommand * m_rightButtonCommand;

    bool m_needSubservicesDeletion;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: remove distance
    void removeDistance(::fwData::PointList::sptr pointList);

    /// Slot: to create a new distance attached to this adaptor
    void createDistance();
    /**
     * @}
     */
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP__
