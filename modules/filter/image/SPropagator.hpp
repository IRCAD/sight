/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <core/base.hpp>

#include <data/tools/PickingInfo.hpp>

#include <filter/image/ImageDiff.hpp>
#include <filter/image/LineDrawer.hpp>
#include <filter/image/MinMaxPropagation.hpp>

#include <service/IFilter.hpp>

#include <ui/base/IHasParameters.hpp>
#include <ui/history/ICommand.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service placing seeds and propagating from them in an image.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after propagation when the mouse has been released.
 * - \b drawn(ui::history::ICommand::sptr) : Signal emitted after propagation, sends an ImageDiffCommand.
 *
 * @section Slots Slots
 * - \b setIntParameter(int, std::string) : set the value to write in the image, the key passed must be "value".
 * - \b setBoolParameter(bool, std::string) : set the overwrite mode, the key passed must be "overwrite".
 * - \b setDoubleParameter(double, std::string) : set the radius, the passed key must be "radius".
 * - \b setEnumParameter(std::string, std::string) : set the propagation mode, the passed key must be "mode".
 * - \b draw(data::tools::PickingInfo) : draw a line between the last and current picked voxels.
 * The coordinates must be in image space (by using an ::visuVTKAdaptor::ImagePickerInteractor for example).
 * - \b resetDrawing() : reset drawing, this can be called when the current slice is changed in the image.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::SPropagator">
       <in key="imageIn" uid="..." autoConnect="true" />
       <inout key="imageOut" uid="..." />
       <value>1</value>
       <radius>50</radius>
       <overwrite>true</overwrite>
       <mode>min</mode>
       <orientation>axial</orientation>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b imageIn [sight::data::Image]: The background image, whose values we read from during propagation.
 *
 * @subsection In-Out In-Out
 * - \b imageOut [sight::data::Image]: The output image, in which we will draw.
 *
 * @subsection Configuration Configuration
 * - \b value (optional) : The initial value used for drawing. 1 by default.
 * - \b radius(optional) : The maximum propagation distance. Infinity by default.
 * - \b overwrite (optional) : The overwrite mode. true by default.
 * - \b mode (optional) : Propagation mode. Possible values are 'min', 'max' and 'minmax'. 'min' by default.
 * - \b orientation (optional) : The initial slice orientation. 'axial' by default.
 */
class MODULE_FILTER_IMAGE_CLASS_API SPropagator : public service::IFilter,
                                                  public ui::base::IHasParameters
{
public:

    SIGHT_DECLARE_SERVICE(SPropagator, ::sight::service::IFilter);

    /// Initializes slots signals and member variables.
    MODULE_FILTER_IMAGE_API SPropagator();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_API ~SPropagator() override;

protected:

    ///Configures the services' parameters.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Initializes line drawer and propagator.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Destroys line drawer and propagator.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Recreates line drawer and propagator when the image has been modified.
    MODULE_FILTER_IMAGE_API void updating() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_IMAGE_IN to s_UPDATE_SLOT
     * Connect data::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_IN to s_SET_ORIENTATION_SLOT
     * Connect data::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_IN to s_RESET_DRAWING
     */
    MODULE_FILTER_IMAGE_API KeyConnectionsMap getAutoConnections() const override;

    /// Sets overwrite mode. Key must be 'overwrite'.
    MODULE_FILTER_IMAGE_API void setBoolParameter(bool val, std::string key) override;

    /// Sets value. Key must be 'value'.
    MODULE_FILTER_IMAGE_API void setIntParameter(int val, std::string key) override;

    /// Sets radius. Key must be 'radius'.
    MODULE_FILTER_IMAGE_API void setDoubleParameter(double val, std::string key) override;

    /// Sets propagation mode. Key must be 'mode'.
    MODULE_FILTER_IMAGE_API void setEnumParameter(std::string val, std::string key) override;

private:

    typedef sight::filter::image::MinMaxPropagation::CoordinatesType CoordinatesType;

    typedef sight::filter::image::MinMaxPropagation::OrientationType OrientationType;

    typedef core::com::Signal<void (ui::history::ICommand::sptr)> DrawnSignalType;

    /// Swaps orientation.
    void setOrientation(int from, int to);

    /// Sets drawing to false.
    void resetDrawing();

    /// Draws seeds while the left button is pressed, propagate on release.
    void draw(data::tools::PickingInfo pickingInfo);

    /// Appends diff to member diff.
    bool appendDiff(const sight::filter::image::ImageDiff& diff);

    /// Gets seeds from the diff list.
    sight::filter::image::MinMaxPropagation::SeedsType convertDiffToSeeds() const;

    /// Line drawer.
    UPTR(sight::filter::image::LineDrawer) m_lineDrawer;

    /// Propagator.
    UPTR(sight::filter::image::MinMaxPropagation) m_propagator;

    /// Memorizes the last cursor position.
    CoordinatesType m_oldPoint;

    /// Current slice orientation.
    OrientationType m_orientation;

    /// Memorizes the seeds while drawing.
    sight::filter::image::ImageDiff m_diff;

    /// Signal sent when the pencil is released and the image has been modified.
    DrawnSignalType::sptr m_sigDrawn;

    /// Value to be written inside the image.
    int m_value;

    /// Maximum propagation distance.
    double m_radius;

    /// Propagation mode (min, max or minmax).
    sight::filter::image::MinMaxPropagation::Mode m_mode;

    /// Overwrite mode.
    bool m_overwrite;

    /// Set to 'true' if we currently drawing in the image.
    bool m_drawing;
};

} // namespace sight::module::filter::image.
