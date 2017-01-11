/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_NAMESPACE_HPP__
#define __FWSERVICES_NAMESPACE_HPP__

/**
 * @brief Namespace fwServices is dedicated to (mimic) the dynamic affectation of methods to (pure data) objects
 * (::fwTools::Object), and component orientation transparently. This is assimilable to the
 * notion of role (corresponding to the IService class).
 * \li IService : API to normalize manipulation, using a limited set of well defined methods
 * \li IService : contract for service implementation
 * \li macros.hpp : declaration of service to object bindings
 */
namespace fwServices
{

/**
 * @page AppConfig AppConfig
 *
 * The FW4SPL architecture provides a dynamic management of configurations to allow the use of multiple objects and
 * services.
 *
 * The xml configuration for an application is defines with the extension ``::fwServices::registry::AppConfig``.
 *
 * @code{.xml}
     <extension implements="::fwServices::registry::AppConfig">
         <id>myAppConfigId</id>
         <parameters>
             <param name="appName" default="my Application" />
             <param name="appIconPath" />
         </parameters>
         <desc>Image Viewer</desc>
         <config>

             <object type="::fwData::Composite">

                 <!--
                     Description service of the GUI:
                     The ::gui::frame::SDefaultFrame service automatically positions the various
                     containers in the application main window.
                     Here, it declares a container for the 3D rendering service.
                 -->
                 <service uid="myFrame" impl="::gui::frame::SDefaultFrame">
                     <gui>
                         <frame>
                             <name>${appName}</name>
                             <icon>${appIconPath}</icon>
                             <minSize width="800" height="600" />
                         </frame>
                     </gui>
                     <registry>
                         <!-- Associate the container for the rendering service. -->
                         <view sid="myRendering" />
                     </registry>
                 </service>

                 <item key="myImage">
                     <object uid="myImageUid" type="::fwData::Image">
                         <!--
                             Reading service:
                             The <file> tag defines the path of the image to load. Here, it is a relative
                             path from the repository in which you launch the application.
                         -->
                         <service uid="myReaderPathFile" impl="::ioVTK::SImageReader">
                            <file>./TutoData/patient1.vtk</file>
                         </service>

                         <!--
                             Visualization service of a 3D medical image:
                             This service will render the 3D image.
                         -->
                         <service uid="myRendering" impl="::vtkSimpleNegato::SRenderer" />
                     </object>
                 </item>

                 <!--
                     Definition of the starting order of the different services:
                     The frame defines the 3D scene container, so it must be started first.
                     The services will be stopped the reverse order compared to the starting one.
                 -->
                 <start uid="myFrame" />
                 <start uid="myReaderPathFile" />
                 <start uid="myRendering" />

                 <!--
                     Definition of the service to update:
                     The reading service load the data on the update.
                     The render update must be called after the reading of the image.
                 -->
                 <update uid="myReaderPathFile" />
                 <update uid="myRendering" />

             </object>

         </config>
     </extension>
   @endcode
 *
 * - \b id: it is the configuration identifier
 * - \b parameters (optional): it defines the list of the parameters used by the configuration
 * - \b param: defines a parameter
 *   - \b name: parameter name, used as "${paramName}" in the configuration. It will be replaced by the string
 *     defined by the service, activity or application that launchs the configuration.
 *   - \b default(optional): default value for the parameter, it is used if the value is not given by the config
 *     launcher.
 * - \b desc (optional): it is the description of the application
 * - \b config: it defines the services and objects to launch
 *
 * - \b object: it defines an object of the AppConfig. We usually use a ::fwData::Composite in order to add
 *   sub-objects. An object can defines a list of services. Some object object can have a specific configuration :
 *   ::fwData::TransformationMatrix3D, ::fwData::Float, ::fwData::List, ...
 *    - \b uid (optional): unique identifier of the object (::fwTools::fwID). If it is not defined, it will be
 *      automatically generated.
 *    - \b type: object type (ex: ::fwData::Image, ::fwData::Composite)
 *    - \b src (optional, "new" by default): defines if the object should be created ("new") or if it already
 *      exists in the application ("ref"). In the last case, the uid must be the same as the first declaration of
 *      this object (with "new").
 * - \b service: it represents a service working on the object. Some services needs a specific configuration, it is
 *   usually described in the doxygen of the method configuring().
 *     - \b uid (optional): unique identifier of the service. If it is not defined, it will be automatically
 *       generated.
 *     - \b impl: service implementation
 *     - \b type (optional): service type (ex: ::fwGui::IFrameSrv)
 *     - \b autoConnect (optional, "no" by default): defines if the service listen the signals of the working object
 *     - \b worker (optional): allows to run the service in another worker (see ::fwThread::Worker)
 *
 * - \b matrix (optional): it works only for ::fwData::TransformationMatrix3D objects. It defines the value of the
 *   matrix.
 *
 * @code{.xml}
    <object uid="matrix" type="::fwData::TransformationMatrix3D">
        <matrix>
         <![CDATA[
         1  0  0  0
         0  1  0  0
         0  0  1  0
         0  0  0  1
         ]]>
         </matrix>
    </object>
   @endcode
 *
 * - \b value (optional): it works only for ::fwData::Boolean, ::fwData::Integer, ::fwData::Float and
 *   ::fwData::String. It allows to define the value of the object.
 *
 * @code{.xml}
    <object type="::fwData::Integer">
        <value>42</value>
    </object>
   @endcode
 *
 * - \b item (optional): it defines a sub-object of a composite. It can only be used if the parent object is a
 *   ::fwData::Composite.
 *    - \b key: key of the object in the composite
 *    - \b object: the 'item' tag can only contain 'object' tags that represents the composite sub-object
 *
 * @code{.xml}
    <item key="myImage">
        <object uid="myImageUid" type="::fwData::Image" />
    </item>
   @endcode
 *
 * - \b colors (optional): it works only for ::fwData::TransferFunction. It allows to fill the transfer function values.
 *
 * @code{.xml}
     <object type="::fwData::TransferFunction">
         <colors>
             <step color="#ff0000ff" value="1" />
             <step color="#ffff00ff" value="500" />
             <step color="#00ff00ff" value="1000" />
             <step color="#00ffffff" value="1500" />
             <step color="#0000ffff" value="2000" />
             <step color="#000000ff" value="4000" />
         </colors>
     </object>
   @endcode
 *
 * - \b connect (optional): allows to connect a signal to one or more slot(s). The signal and slots must be
 *   compatible.
 *
 * @code{.xml}
     <connect>
         <signal>object_uid/signal_name</signal>
         <slot>service_uid/slot_name</slot>
     </connect>
   @endcode
 *
 * - \b proxy (optional): allows to connect one or more signal(s) to one or more slot(s). The signals and slots must
 *   be compatible.
 *    - \b channel: name of the channel use for the proxy.
 *
 * @code{.xml}
    <proxy channel="myChannel">
        <signal>object_uid/signal_name</signal>
        <slot>service_uid/slot_name</slot>
    </proxy>
    @endcode
 *
 * - \b start: defines the service to start when the AppConfig is launched. The services will be automatically
 *   stopped in the reverse order when the AppConfig is stopped.
 *
 * @code{.xml}
    <start uid="service_uid" />
   @endcode
 *
 * - \b update: defines the service to update when the AppConfig is launched.
 *
 * @code{.xml}
    <update uid="service_uid" />
   @endcode
 */


/**
 * @page AppConfig2 AppConfig2
 *
 * The FW4SPL architecture provides a dynamic management of configurations to allow the use of multiple objects and
 * services.
 *
 * The xml configuration for an application is defines with the extension ``::fwServices::registry::AppConfig2``.
 *
 * @code{.xml}
     <extension implements="::fwServices::registry::AppConfig2">
         <id>myAppConfigId</id>
         <parameters>
             <param name="appName" default="my Application" />
             <param name="appIconPath" />
         </parameters>
         <desc>Image Viewer</desc>
         <config>

             <object uid="myImage" type="::fwData::Image" />

             <!--
                 Description service of the GUI:
                 The ::gui::frame::SDefaultFrame service automatically positions the various
                 containers in the application main window.
                 Here, it declares a container for the 3D rendering service.
             -->
             <service uid="myFrame" type="::gui::frame::SDefaultFrame">
                 <gui>
                     <frame>
                         <name>${appName}</name>
                         <icon>${appIconPath}</icon>
                         <minSize width="800" height="600" />
                     </frame>
                 </gui>
                 <registry>
                     <!-- Associate the container for the rendering service. -->
                     <view sid="myRendering" />
                 </registry>
             </service>

             <!--
                 Reading service:
                 The <file> tag defines the path of the image to load. Here, it is a relative
                 path from the repository in which you launch the application.
             -->
             <service uid="myReaderPathFile" type="::ioVTK::SImageReader">
                <inout key="target" uid="myImage" />
                <file>./TutoData/patient1.vtk</file>
             </service>

             <!--
                 Visualization service of a 3D medical image:
                 This service will render the 3D image.
             -->
             <service uid="myRendering" type="::vtkSimpleNegato::SRenderer">
                <in key="image" uid="myImage" />
             </service>

             <!--
                 Definition of the starting order of the different services:
                 The frame defines the 3D scene container, so it must be started first.
                 The services will be stopped the reverse order compared to the starting one.
             -->
             <start uid="myFrame" />
             <start uid="myReaderPathFile" />
             <start uid="myRendering" />

             <!--
                 Definition of the service to update:
                 The reading service load the data on the update.
                 The render update must be called after the reading of the image.
             -->
             <update uid="myReaderPathFile" />
             <update uid="myRendering" />

         </config>
     </extension>
   @endcode
 *
 * @section Definition Definition
 * @subsection Parameters Parameters
 * - \b id: it is the configuration identifier
 * - \b parameters (optional): it defines the list of the parameters used by the configuration
 * - \b param: defines a parameter
 *   - \b name: parameter name, used as "${paramName}" in the configuration. It will be replaced by the string
 *     defined by the service, activity or application that launchs the configuration.
 *   - \b default(optional): default value for the parameter, it is used if the value is not given by the config
 *     launcher.
 * - \b desc (optional): it is the description of the application
 * - \b config: it defines the services and objects to launch
 *
 * @subsection Object Object
 * - \b object: it defines an object of the AppConfig.
 *    - \b uid (optional): unique identifier of the object (::fwTools::fwID). If it is not defined, it will be
 *      automatically generated.
 *    - \b type: object type (ex: ::fwData::Image, ::fwData::Mesh)
 *    - \b src (optional, "new" by default, values: "new", "ref", "deferred"):
 *        - "new" : defines that the object should be created
 *        - "ref" : defines that the object already exists in the application. The uid must be the same as the first
 *            declaration of this object (with "new").
 *        - "deferred" : defines that the object will be created later (by a service).
 *
 * Some objects can have a specific configuration :
 * - \b matrix (optional): it works only for ::fwData::TransformationMatrix3D objects. It defines the value of the
 *   matrix.
 *
 * @code{.xml}
    <object uid="matrix" type="::fwData::TransformationMatrix3D">
        <matrix>
         <![CDATA[
         1  0  0  0
         0  1  0  0
         0  0  1  0
         0  0  0  1
         ]]>
         </matrix>
    </object>
   @endcode
 *
 * - \b value (optional): it works only for ::fwData::Boolean, ::fwData::Integer, ::fwData::Float and
 *   ::fwData::String. It allows to define the value of the object.
 *
 * @code{.xml}
    <object type="::fwData::Integer">
        <value>42</value>
    </object>
   @endcode
 *
 * - \b colors (optional): it works only for ::fwData::TransferFunction. It allows to fill the transfer function values.
 *
 * @code{.xml}
     <object type="::fwData::TransferFunction">
         <colors>
             <step color="#ff0000ff" value="1" />
             <step color="#ffff00ff" value="500" />
             <step color="#00ff00ff" value="1000" />
             <step color="#00ffffff" value="1500" />
             <step color="#0000ffff" value="2000" />
             <step color="#000000ff" value="4000" />
         </colors>
     </object>
   @endcode
 *
 * - \b item (optional): it defines a sub-object of a composite or a field of any other object.
 *    - \b key: key of the object
 *    - \b object: the 'item' tag can only contain 'object' tags that represents the sub-object
 *
 * @code{.xml}
    <item key="myImage">
        <object uid="myImageUid" type="::fwData::Image" />
    </item>
   @endcode
 *
 * @subsection Service Service
 * - \b service: it represents a service working on the object. Services list the data the use and how they access them.
 *   Some services needs a specific configuration, it is usually described in the doxygen of the method configuring().
 *     - \b uid (optional): unique identifier of the service. If it is not defined, it will be automatically
 *       generated.
 *     - \b type: service type (ex: ::ioVTK::SImageReader)
 *     - \b autoConnect (optional, "no" by default): defines if the service listen the signals of the working objects
 *     - \b worker (optional): allows to run the service in another worker (see ::fwThread::Worker)
 *
 * @code{.xml}
    <service uid="mesher" type="::opMesh::SMesher">
        <in key="image" uid="imageId" />
        <out key="mesh" uid="meshId" />
    </service>
   @endcode
 *
 * - \b in : input object, it is const and cannot be modified
 * - \b inout : input object that can be modified
 * - \b out : output object, it must be created by the service and registered with the 'setOutput(key, obj)' method.
 *      The output object must be declared as "deferred" in the \<object\> declaration.
 *   - \b key: Object key used to retreive the object into the service
 *   - \b uid: Uid of the object declared in the <object> tag
 *   - \b optional(optional, default "no", values: "yes" or "no") If "yes", the service can be started even if the
 *        object is not present. The output objects are always optional.
 *
 * @code{.cpp}
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >("image");
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    // mesher .....
    this->setOutput("mesh", mesh);
   @endcode
 *
 * - \b connect (optional): allows to connect one or more signal(s) to one or more slot(s). The signals and slots must
 *   be compatible.
 *    - \b channel (optional): name of the channel use for the connections.
 *
 * @code{.xml}
    <connect channel="myChannel">
        <signal>object_uid/signal_name</signal>
        <slot>service_uid/slot_name</slot>
    </connect>
    @endcode
 *
 * - \b start: defines the service to start when the AppConfig is launched. The services will be automatically
 *   stopped in the reverse order when the AppConfig is stopped.
 *
 * @code{.xml}
    <start uid="service_uid" />
   @endcode
 *
 * The service using "deferred" object as input will be automatically started when the object is created.
 *
 * - \b update: defines the service to update when the AppConfig is launched.
 *
 * @code{.xml}
    <update uid="service_uid" />
   @endcode
 */

}
#endif /* __FWSERVICES_NAMESPACE_HPP__ */
