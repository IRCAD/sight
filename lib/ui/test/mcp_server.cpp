/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "ui/test/mcp_server.hpp"

#include "ui/test/detail/locator.hpp"
#include "ui/test/detail/main_window.hpp"
#include "ui/test/detail/widget_text.hpp"
#include "ui/test/expectation.hpp"

#include <core/runtime/runtime_exception.hpp>

#include <ui/__/preferences.hpp>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSlider>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QMetaObject>
#include <QProgressBar>
#include <QSpinBox>
#include <QWidget>

#include <nlohmann/json.hpp>

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace sight::ui::test
{

using json = nlohmann::json;

namespace
{

constexpr std::array PROTOCOL_VERSIONS {"2025-11-25", "2025-06-18", "2025-03-26"};

/// The longest request line accepted, to bound the memory a client without a line end could take.
constexpr std::size_t MAX_REQUEST_SIZE = std::size_t {1024} *1024;

/// The verbs of `do` that take no locator: they answer the file dialog in front.
constexpr std::array DIALOG_VERBS {"choose_file", "choose_files", "choose_folder", "cancel_file_dialog"};

} // namespace

//------------------------------------------------------------------------------

static bool is_dialog_verb(std::string_view _verb)
{
    return std::ranges::find(DIALOG_VERBS, _verb) != DIALOG_VERBS.end();
}

//------------------------------------------------------------------------------

static json tool()
{
    // A point of the widget, from (0, 0) at the top left to (1, 1) at the bottom right.
    const json position {
        {"type", "object"},
        {"properties", {{"x", {{"type", "number"}}}, {"y", {{"type", "number"}}}}},
        {"required", {"x", "y"}}
    };
    const auto described = [](json _schema, const std::string& _verbs)
                           {
                               _schema["description"] = _verbs + " From (0, 0) at the top left of the widget to (1, 1)"
                                                                 " at the bottom right.";
                               return _schema;
                           };

    const json text {{"type", "string"}};
    const json flag {{"type", "boolean"}};

    return {
        {"name", "ui"},
        {"description",
         "Drive the application as a GUI test does, one call per scenario line. Without arguments, return the tree "
         "of the names a locator can use, as the user sees it now: only visible widgets, so scroll_to or open what "
         "hides the others, and call again right after the start while it still builds. With a locator, return the "
         "state of that widget, or the tree for an action. 'do' acts first, as a user does; 'expect' then waits for "
         "a state, as ui.expect() does. An unknown name fails with the closest known ones."
        },
        {"inputSchema",
         {
             {"type", "object"},
             {"properties",
              {
                  {"locator", {{"type", "string"}, {"description", "A name or a scoped locator, as in a scenario."}}},
                  {"do",
                   {
                       {"type", "string"},
                       {"enum",
                        {"click", "double_click", "right_click", "drag", "wheel", "pinch", "type", "press", "set",
                         "select", "scroll_to", "close", "choose_file", "choose_files", "choose_folder",
                         "cancel_file_dialog"
                        }
                       }
                   }
                  },
                  {"value", {{"type", {"boolean", "number", "string"}}, {"description", "set: the new value."}}},
                  {"text", {{"type", "string"}, {"description", "type: the text; select: the item."}}},
                  {"key", {{"type", "string"}, {"description", "press: a key sequence, such as Ctrl+S."}}},
                  {"at", described(position, "click: the point clicked, the centre by default; drag: the start.")},
                  {"to", described(position, "drag: the end, which may lie outside the widget.")},
                  {"notches", {{"type", "integer"}, {"description", "wheel: notches, positive away from the user."}}},
                  {"scale", {{"type", "number"}, {"description", "pinch: the zoom factor."}}},
                  {"path",
                   {
                       {"anyOf", {text, {{"type", "array"}, {"items", text}}}},
                       {"description", "choose_file, choose_folder: a path; choose_files: a list of paths."}
                   }
                  },
                  {"filter", {{"type", "string"}, {"description", "choose_file, choose_files: the filter to pick."}}},
                  {"expect",
                   {
                       {"type", "object"},
                       {"description", "States to wait for, each as ui.expect() checks it."},
                       {"properties",
                        {
                            {"text", text},
                            {"text_contains", text},
                            {"tooltip", text},
                            {"value", {{"type", {"boolean", "number"}}}},
                            {"rows", {{"type", "integer"}}},
                            {"checked", flag},
                            {"enabled", flag},
                            {"visible", flag}
                        }
                       },
                       {"additionalProperties", false}
                   }
                  },
                  {"timeout_ms", {{"type", "integer"}, {"description", "Deadline of each step, 30000 by default."}}}
              }
             }
         }
        }
    };
}

//------------------------------------------------------------------------------

static std::filesystem::path path_of(const json& _value)
{
    const auto text = _value.get<std::string>();
    return {std::u8string(text.begin(), text.end())};
}

//------------------------------------------------------------------------------

static relative_position position_of(const json& _value)
{
    return at(_value.at("x").get<double>(), _value.at("y").get<double>());
}

//------------------------------------------------------------------------------

/// What a test can check on a widget, as JSON.
static json state_of(const QWidget& _widget)
{
    json state {
        {"name", _widget.objectName().toStdString()},
        {"class", _widget.metaObject()->className()},
        {"visible", _widget.isVisible()},
        {"enabled", _widget.isEnabled()}
    };

    if(const auto text = detail::displayed_text(_widget); text.has_value())
    {
        state["text"] = text->toStdString();
    }

    if(!_widget.toolTip().isEmpty())
    {
        state["tooltip"] = _widget.toolTip().toStdString();
    }

    if(const auto* const button = qobject_cast<const QAbstractButton*>(&_widget);
       button != nullptr && button->isCheckable())
    {
        state["checked"] = button->isChecked();
    }
    else if(const auto* const slider = qobject_cast<const QAbstractSlider*>(&_widget); slider != nullptr)
    {
        state["value"] = slider->value();
    }
    else if(const auto* const spin_box = qobject_cast<const QSpinBox*>(&_widget); spin_box != nullptr)
    {
        state["value"] = spin_box->value();
    }
    else if(const auto* const double_spin_box = qobject_cast<const QDoubleSpinBox*>(&_widget);
            double_spin_box != nullptr)
    {
        state["value"] = double_spin_box->value();
    }
    else if(const auto* const progress = qobject_cast<const QProgressBar*>(&_widget); progress != nullptr)
    {
        state["value"] = progress->value();
    }
    else if(const auto* const combo = qobject_cast<const QComboBox*>(&_widget); combo != nullptr)
    {
        state["value"] = combo->currentIndex();
    }
    else if(const auto* const view = qobject_cast<const QAbstractItemView*>(&_widget);
            view != nullptr && view->model() != nullptr)
    {
        state["rows"] = view->model()->rowCount(view->rootIndex());
    }

    return state;
}

//------------------------------------------------------------------------------

/// Refuses a parameter the call does not use, which would otherwise be ignored.
static void check_parameters(const json& _call)
{
    // The parameters each verb of `do` reads, besides its locator.
    static const std::map<std::string, std::vector<std::string_view>, std::less<> > s_PARAMETERS {
        {"click", {"at"}},
        {"drag", {"at", "to"}},
        {"wheel", {"notches"}},
        {"pinch", {"scale"}},
        {"type", {"text"}},
        {"press", {"key"}},
        {"set", {"value"}},
        {"select", {"text"}},
        {"choose_file", {"path", "filter"}},
        {"choose_files", {"path", "filter"}},
        {"choose_folder", {"path"}}
    };

    static constexpr std::array s_GENERAL {std::string_view("locator"), std::string_view("do"),
                                           std::string_view("expect"), std::string_view("timeout_ms")
    };

    const auto name  = _call.value("do", std::string());
    const auto found = s_PARAMETERS.find(name);
    for(const auto& item : _call.items())
    {
        const std::string& parameter = item.key();
        const bool general           = std::ranges::find(s_GENERAL, parameter) != s_GENERAL.end();
        const bool read_by_verb      = found != s_PARAMETERS.end()
                                       && std::ranges::find(found->second, parameter) != found->second.end();
        if(!general && !read_by_verb)
        {
            throw std::invalid_argument(
                      "'" + parameter + "' is not used " + (name.empty() ? "without 'do'" : "by '" + name + "'")
            );
        }
    }
}

//------------------------------------------------------------------------------

/// Performs the `do` of a call, with the scenario verb of the same name.
static void act(scenario& _ui, const json& _call, duration_t _timeout)
{
    using verb = void (*)(scenario&, const std::string&, const json&, duration_t);

    // The verbs of `do`, each the scenario verb of the same name.
    static const std::map<std::string, verb, std::less<> > s_VERBS {
        {"click", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.click(_l, _c.contains("at") ? position_of(_c["at"]) : relative_position {}, _t);
         }
        },
        {"double_click", [](scenario& _s, const std::string& _l, const json&, duration_t _t){_s.double_click(_l, _t);}},
        {"right_click", [](scenario& _s, const std::string& _l, const json&, duration_t _t){_s.right_click(_l, _t);}},
        {"drag", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.drag(_l, position_of(_c.at("at")), position_of(_c.at("to")), _t);
         }
        },
        {"wheel", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.wheel(_l, _c.at("notches").get<int>(), _t);
         }
        },
        {"pinch", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.pinch(_l, _c.at("scale").get<double>(), _t);
         }
        },
        {"type", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.type(_l, _c.at("text").get<std::string>(), _t);
         }
        },
        {"press", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.press(_l, _c.at("key").get<std::string>(), _t);
         }
        },
        {"set", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             const json& value = _c.at("value");
             if(value.is_boolean())
             {
                 _s.set(_l, value.get<bool>(), _t);
             }
             else if(value.is_number_integer())
             {
                 _s.set(_l, value.get<int>(), _t);
             }
             else if(value.is_number())
             {
                 _s.set(_l, value.get<double>(), _t);
             }
             else
             {
                 _s.set(_l, value.get<std::string>(), _t);
             }
         }
        },
        {"select", [](scenario& _s, const std::string& _l, const json& _c, duration_t _t)
         {
             _s.select(_l, _c.at("text").get<std::string>(), _t);
         }
        },
        {"scroll_to", [](scenario& _s, const std::string& _l, const json&, duration_t _t){_s.scroll_to(_l, _t);}},
        {"close", [](scenario& _s, const std::string& _l, const json&, duration_t _t){_s.close(_l, _t);}},
        {"choose_file", [](scenario& _s, const std::string&, const json& _c, duration_t _t)
         {
             _s.choose_file(path_of(_c.at("path")), _c.value("filter", std::string()), _t);
         }
        },
        {"choose_files", [](scenario& _s, const std::string&, const json& _c, duration_t _t)
         {
             std::vector<std::filesystem::path> paths;
             std::ranges::transform(_c.at("path"), std::back_inserter(paths), path_of);
             _s.choose_files(paths, _c.value("filter", std::string()), _t);
         }
        },
        {"choose_folder", [](scenario& _s, const std::string&, const json& _c, duration_t _t)
         {
             _s.choose_folder(path_of(_c.at("path")), _t);
         }
        },
        {"cancel_file_dialog", [](scenario& _s, const std::string&, const json&, duration_t _t)
         {
             _s.cancel_file_dialog(_t);
         }
        }
    };

    const auto name    = _call.at("do").get<std::string>();
    const auto locator = _call.value("locator", std::string());
    const auto found   = s_VERBS.find(name);
    if(found == s_VERBS.end())
    {
        throw std::invalid_argument("unknown verb '" + name + "'");
    }

    if(locator.empty() && !is_dialog_verb(name))
    {
        throw std::invalid_argument("'" + name + "' needs a locator");
    }

    found->second(_ui, locator, _call, _timeout);
}

