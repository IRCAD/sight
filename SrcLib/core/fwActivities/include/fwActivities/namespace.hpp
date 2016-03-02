/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_NAMESPACE_HPP__
#define __FWACTIVITIES_NAMESPACE_HPP__

/**
 * @brief Namespace containing activities data and builder
 */
namespace fwActivities
{

/// The namespace fwActivities::builder contains the base interface for data ActivitySeries builder.
namespace builder
{

/// Contains fwActivities::builder::registry details
namespace registry
{
} // namespace registry

/// Contains fwActivities::builder::factory utilities
namespace factory
{
} // namespace factory

} // end namespace builder

namespace validator
{

/// Contains fwActivities::validator::registry details
namespace registry
{
} // namespace registry


/// Contains fwActivities::validator::factory utilities
namespace factory
{
} // namespace factory

} // namespace validator

/**
 * @page Activity Activity configuration
 *
 * An activity is defined by the extension "::fwActivities::registry::Activities". It is used to launch an
 * AppConfig with the selected data, it will create a new data ::fwMedData::ActivitySeries that inherits from a
 * ::fwMedData::Series.
 *
 * The service ::activities::action::SActivityLauncher allows to launch an activity. Its role is to create the specific
 * Activity associated with the selected data.
 *
 * This action should be followed by the service ::guiQt::editor::DynamicView : this service listens the action signals
 * and launchs the activity in a new tab.
 *
 * - ::activities::action::SActivityLauncher uses the selected data to generate the activity.
 * - ::guiQt::editor::DynamicView  displays the activity in the application.
 * - ::fwData::Vector contains the set of selected data .
 *
 * @code{.xml}
     <extension implements="::fwActivities::registry::Activities">
         <id>myActivityId</id>
         <title>3D Visu</title>
         <desc>Activity description ...</desc>
         <icon>Bundles/media_0-1/icons/icon-3D.png</icon>
         <requirements>
             <requirement name="param1" type="::fwData::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
             <requirement name="param2" type="::fwData::Mesh" maxOccurs="3" container="composite">
                 <key>Item1</key>
                 <key>Item2</key>
                 <key>Item3</key>
             </requirement>
             <requirement name="imageSeries" type="::fwMedData::ImageSeries" minOccurs="0" maxOccurs="2" />
             <requirement name="modelSeries" type="::fwMedData::ModelSeries" minOccurs="1" maxOccurs="1" />
             <!-- ...-->
         </requirements>
         <builder>::fwActivities::builder::ActivitySeries</builder>
         <validator>::fwActivities::validator::ImageProperties</validator><!-- pour fw4spl_0.9.2 -->
         <appConfig id="myAppConfigId">
            <parameters>
                <parameter replace="registeredImageUid" by="@values.param1" />
                <parameter replace="orientation" by="frontal" />
                <!-- ...-->
            </parameters>
         </appConfig>
     </extension>
   @endcode
 *
 * The activity parameters are (in this order):
 *
 * - \b id: it is the activity unique identifier.
 * - \b title: it is the activity title that will be displayed on the tab.
 * - \b desc: it is the description of the activity. It is displayed by the SActivityLauncher when several activity can
 *   be launched with the selected data.
 * - \b icon: it is the path to the activity icon. It is displayed by the SActivityLauncher when several activity can be
 *   launched with the selected data.
 * - \b requirements: it is the list of the data required to launch the activity. This data must be selected in the
 *   vector (``::fwData::Vector``).
 *     - \b requirement: a required data.
 *         - \b name: key used to add the data in the activity Composite.
 *         - \b type: the data type (ex: ``::fwMedData::ImageSeries``).
 *         - \b minOccurs (optional, "1" by default): the minimum number of occurrences of this type of object in the
 *           vector.
 *         - \b maxOccurs (optional, "1" by default): the maximum number of occurrences of this type of object in the
 *           vector.
 *         - \b container (optional, "vector" or "composite", default: composite): container used to contain the data if
 *           minOccurs or maxOccurs are not "1". if the container is "composite", you need to specify the "key" of each
 *           object in the composite.
 * - \b builder: implementation of the activity builder. The default builder is
 *   ``::fwActivities::builder::ActivitySeries`` : it creates the ``::fwMedData::ActivitySeries`` and adds the required
 *   data in its composite with de defined key.
 *   The builder ``::fwActivities::builder::ActivitySeriesInitData`` allows, in addition to what the default builder
 *   does, to create data when minOccurs == 0 et maxOccurs == 0.
 * - \b validators (optional):  it defines the list of validator. If you need only one validator, you don't need the
 *   "validators" tag (only "validator").
 * - \b validator (optional): it allows to validate if the selected required object are correct for the activity.
 *   For example, the validator ``::fwActivities::validator::ImageProperties`` checks that all the selected images
 *   have the same size, spacing and origin.
 * - \b appConfig: it defines the AppConfig to launch and its parameters
 * - \b id: identifier of the AppConfig
 * - \b parameters: list of the parameters required by the AppConfig
 *    - \b parameter: defines a parameter
 *          - \b replace: name of the parameter as defined in the AppConfig
 *          - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *               frontal) or define a sesh@ path (ex. @values.myImage). The root object of the sesh@ path if the
 *               composite contained in the ActivitySeries.
 */

} // namespace fwActivities

#endif /* __FWACTIVITIES_NAMESPACE_HPP__ */

