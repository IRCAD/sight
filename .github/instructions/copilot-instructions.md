---
description: Sight coding guidelines
---

This is Sight, the Surgical Image Guidance and Healthcare Toolkit.

Sight is based on a component architecture composed of C++ libraries. It uses an object-service concept approach.
Object and services communicates using signals and slots.

It is mainly written in C++20. Components and applications are described with XML files.

The documentation is available at https://sight.pages.ircad.fr/sight-doc/.

## Code Conventions

- All code and comments are in english.
- Tab are set to 4 spaces and indentation is done with spaces, not tabs.
- Use snake_case for variables, functions and class names.
- Try to respect as much as possible the isocpp guidelines at https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
- Maximum line length is 120 characters
- Class members variables are prefixed with "m_"
- We use clang-tidy to enforce the coding style, rules are defined in the .clang-tidy file at the root of the project.


## File hierarchy

The codebase is organized as follows:
- /app: applications written in XML
- /cmake: build files with CMake
- /config: dynamic components containing only shared configurations in XML
- /example: more elaborated code than tutorials
- /lib: shared code
- /module: dynamic components that can be assigned to an application. Most services are implemented inside modules.
- /tutorial: simple samples that demonstrate basic features with an increasing difficulty
- /util: standalone executables

## Includes

Includes headers should always be minimal.
For header guards, we use the `#pragma once` directive.

## Namespaces

We use namespaces everywhere, and they follow the file tree layout from the root folders.
In C++, we specify namespaces on a single line


| Folder | Namespace | Example folder | Example namespace |
| --- | --- | --- | --- |
| app | sight::app_name | app/viewer | sight::viewer |
| config | sight::config::config_name | config/filter/image | sight::filter::image |
| lib | sight::lib_name | lib/filter/image | sight::filter::image |
| module | sight::module::module_name | lib/filter/image | sight::module::filter::image |

Creating a new module is described in https://sight.pages.ircad.fr/sight-doc/HowTos/ModuleCreation.html
Creating a new service is described in https://sight.pages.ircad.fr/sight-doc/HowTos/ServiceCreation.html#howtosservicecreation

## Testing

Unit-tests are present in each CMake target with C++ code. They lie in the target under the `test/ut` folder.
We use Doctest for unit testing. Each cpp file contains one or more TEST_SUITE(), named after the C++ class for instance
it tests (i.e. "sight::data::image"). The test name is free but must be comprehensive and not hold a redundant "test"
prefix or suffix.

## How-tos

### Create a new service

[//]: #cspell:ignore noexcept

To create a new service in a module, you must:

1. write a .hpp file with the following skeleton:

```cpp
namespace sight::module::my_module
{

class my_service final : public sight::service::base
{
public:
    SIGHT_DECLARE_SERVICE(my_service, sight::service::base);

    // Service constructor
    my_service() noexcept = default;

    // Service destructor.
    ~my_service() noexcept final = default;

protected:

    // To configure the service
    void configuring(const config_t& _config) final;

    // To start the service
    void starting() final;

    // To stop the service
    void stopping() final;

    // To update the service
    void updating() final;
};

} // namespace sight::module::my_module

```

A new service can be created by taking input from various data types, such as those found in the `lib/__/data`
directory. These data are always smart-pointed and registered with key name using `sight::data::ptr` for single data
or `sight::data::ptr_vector` for multiple data, for instance for `sight::data::image` :

```cpp
sight::data::ptr<sight::data::image> m_image { this, "image" };
```

See https://sight.pages.ircad.fr/sight-doc/SAD/src/ObjService.html for more details.

2. A corresponding .cpp file that implements the above methods.
3. Register the service in the `rc/plugin.xml` file, for example:

```xml
    <extension implements="::sight::service::extension::factory">
        <service>sight::module::my_module</service>
        <type>sight::service::base</type>
        <object>sight::data::image</object>
    </extension>
```

4. Create the associated unit-test in the `test/ut` folder.

### Create a unit-test for a service

Every service must-be unit-tested. It is recommended to use a test fixture to initialize and destroy the service, this
way it is properly unregistered if the test fails, allowing other tests to be launched. Here is an example with a
service that works on an image.

```cpp
#include <data/image.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::my_service")
{
    class service_fixture
    {
    public:

        service_fixture()
        {
            service = sight::service::add("sight::module::my_service");
            REQUIRE(service);
            REQUIRE(service->is_a("sight::module::my_service"));

            image = std::make_shared<sight::data::image>();
            service->set_input(image, "image");
        }

        ~service_fixture()
        {
            if(service->started())
            {
                service->stop().get();
            }
            sight::service::remove(service);
        }

        sight::service::base::sptr service;
        sight::data::image::sptr image;
    };

    TEST_CASE_FIXTURE(service_fixture, "test_1")
    {
        std::string config_string ="<properties prop1='90' prop2='30'/>";
        service->set_config(config_string);
        service->configure();
        service->start().get();
        service->update().get();

        CHECK(...);
    }

    TEST_CASE_FIXTURE(service_fixture, "test_2")
    {
        std::string config_string ="<properties prop1='90' prop2='30'/>";
        service->set_config(config_string);
        service->configure();
        service->start().get();
        service->update().get();

        CHECK(...);
    }
}
```

The filename must be `test/ut/my_service_test.cpp`.

In tests, services must be instantiated using the factory:

```cpp
auto srv = sight::service::add("sight::module::my_service");
```
and removed using:

```cpp
sight::service::remove(srv);
```

Services data (input/inout) must be setup as usual following the instructions in
https://sight.pages.ircad.fr/sight-doc/SAD/src/ObjService.html#object-service-registration.

Sight services always protect their member variables so they can't be tested. Instead we usually monitor the input/inout
data. We can also bind local slots in the test to services signals.

Here is an example to test for instance to test the internal state of a service, assuming the `updating()` method of
the service triggers the `sample_signal`.

```cpp
    bool call_slot = false;
    auto slot_called = sight::core::com::new_slot(
        [&call_slot]()
        {
            call_slot = true;
        });

    slot_called->set_worker(sight::core::thread::get_default_worker());
    srv->signal("sample_signal")->connect(slot_called);

    srv->update().get();

    SIGHT_TEST_WAIT(call_slot == true);
```
