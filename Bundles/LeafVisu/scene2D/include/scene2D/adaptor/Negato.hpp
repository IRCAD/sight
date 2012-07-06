/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_NEGATO_HPP_
#define _SCENE2D_ADAPTOR_NEGATO_HPP_

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "scene2D/adaptor/IAdaptor.hpp"
#include "scene2D/data/Coord.hpp"

#include <QImage>
#include <QGraphicsItemGroup>
#include <QPointF>

namespace scene2D
{
namespace adaptor
{

class SCENE2D_CLASS_API Negato : public ::fwComEd::helper::MedicalImageAdaptor, public ::scene2D::adaptor::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Negato)(::scene2D::adaptor::IAdaptor) ) ;

    SCENE2D_API Negato() throw();
    SCENE2D_API virtual ~Negato() throw();

protected:

    SCENE2D_API void configuring() throw ( ::fwTools::Failed );
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );
    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

private:

    QImage * createQImage();
    void updateFromImage( QImage * qimg );
    void changeImageMinMaxFromCoord( scene2D::data::Coord & oldCoord, scene2D::data::Coord & newCoord );

    QRgb getQImageVal(const unsigned int index, signed short* buffer,
            const double wlMin, const double wlMax, const double window,
            const double tfMin, const double tfMax,
            ::fwData::TransferFunction::sptr tf);

    QImage * m_qimg;
    QGraphicsPixmapItem * m_pixmapItem;
    QGraphicsItemGroup* m_layer;

    /**
     * @brief Reference position when a drag and drop move is performed
     */
    QPointF m_pos;

    /**
     * @brief Zoom ratio applied at each mouse scroll
     */
    float m_scaleRatio;

    /**
     * @brief Indicate if the negato is being moved when pressing mouse's middle button
     */
    bool m_negatoIsBeingMoved;

    /// The current orientation of the negato
    ::fwComEd::helper::MedicalImageAdaptor::Orientation m_orientation;

    /// Scale ratios (x, y)
    std::pair<double, double> m_scale;

    /// Tell if the negato is being moved
    bool m_pointIsCaptured;

    /// Ref. position when changing image min/max
    scene2D::data::Coord m_oldCoord;

    /// Specify if the negato allow slice type events
    bool m_changeSliceTypeAllowed;
};



} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_NEGATO_HPP_

