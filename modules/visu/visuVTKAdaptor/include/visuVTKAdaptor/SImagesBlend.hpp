/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkImageActor;
class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageData;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkImageCheckerboard;
class vtkThreadedImageAlgorithm;

namespace visuVTKAdaptor
{

/**
 * @brief Manage blend for image given in configuration.
 *
 * @section Slots Slots
 * - \b changeMode(std::string _value, std::string _key): set the blending algorithm. The key must be "ImageSource"
 * - \b changeCheckerboardDivision(int division): set the number of cells to display the checkerboard
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::visuVTKAdaptor::SImagesBlend">
        <inout group="image" autoConnect="yes">
            <key uid="..." tfalpha="no" />
            <key uid="..." tfalpha="yes" opacity="0.5" />
        </inout>
        <inout group="tf" optional="yes">
            <key uid="..." />
            <key uid="..." />
        </inout>
        <config renderer="default" vtkimageregister="imageBlend" checkerboardDivision="10" />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: group containing the images to displays the information. (at least two images are
 *      required)
 *    - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato
 *    - \b opacity (optional, default=1.0): opacity (0.0..1.0)
 * - \b tf [::fwData::TransferFunction](optional) : group containing the TransferFunction used to display the images, if
 *      it is not set, the default image's transfer function is used (CT-GreyLevel)
 * @note There must be as much elements in the two groups
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the images.
 *    - \b picker (mandatory): identifier of the picker
 *    - \b vtkimageregister (mandatory): VTK object name for default blending algorithm.
 *    - \b checkerboardDivision (optional): Number of division in checkerboard mode.
 *    - \b setTopImageTransparency(int): sets the transparency (inside the [0, 255] range) of the image blended on top.
 */
class VISUVTKADAPTOR_CLASS_API SImagesBlend : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SImagesBlend, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SImagesBlend() noexcept;

    VISUVTKADAPTOR_API virtual ~SImagesBlend() noexcept;

    //------------------------------------------------------------------------------

    void setVtkImageRegisterId(const std::string& id)
    {
        m_imageRegisterId = id;
    }

protected:

    class ImageInfo
    {

    public:
        ImageInfo() :
            m_useTFAlfa(false),
            m_imageOpacity(1.0)
        {
        }

        bool m_useTFAlfa;
        double m_imageOpacity;
    };

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;
    VISUVTKADAPTOR_API void swapping(const KeyType& key) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    void addImageAdaptors();
    void removeImageAdaptors();

    /// Return true if images to blend have the same spacing, origin and size. Else show a message dialog and return
    // false
    bool checkImageInformations();

private:

    ///SLOT: changeMode
    void changeMode(std::string _value, std::string _key);

    ///SLOT: Change number of celle in the checkerboard
    void changeCheckerboardDivision(int division);

    /// Create a new image adaptor
    void addImage(::fwData::Image::sptr img, ::fwData::TransferFunction::sptr tf, const ImageInfo& info);

    ///SLOT: sets the top image's transparency. Transparency value must be in [0, 255].
    void setTopImageTransparency(int transparency);

    typedef std::vector< ImageInfo > ImageInfoMap;
    /// Map containing the image displaying information
    ImageInfoMap m_imagesInfo;

    /// Holds the vtkImageblend or vtkimageCheckerboard algorithms
    vtkThreadedImageAlgorithm* m_imageAlgorithm;

    std::string m_imageRegisterId;

    /// Number of division in checkerboard mode
    int m_checkerboardDivision;

    /// Number of Z divisions for checkerboard (=1 if it is a 2D image)
    int m_zDivision;
};

} //namespace visuVTKAdaptor
