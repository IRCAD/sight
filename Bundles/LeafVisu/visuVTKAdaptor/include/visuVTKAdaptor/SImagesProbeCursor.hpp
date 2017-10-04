/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGESPROBECURSOR_HPP__
#define __VISUVTKADAPTOR_SIMAGESPROBECURSOR_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SProbeCursor.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;
class vtkActor;
class vtkActor2D;
class vtkTextMapper;
class vtkPolyData;
class vtkPolyDataMapper;

namespace visuVTKAdaptor
{

/**
 * @brief   Displays pixel information from images.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::visuVTKAdaptor::SNegatoMPR" autoConnect="yes">
       <inout group="image">
            <key uid="..." name="reference" />
            <key uid="..." name="drawing" />
       </inout>
       <config renderer="default" picker="myPicker" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: group containing the images to displays the information.
 *   - \b name: name of the image displayed before its information
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the information.
 *    - \b picker (mandatory): identifier of the picker
 */
class VISUVTKADAPTOR_CLASS_API SImagesProbeCursor : public ::visuVTKAdaptor::SProbeCursor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SImagesProbeCursor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImagesProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~SImagesProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual void updateView( double world[3] ) override;

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

    std::vector< std::string > m_imagesNames;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SIMAGESPROBECURSOR_HPP__