//------------------------------------------------------------------------------

/// Waits for each state of `expect`, with the expectation of the same name.
static void wait(scenario& _ui, const json& _call, duration_t _timeout)
{
    const auto locator = _call.value("locator", std::string());
    if(locator.empty())
    {
        throw std::invalid_argument("'expect' needs a locator");
    }

    for(const auto& [state, expected] : _call.at("expect").items())
    {
        auto expectation = _ui.expect(locator);
        static_cast<void>(expectation.within(_timeout));
        if(state == "text")
        {
            expectation.text(expected.get<std::string>());
        }
        else if(state == "text_contains")
        {
            expectation.text_contains(expected.get<std::string>());
        }
        else if(state == "tooltip")
        {
            expectation.tooltip(expected.get<std::string>());
        }
        else if(state == "rows")
        {
            expectation.rows(expected.get<int>());
        }
        else if(state == "checked" || (state == "value" && expected.is_boolean()))
        {
            expected.get<bool>() ? expectation.checked() : expectation.unchecked();
        }
        else if(state == "value")
        {
            expected.is_number_integer() ? expectation.value(expected.get<int>())
                                         : expectation.value(expected.get<double>());
        }
        else if(state == "enabled")
        {
            expected.get<bool>() ? expectation.enabled() : expectation.disabled();
        }
        else if(state == "visible")
        {
            expected.get<bool>() ? expectation.visible() : expectation.hidden();
        }
        else
        {
            throw std::invalid_argument("unknown state '" + state + "'");
        }
    }
}

