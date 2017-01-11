/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGELANDMARKS_HPP__
#define __VISUVTKADAPTOR_IMAGELANDMARKS_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkCommand;

namespace visuVTKAdaptor
{


/**
 * @brief Adaptor for landmarks in the field of an image.
 *
 * Registers a ::fwData::PointList in to the generic scene for the landmarks in the given Image
 *
 * @section Slots Slots
 * - \b updateLandmaks() : Updates the rendering on the scene
 * - \b updateLandmaksField() : Tests if the added field is a landmark image type and updates the rendering accordingly.
 * @section XML XML Configuration
 *
 * @code{.xml}
        <adaptor id="..." class="::visuVTKAdaptor::ImageLandmarks" objectId="imageKey">
            <config renderer="default" picker="default" />
        </adaptor>
   @endcode
 *
 */
class VISUVTKADAPTOR_CLASS_API ImageLandmarks : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageLandmarks)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API ImageLandmarks() throw();

    VISUVTKADAPTOR_API virtual ~ImageLandmarks() throw();

    VISUVTKADAPTOR_API virtual void show(bool b = true);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_LANDMARK_ADDED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     * Connect Image::s_LANDMARK_REMOVED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     * Connect Image::s_LANDMARK_DISPLAYED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    std::list< ::fwRenderVTK::IVtkAdaptorService::sptr > m_subServices;

    vtkCommand* m_rightButtonCommand;

    bool m_needSubservicesDeletion;

private:

    /// Slot: update landmarks sub-adaptors
    void updateLandmaks();

    /// Slot: tests if the added field is a landmark, if it is, updates the vrkPoints.
    void updateLandmaksField(::fwData::Object::FieldsContainerType fieldsContainer);

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGELANDMARKS_HPP__
