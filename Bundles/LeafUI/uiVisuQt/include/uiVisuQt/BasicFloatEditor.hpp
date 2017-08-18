/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_BASICFLOATEDITOR_HPP__
#define __UIVISUQT_BASICFLOATEDITOR_HPP__

#include "uiVisuQt/config.hpp"

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

namespace uiVisu
{

/**
 * @brief   BasicFloatEditor service permits to edit a float value.
 * * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiVisu::BasicFloatEditor">
       <inout key="float" uid="..." />
   </service>
   @endcode
 * @subsection InOut InOut
 * - \b float [::fwData::Float]: float.
 */

class UIVISUQT_CLASS_API BasicFloatEditor : public QObject,
                                            public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (BasicFloatEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API BasicFloatEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~BasicFloatEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout.
    virtual void starting();

    /// Destroy the layout.
    virtual void stopping();

    /// Do nothing
    virtual void updating();

    /// Do nothing
    virtual void swapping();

    /// Configure the editor. Do nothing.
    virtual void configuring();

    /// Overrides
    virtual void info( std::ostream& _sstream );

protected Q_SLOTS:

    /**
     * @brief This method is called when the value change.
     *
     * Check if the value is a valid float, update the Float object value and notify the modification.
     */
    void onModifyValue(QString value);

private:

    QPointer< QLineEdit > m_valueCtrl;

};

} // uiVisu

#endif // __UIVISUQT_BASICFLOATEDITOR_HPP__