//------------------------------------------------------------------------------

static json text_result(const std::string& _text, bool _error = false)
{
    return {{"content", {{{"type", "text"}, {"text", _text}}}}, {"isError", _error}};
}

//------------------------------------------------------------------------------

/// The state of a widget, or the tree for a target without one: an action, or a widget its action closed.
static std::string look(scenario& _ui, const std::string& _locator, duration_t _timeout)
{
    const bool found = _ui.exists(_locator, _timeout);
    try
    {
        // One look, which the GUI answers however short the timeout.
        return _ui.read(_locator, state_of, std::chrono::milliseconds(1)).dump(2);
    }
    catch(const std::exception&)
    {
        if(!found)
        {
            throw;
        }

        return _ui.dump();
    }
}

//------------------------------------------------------------------------------

/// One call of the tool: act, wait, then return what to look at next.
static json call_ui(scenario& _ui, const json& _call)
{
    try
    {
        check_parameters(_call);

        const auto timeout = std::chrono::milliseconds(_call.value("timeout_ms", 30000));
        if(_call.contains("do"))
        {
            act(_ui, _call, timeout);
        }

        if(_call.contains("expect"))
        {
            wait(_ui, _call, timeout);
        }

        // After a close or a dialog, the widget may be gone: the tree tells what the user sees now. After another
        // action the target was there an instant ago: no need to wait for it again.
        const auto locator = _call.value("locator", std::string());
        const auto verb    = _call.value("do", std::string());
        if(locator.empty() || verb == "close" || is_dialog_verb(verb))
        {
            return text_result(_ui.dump());
        }

        return text_result(look(_ui, locator, verb.empty() ? timeout : std::chrono::milliseconds(1)));
    }
    catch(const std::exception& e)
    {
        return text_result(e.what(), true);
    }
}

