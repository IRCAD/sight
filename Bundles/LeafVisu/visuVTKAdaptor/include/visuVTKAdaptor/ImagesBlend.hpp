/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESBLEND_HPP__
#define __VISUVTKADAPTOR_IMAGESBLEND_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

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
 */
class VISUVTKADAPTOR_CLASS_API ImagesBlend : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro( (ImagesBlend)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_CLASS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MODE_SLOT;

    VISUVTKADAPTOR_API ImagesBlend() throw();

    VISUVTKADAPTOR_API virtual ~ImagesBlend() throw();

    //------------------------------------------------------------------------------

    void setVtkImageRegisterId(const std::string& id)
    {
        m_imageRegisterId = id;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

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
        std::string m_tfSelectionFwID;
        std::string m_selectedTFKey;

        ::fwCom::helper::SigSlotConnection m_connections;
    };

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    void addImageAdaptors();
    void removeImageAdaptors();

    /// Return true if images to blend have the same spacing, origin and size. Else show a message dialog and return
    // false
    bool checkImageInformations();

private:

    ///SLOT: changeMode
    void changeMode(std::string _value, std::string _key);

    /// Create a new image adaptor
    void addImage(::fwData::Image::csptr img, SPTR(ImageInfo) info);

    std::vector<std::string> m_imageIds;
    typedef std::map< std::string, SPTR(ImageInfo) > ImageInfoMap;
    ImageInfoMap m_imagesInfo;

    /// map <fwId, adaptor>
    typedef std::map< std::string, ::fwRenderVTK::IVtkAdaptorService::sptr > RegisteredImageMap;
    RegisteredImageMap m_registeredImages;

    /// Holds the vtkImageblend or vtkimageCheckerboard algorithms
    vtkThreadedImageAlgorithm* m_imageAlgorithm;

    std::string m_imageRegisterId;

    /// Number of division in checkerboard mode
    int m_checkerboardDivision;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGESBLEND_HPP__
