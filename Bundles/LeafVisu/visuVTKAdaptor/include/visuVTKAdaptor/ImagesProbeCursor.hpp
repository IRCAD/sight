/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESPROBECURSOR_HPP__
#define __VISUVTKADAPTOR_IMAGESPROBECURSOR_HPP__

#include "visuVTKAdaptor/config.hpp"

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
 * @brief   Draw pixel information from images given by id in composite.
 */
class VISUVTKADAPTOR_CLASS_API ImagesProbeCursor : public  ::fwDataTools::helper::MedicalImageAdaptor,
                                                   public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (ImagesProbeCursor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API ImagesProbeCursor() noexcept;

    VISUVTKADAPTOR_API virtual ~ImagesProbeCursor() noexcept;

    VISUVTKADAPTOR_API void updateView( double world[3] );

    VISUVTKADAPTOR_API void setVisibility( bool visibility );

    VISUVTKADAPTOR_API void StartImagesProbeCursor();

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    /**
     * @code{.xml}
       <adaptor id="GENERIC_UID_ImagesProbeCursor" class="::visuVTKAdaptor::ImagesProbeCursor" objectId="self">
         <config renderer="default" picker="myPicker">
             <image objectId="myRefImage" name="background" />
             <image objectId="myObject" name="working" />
         </config>
       </adaptor>
       @endcode
     */
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    void buildTextActor();
    void buildPolyData();

    void computeCrossExtremity( const int probeSlice[3], double worldCross[4][3] );

    typedef std::pair< std::string, std::string > ImagesIdPair;
    std::vector< ImagesIdPair > m_imagesId;
    double m_priority;

    vtkCommand* m_vtkObserver;

    vtkActor2D* m_textActor;     // rendering defect using a vtkTextActor
    vtkTextMapper* m_textMapper;

    vtkPolyData* m_cursorPolyData;
    vtkPolyDataMapper* m_cursorMapper;
    vtkActor* m_cursorActor;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGESPROBECURSOR_HPP__