//------------------------------------------------------------------------------

static json error(const json& _id, int _code, const std::string& _message)
{
    return {{"jsonrpc", "2.0"}, {"id", _id}, {"error", {{"code", _code}, {"message", _message}}}};
}

/// The refusal of a request longer than the limit, which is not read.
static std::string too_long()
{
    return error(nullptr, -32600, "request longer than 1 MiB").dump();
}

/// The response to a request of the client, whose fields have the expected types.
static std::string answer_request(scenario& _ui, const json& _id, const std::string& _method, const json& _params)
{
    const auto respond = [&_id](json _result){return json {{"jsonrpc", "2.0"}, {"id", _id}, {"result", _result}};};
    const auto requested = _params.value("protocolVersion", std::string());

    if(_method == "initialize")
    {
        // The version of the client if this server speaks it, else the latest this one does.
        const bool known = std::ranges::find(PROTOCOL_VERSIONS, requested) != PROTOCOL_VERSIONS.end();
        return respond(
            {
                {"protocolVersion", known ? requested : PROTOCOL_VERSIONS.front()},
                {"capabilities", {{"tools", json::object()}}},
                {"serverInfo", {{"name", "sight"}, {"version", "1.0"}}},
                {"instructions",
                 "Explore a running Sight application with the 'ui' tool, then write what works as a GUI test: see "
                 ".agents/skills/sight-gui-test/SKILL.md."
                }
            }).dump();
    }

    if(_method == "ping")
    {
        return respond(json::object()).dump();
    }

    if(_method == "tools/list")
    {
        return respond({{"tools", {tool()}}}).dump();
    }

    if(_method == "tools/call")
    {
        if(_params.value("name", std::string()) != "ui")
        {
            return error(_id, -32602, "unknown tool: the only one is 'ui'").dump();
        }

        return respond(call_ui(_ui, _params.value("arguments", json::object()))).dump();
    }

    return error(_id, -32601, "unknown method '" + _method + "'").dump();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

mcp_server::mcp_server(scenario& _ui) :
    m_ui(_ui)
{
}

//------------------------------------------------------------------------------

std::optional<std::string> mcp_server::answer(std::string_view _message)
{
    if(_message.ends_with('\r'))
    {
        _message.remove_suffix(1);
    }

    const json message = json::parse(_message, nullptr, false);
    if(message.is_discarded())
    {
        return error(nullptr, -32700, "not JSON").dump();
    }

    if(!message.is_object() || !message.contains("method") || !message["method"].is_string()
       || !message.value("params", json::object()).is_object())
    {
        return error(message.is_object() ? message.value("id", json()) : json(), -32600, "not a JSON-RPC request")
               .dump();
    }

    // A notification, initialized or cancelled, needs no answer: calls run one after the other anyway.
    if(!message.contains("id"))
    {
        return std::nullopt;
    }

    const json& id = message["id"];
    try
    {
        return answer_request(m_ui, id, message["method"].get<std::string>(), message.value("params", json::object()));
    }
    catch(const json::exception& e)
    {
        // Valid JSON of the wrong shape, such as a number where a text is expected.
        return error(id, -32602, std::string("invalid params: ") + e.what()).dump();
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/// The standard output kept for the protocol, once reserved.
static std::optional<int> s_output;

//------------------------------------------------------------------------------

struct mcp_stdio::implementation
{
    explicit implementation(QApplication& _application) :
        ui(make_scenario(_application)),
        server(*ui),
        reader([this]{serve(); served = true;})
    {
    }

    ~implementation()
    {
        stopping = true;
#ifdef WIN32
        // A console read waits for a line of the user, which may never come: it is cancelled.
        while(!served)
        {
            ::CancelSynchronousIo(reader.native_handle());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#endif
        reader.join();
    }

    implementation(const implementation&)            = delete;
    implementation(implementation&&)                 = delete;
    implementation& operator=(const implementation&) = delete;
    implementation& operator=(implementation&&)      = delete;

    /// With the names of the XML of the application, if it has some: a bare test application has none.
    static std::unique_ptr<scenario> make_scenario(QApplication& _application)
    {
        try
        {
            return std::make_unique<scenario>(_application, detail::locator_catalog::from_loaded_modules());
        }
        catch(const core::runtime::runtime_exception&)
        {
            return std::make_unique<scenario>(_application);
        }
    }

    //------------------------------------------------------------------------------

    static int reserved_output()
    {
        prepare();
        return *s_output;
    }

    /// Waits a little for input, so that the reader sees `stopping` in time.
    static bool input_ready()
    {
#ifdef WIN32
        DWORD available = 0;
        if(::PeekNamedPipe(::GetStdHandle(STD_INPUT_HANDLE), nullptr, 0, nullptr, &available, nullptr) == 0)
        {
            // Closed, or not a pipe (a console, a file): the read tells, and the destructor cancels it.
            return true;
        }

        if(available == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return available > 0;
#else
        pollfd input {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
        return ::poll(&input, 1, 100) != 0;
#endif
    }

    //------------------------------------------------------------------------------

    void write(std::string _line) const
    {
        _line += '\n';
        for(std::string_view rest = _line ; !rest.empty() ; )
        {
#ifdef WIN32
            const int written = ::_write(output, rest.data(), static_cast<unsigned int>(rest.size()));
#else
            const auto written = ::write(output, rest.data(), rest.size());
#endif
            if(written <= 0)
            {
                return;
            }

            rest.remove_prefix(static_cast<std::size_t>(written));
        }
    }

    /// The response to a request; no exception may end the server.
    std::optional<std::string> answer(std::string_view _request)
    {
        try
        {
            return server.answer(_request);
        }
        catch(const std::exception& e)
        {
            return error(nullptr, -32603, e.what()).dump();
        }
    }

    /// Answers each line of the client, until it closes its input, which ends the application.
    void serve()
    {
        // As a GUI test sees it: the main window on the screen, at the size of the tests. Without one after the
        // deadline of the tests, the application is served as it is.
        try
        {
            ui->prepare_window(detail::wait_for_main_window(stopping));
        }
        catch(const std::exception& e)
        {
            std::cerr << "[sight] MCP: " << e.what() << std::endl;
        }

        std::string line;
        bool skipping = false;
        std::array<char, 4096> buffer {};
        while(!stopping)
        {
            if(!input_ready())
            {
                continue;
            }

#ifdef WIN32
            const int count = ::_read(::_fileno(stdin), buffer.data(), static_cast<unsigned int>(buffer.size()));
#else
            const auto count = ::read(STDIN_FILENO, buffer.data(), buffer.size());
#endif
            if(count <= 0)
            {
                break;
            }

            line.append(buffer.data(), static_cast<std::size_t>(count));
            std::size_t start = 0;
            for(auto end = line.find('\n') ; end != std::string::npos ; end = line.find('\n', start))
            {
                const auto request = std::string_view(line).substr(start, end - start);
                if(skipping)
                {
                    // The end of a request refused already.
                    skipping = false;
                }
                else if(request.size() > MAX_REQUEST_SIZE)
                {
                    write(too_long());
                }
                else if(const auto response = answer(request); response)
                {
                    write(*response);
                }

                start = end + 1;
            }

            line.erase(0, start);

            // A request without its end yet: past the limit, it is refused once, then dropped up to its end.
            if(skipping)
            {
                line.clear();
            }
            else if(line.size() > MAX_REQUEST_SIZE)
            {
                write(too_long());
                line.clear();
                skipping = true;
            }
        }

        if(!stopping)
        {
            QMetaObject::invokeMethod(qApp, []{QCoreApplication::exit(0);}, Qt::QueuedConnection);
        }
    }

    std::unique_ptr<scenario> ui;
    mcp_server server;
    int output {reserved_output()};
    std::atomic_bool stopping {false};
    std::atomic_bool served {false};
    std::thread reader;
};

//------------------------------------------------------------------------------

void mcp_stdio::prepare()
{
    if(s_output.has_value())
    {
        return;
    }

    // What the agent sees is what a test sees, and its clicks stay out of the settings of the user. The file and color
    // dialogs are Qt's, which the dialog verbs drive.
    ui::preferences::ignore_filesystem(true);
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    static_cast<void>(std::fflush(stdout));
#ifdef WIN32
    const int protocol = ::_dup(::_fileno(stdout));
    ::_dup2(::_fileno(stderr), ::_fileno(stdout));
    ::_setmode(protocol, _O_BINARY);
    ::_setmode(::_fileno(stdin), _O_BINARY);
#else
    const int protocol = ::fcntl(STDOUT_FILENO, F_DUPFD_CLOEXEC, 0);
    ::dup2(STDERR_FILENO, STDOUT_FILENO);
#endif
    if(protocol < 0)
    {
        throw std::runtime_error("could not reserve the standard output for MCP");
    }

    s_output = protocol;
}

//------------------------------------------------------------------------------

mcp_stdio::mcp_stdio(QApplication& _application) :
    m_implementation(std::make_unique<implementation>(_application))
{
}

//------------------------------------------------------------------------------

mcp_stdio::~mcp_stdio() = default;

} // namespace sight::ui::test
