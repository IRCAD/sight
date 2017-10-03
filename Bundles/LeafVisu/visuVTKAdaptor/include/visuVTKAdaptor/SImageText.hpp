/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGETEXT_HPP__
#define __VISUVTKADAPTOR_SIMAGETEXT_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SText.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <string>

class vtkTextActor;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display window image information (width, heigth, picked position)
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int axial, int frontal, int sagittal) : update image slice index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageText">
        <inout key="image" uid="..." />
        <inout key="tfSelection" uid="..." />
        <config renderer="default" text="@patient.name" color="#ff0000" fontSize="16" />
   </service>
   @endcode
 * or
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageText">
       <inout key="image" uid="..." />
       <inout key="tfSelection" uid="..." />
       <config renderer="default" vAlign='top' hAlign='center'>
       <text>SText to display<text>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tfSelection [::fwData::Composite] (optional): composite containing the TransferFunction.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b text : text to display. If a valid camp object attribute path is detected, the associated
 *      value is used. Otherwise, it uses the given value in XML configuration.
 *    - \b color : text color in hexadecimal format : \#rrggbb or \#rrggbbaa (optional, default: white)
 *    - \b fontSize : font size in points of displayed text (optional, default: 20)
 *    - \b hAlign : horizontal alignment (left, center or right, optional, default: 'left')
 *    - \b vAlign : vertical alignment (top, center or bottom, optional, default: 'bottom')
 *
 * - \b text The configuration accepts also a text tag instead of the text attribute
 *   (see second example). This is useful for multiline text. The same rules
 *   that for the attribute are applied.
 */
class VISUVTKADAPTOR_CLASS_API SImageText : public SText,
                                            public ::fwDataTools::helper::MedicalImageAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageText)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImageText() noexcept;

    VISUVTKADAPTOR_API virtual ~SImageText() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Update tranfer function points
    VISUVTKADAPTOR_API virtual void updatingTFPoints() override;

    /// Update transfer function windowing
    VISUVTKADAPTOR_API virtual void updatingTFWindowing(double window, double level) override;

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor

#endif //__VISUVTKADAPTOR_SIMAGETEXT_HPP__
