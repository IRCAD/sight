/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_STEXT_HPP__
#define __VISUVTKARADAPTOR_STEXT_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <algorithm>
#include <string>

class vtkTextActor;
class vtkActor2D;
class vtkTextMapper;

namespace visuVTKARAdaptor
{

//-----------------------------------------------------------------------------
typedef enum
{
    ARIAL,
    COURIER,
    TIMES
}FontFamily;

typedef enum
{
    LEFT,
    RIGHT,
    HCENTERED
}HAlign;

typedef enum
{
    TOP,
    BOTTOM,
    VCENTERED
}VAlign;

struct TextProperty
{
    TextProperty() :
        m_fontFamily(ARIAL),
        m_fontSize(20),
        m_hAlign(LEFT),
        m_vAlign(BOTTOM),
        m_bold(false),
        m_italic(false),
        m_shadow(false),
        m_opacity(1.0)
    {
        std::fill(m_color, m_color+3, 0.);
    }

    FontFamily m_fontFamily; ///< Font family (in points)
    size_t m_fontSize;   ///< Font size (in points)
    HAlign m_hAlign; ///< Horizontal alignment
    VAlign m_vAlign; ///< Vertical alignment

    bool m_bold;
    bool m_italic;
    bool m_shadow;
    double m_color[3];
    double m_opacity;
};
//-----------------------------------------------------------------------------

/**
 * @brief   Adaptor to show a text, representing the associated object, in the scene.
 *          The service listens to the event OBJECT_MODIFIED for updating the displayed text.
 */
class VISUVTKARADAPTOR_CLASS_API SText : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SText)(::fwRenderVTK::IAdaptor) );

    /// Constructor
    VISUVTKARADAPTOR_API SText();

    /// Destructor
    VISUVTKARADAPTOR_API virtual ~SText() noexcept;

protected:

    /// starts the adaptor
    virtual void doStart();

    /// stops the adaptor
    virtual void doStop();

    /// Calls doStop() and doStart()
    virtual void doSwap();

    /// updates the shown text and its properties.
    virtual void doUpdate();

    /**
     * @brief Configure the adaptor
     *
     * Example :
       @code{.xml}
       <adaptor id="text" class="::visuVTKARAdaptor::SText" objectId="text">
        <config renderer="default" seshat="@patient.name" >
            <style>
                <fontSize>15</fontSize>
                <fontFamily>ARIAL</fontFamily>
                <hAlign>right</hAlign>
                <vAlign>top</vAlign>
                <color>#FF0000</color>
                <italic>false</italic>
                <bold>false</bold>
                <opacity>1.0</opacity>
                <shadow>true</shadow>
            </style>
        </config>
       </adaptor>
       @endcode
     * - seshat : seshat path, optional, if present it's used to extract text value in associated data.
     * - style : described the look and formatting of the displayed text on the scene. Available options are :
     *     - fontSize : font size in points of displayed text (not mandatory, default value is set to 20)
     *     - fontFamily : defined the font family
     *       ('ARIAL', 'COURIER' or 'TIMES', not mandatory, default value is set to ARIAL)
     *     - hAlign : horizontal alignment
     *       ('left', 'right' or 'centered', not mandatory, default value is set to 'left')
     *     - vAlign : vertical alignment
     *       ('top', 'bottom' or 'centered', not mandatory, default value is set to 'bottom')
     *     - color : defined text color in hexa (not mandatory, default value is is set to black)
     *     - italic : enable/disable text italic (not mandatory, default value is set to false, '#000000')
     *     - bold : enable/disable text bolding (not mandatory, default value is set to false)
     *     - opacity : text opacity. 1.0 is totally opaque and 0.0 is completely transparent
     *       (not mandatory, default value is set to 1.0)
     *     - shadow : enable/disable text shadow (not mandatory, default value is set to false)
     */
    VISUVTKARADAPTOR_API virtual void doConfigure();

private:

    vtkActor2D* m_actor; ///< the vtk actor for showing the 2D text.
    vtkTextMapper* m_mapper;  ///< provides 2D text annotation support for VTK

    TextProperty m_textProperty; ///< text properties (bold, italic, font size...)

    std::string m_seshatPath; ///< seshat path to retrieve a "sub-object" in the associated object. (ex: @patient.name)

    void updateText(); ///< updates the shown text.
    void updateStyle(); ///< updates the text properties.
};

} //namespace visuVTKARAdaptor

#endif //__VISUVTKARADAPTOR_STEXT_HPP__

