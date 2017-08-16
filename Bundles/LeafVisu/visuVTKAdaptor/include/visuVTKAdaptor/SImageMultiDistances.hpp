/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGEMULTIDISTANCES_HPP__
#define __VISUVTKADAPTOR_SIMAGEMULTIDISTANCES_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/PointList.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display distance on an image
 *
 * @section Slots Slots
 * - \b createDistance() : creates a new distance attached to this adaptor
 * - \b removeDistance(::fwData::PointList::sptr) : remove the distance
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageMultiDistances" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" picker="negatodefault" filter="false" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the distances
 *    - \b picker (mandatory): identifier of the picker
 *    - \b filter (optional, default: false): if true, this adaptor only displays the distance associated to the scene
 *         (it uses a field in the image containing the uid of the scene)
 */
class VISUVTKADAPTOR_CLASS_API SImageMultiDistances : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageMultiDistances)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API ~SImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API void setNeedSubservicesDeletion( bool _needSubservicesDeletion );

    VISUVTKADAPTOR_API virtual void show(bool showDistances = true);

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_DISTANCE_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_DISTANCE_REMOVED_SIG to this::s_REMOVE_DISTANCE_SLOT
     * Connect Image::s_DISTANCE_DISPLAYED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    void installSubServices( ::fwData::PointList::sptr pl );
    ::fwData::Point::sptr screenToWorld(int X, int Y);

    void createNewDistance( std::string sceneId );

    std::list< ::fwRenderVTK::IAdaptor::sptr > m_subServices;

    vtkCommand* m_rightButtonCommand;

    bool m_needSubservicesDeletion;

    bool m_filter;

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

#endif // __VISUVTKADAPTOR_SIMAGEMULTIDISTANCES_HPP__
