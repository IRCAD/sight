/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_SLICESCURSOR_HPP_
#define _VISUVTKADAPTOR_SLICECURSOR_HPP_

#include <fwData/Image.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"


class VISUVTKADAPTOR_CLASS_API vtkPolyData;
class VISUVTKADAPTOR_CLASS_API vtkPolyDataMapper;
class VISUVTKADAPTOR_CLASS_API vtkActor;


namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API SlicesCursor: public ::fwComEd::helper::MedicalImageAdaptor,public ::fwRenderVTK::IVtkAdaptorService 
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SlicesCursor)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API SlicesCursor()  throw();

    VISUVTKADAPTOR_API virtual ~SlicesCursor()  throw();

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void reconfiguring() throw(fwTools::Failed);

    /// set the scale for the cross : 1. means full cross, 0.5 half cross, 0. no cross
    void setCrossScale(double scale);

    void updateSliceIndex( ::fwData::Image::sptr image );
    void buildPolyData();
    void updateColors();
    void buildColorAttribute();

    vtkPolyData         *m_cursorPolyData;
    vtkPolyDataMapper   *m_cursorMapper;
    vtkActor            *m_cursorActor;
    float m_scale;
    bool  m_isSelected;

private :


    /// Compute the barycenter : result = scale*ptA + (1-scale)*ptB
    static void barycenter( double ptA[3], double ptB[3], float scale, double result[3] );

    /// Compute points A' and B' on A------A'<---P--->B'---B
    static void computeCrossPoints( double _ptA[3], double _ptB[3], double _ptP[3], double _scale, double _ptAprime[3], double _ptBprime[3] );

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_SLICESCURSOR_HPP_
