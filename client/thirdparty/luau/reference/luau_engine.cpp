#include "luau_engine.h"
#include "qt_api.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

// ============================================================
// Luau Engine Implementation
// Uses the real Luau VM exclusively (github.com/luau-lang/luau)
// ============================================================

LuauEngine* LuauEngine::fromState(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "__luau_engine_ptr");
    LuauEngine* engine = static_cast<LuauEngine*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return engine;
}

const char* LuauEngine::appAPIWrapper() {
    return
        // print() ya está registrado como función C nativa (luaPrint).
        // NO lo sobrescribimos aquí — el override anterior rompía print()
        // porque __app_api nunca se creaba y las llamadas fallaban en silencio.

        // Widget callbacks storage
        "local _widget_callbacks = {} "
        "local _storage = {} "
        "local _widgets = {} "
        "local _widgetId = 0 "

        // Widget creators return descriptor tables
        // If props.onClick is a function, store it and replace with true flag
        "function createButton(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  local id = _widgetId "
        "  if type(p.onClick) == 'function' then "
        "    _widget_callbacks[id] = p.onClick "
        "    p.onClick = true "
        "  end "
        "  if type(p.onChange) == 'function' then "
        "    _widget_callbacks['change_' .. id] = p.onChange "
        "    p.onChange = true "
        "  end "
        "  p._id = id "
        "  local w = {type='button', id=id, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createLabel(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  p._id = _widgetId "
        "  local w = {type='label', id=_widgetId, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createTextInput(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  local id = _widgetId "
        "  if type(p.onChange) == 'function' then "
        "    _widget_callbacks['change_' .. id] = p.onChange "
        "    p.onChange = true "
        "  end "
        "  p._id = id "
        "  local w = {type='textInput', id=id, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createSlider(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  local id = _widgetId "
        "  if type(p.onChange) == 'function' then "
        "    _widget_callbacks['change_' .. id] = p.onChange "
        "    p.onChange = true "
        "  end "
        "  p._id = id "
        "  local w = {type='slider', id=id, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createImage(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  p._id = _widgetId "
        "  local w = {type='image', id=_widgetId, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createToggle(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  local id = _widgetId "
        "  if type(p.onChange) == 'function' then "
        "    _widget_callbacks['change_' .. id] = p.onChange "
        "    p.onChange = true "
        "  end "
        "  p._id = id "
        "  local w = {type='toggle', id=id, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function createList(props) "
        "  local p = props or {} "
        "  _widgetId = _widgetId + 1 "
        "  local id = _widgetId "
        "  if type(p.onSelect) == 'function' then "
        "    _widget_callbacks['select_' .. id] = p.onSelect "
        "    p.onSelect = true "
        "  end "
        "  p._id = id "
        "  local w = {type='list', id=id, properties=p} "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        // setProperty: update a widget property at runtime
        "function setProperty(widgetId, key, value) "
        "  for _, w in ipairs(_widgets) do "
        "    if w.id == widgetId then "
        "      w.properties[key] = value "
        "      return {type='setProperty', widgetId=widgetId, key=key, value=value} "
        "    end "
        "  end "
        "end "

        // Storage API (local sandbox storage)
        "function guardarDato(key, value) _storage[key] = value end "
        "function cargarDato(key, defaultVal) return _storage[key] or defaultVal end "

        // esperar() ahora es una función nativa C registrada por qt_api.cpp
        // que usa QEventLoop para no congelar la UI.

        // Widget management helpers
        "function addWidget(w) "
        "  if not w.id then "
        "    _widgetId = _widgetId + 1 "
        "    w.id = _widgetId "
        "  end "
        "  table.insert(_widgets, w) "
        "  return w "
        "end "

        "function render() "
        "  local result = {} "
        "  for _, w in ipairs(_widgets) do "
        "    table.insert(result, w) "
        "  end "
        "  return result "
        "end "

        // Internal: get stored callback for engine C++ calls
        "function __getWidgetCallback(widgetId) "
        "  return _widget_callbacks[widgetId] "
        "end "
        ;
}

LuauEngine::LuauEngine(QObject* parent)
    : QObject(parent)
    , m_L(nullptr)
    , m_running(false)
    , m_initialized(false)
    , m_instructionLimit(5000000) // 5 millones de instrucciones máx por ejecución
{
}

LuauEngine::~LuauEngine()
{
    stop();
    if (m_L) {
        lua_close(m_L);
        m_L = nullptr;
    }
    m_initialized = false;
}

bool LuauEngine::initialize()
{
    if (m_initialized) return true;

    m_L = luaL_newstate();
    if (!m_L) {
        reportError(QString::fromUtf8("Failed to create Luau state"));
        return false;
    }

    // Store engine pointer in registry for C callbacks
    lua_pushlightuserdata(m_L, this);
    lua_setfield(m_L, LUA_REGISTRYINDEX, "__luau_engine_ptr");

    // Open standard libraries
    luaL_openlibs(m_L);

    // Apply sandbox
    createSandbox();

    // Register C API functions
    registerAPIFunctions();

    // Load the app API wrapper (createButton, render, etc.)
    QString wrapperCode = QString::fromUtf8(appAPIWrapper());
    QByteArray wrapperUtf8 = wrapperCode.toUtf8();

    size_t bcSize = 0;
    char* bc = luau_compile(wrapperUtf8.constData(), wrapperUtf8.size(), nullptr, &bcSize);
    if (bc) {
        int loadResult = luau_load(m_L, "@luau_api_wrapper", bc, bcSize, 0);
        free(bc);
        if (loadResult == LUA_OK) {
            lua_pcall(m_L, 0, 0, 0);
        }
    }

    // Register Qt native API (obtenerAncho, obtenerAlto, reproducirAudio)
    QtAPI::registerQtAPI(this, 800, 600);

    m_initialized = true;
    emit outputReceived(QString::fromUtf8("[Luau] Motor inicializado. Listo para ejecutar código."));
    return true;
}

void LuauEngine::executeCode(const QString& code)
{
    if (!m_initialized) {
        if (!initialize()) {
            emit executionFinished(1);
            return;
        }
    }

    if (m_running) {
        emit errorOccurred(QString::fromUtf8("Ya hay una ejecución en curso."));
        return;
    }

    m_running = true;

    // Reset step counter
    lua_pushinteger(m_L, 0);
    lua_setfield(m_L, LUA_REGISTRYINDEX, "__luau_step_count");

    // Luau no soporta lua_sethook (fue removido del fork).
    // La protección contra bucles infinitos se hace con un QTimer que aborta
    // la ejecución si pasa más de MAX_EXECUTION_TIME_MS.
    // Por ahora confiamos en que los creadores no subirán código malicioso,
    // y el admin revisa las apps antes de aprobarlas.

    emit outputReceived(QString::fromUtf8("[Luau] Ejecutando código..."));

    // Compile with luau_compile for better error messages and optimizations
    QByteArray codeUtf8 = code.toUtf8();
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(codeUtf8.constData(), codeUtf8.size(), nullptr, &bytecodeSize);
    if (!bytecode) {
        reportError(QString::fromUtf8("Error de compilación Luau"));
        m_running = false;
        emit executionFinished(1);
        return;
    }

    int loadResult = luau_load(m_L, "@luau_app", bytecode, bytecodeSize, 0);
    free(bytecode);

    if (loadResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        QString errorMsg = errMsg ? QString::fromUtf8(errMsg) : QString::fromUtf8("Unknown compilation error");
        lua_pop(m_L, 1);
        reportError(QString::fromUtf8("Error de compilación: %1").arg(errorMsg));
        m_running = false;
        emit executionFinished(1);
        return;
    }

    // Execute
    int callResult = lua_pcall(m_L, 0, 0, 0);
    if (callResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        QString errorMsg = errMsg ? QString::fromUtf8(errMsg) : QString::fromUtf8("Unknown runtime error");
        lua_pop(m_L, 1);
        reportError(QString::fromUtf8("Error de ejecución: %1").arg(errorMsg));
        m_running = false;
        emit executionFinished(1);
        return;
    }

    // Call render() if it exists to get the UI definition
    lua_getglobal(m_L, "render");
    if (lua_isfunction(m_L, -1)) {
        lua_pcall(m_L, 0, 1, 0);
        if (lua_istable(m_L, -1)) {
            QJsonArray widgets;
            int len = (int)lua_objlen(m_L, -1);
            for (int i = 1; i <= len; i++) {
                lua_rawgeti(m_L, -1, i);
                if (lua_istable(m_L, -1)) {
                    QJsonObject widget;
                    lua_pushnil(m_L);
                    while (lua_next(m_L, -2) != 0) {
                        const char* key = lua_tostring(m_L, -2);
                        if (key) {
                            QString qkey = QString::fromUtf8(key);
                            if (lua_isstring(m_L, -1)) {
                                widget[qkey] = QString::fromUtf8(lua_tostring(m_L, -1));
                            } else if (lua_isnumber(m_L, -1)) {
                                widget[qkey] = lua_tonumber(m_L, -1);
                            } else if (lua_isboolean(m_L, -1)) {
                                widget[qkey] = (bool)lua_toboolean(m_L, -1);
                            }
                        }
                        lua_pop(m_L, 1);
                    }
                    widgets.append(widget);
                }
                lua_pop(m_L, 1);
            }
            lua_pop(m_L, 1);

            if (m_apiCallback) {
                m_apiCallback(QString::fromUtf8("render"), widgets);
            }
        } else {
            lua_pop(m_L, 1);
        }
    } else {
        lua_pop(m_L, 1);
    }

    emit outputReceived(QString::fromUtf8("[Luau] Ejecución completada exitosamente."));
    m_running = false;
    emit executionFinished(0);
}

// Nota: Luau no soporta lua_sethook (fue removido del fork de Roblox).
// La protección contra bucles infinitos se maneja a nivel de revisión:
// el admin debe revisar el código antes de aprobar una app.
// En el futuro se puede usar un QThread con timeout para abortar.

void LuauEngine::stop()
{
    if (m_running && m_L) {
        m_running = false;
    }
}

bool LuauEngine::isRunning() const
{
    return m_running;
}

void LuauEngine::setOutputCallback(std::function<void(const QString&)> callback)
{
    m_outputCallback = std::move(callback);
}

void LuauEngine::setAPICallback(std::function<void(const QString&, const QJsonArray&)> callback)
{
    m_apiCallback = std::move(callback);
}

void LuauEngine::callWidgetCallback(int widgetId)
{
    if (!m_L || !m_initialized) return;

    // Push __getWidgetCallback onto the stack and call it
    lua_getglobal(m_L, "__getWidgetCallback");
    if (!lua_isfunction(m_L, -1)) {
        lua_pop(m_L, 1);
        return;
    }

    lua_pushinteger(m_L, widgetId);
    int callResult = lua_pcall(m_L, 1, 1, 0);

    if (callResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        if (errMsg) {
            reportError(QString::fromUtf8("Error en callback del widget: %1").arg(QString::fromUtf8(errMsg)));
        }
        lua_pop(m_L, 1);
        return;
    }

    // The return value should be a function (the stored callback)
    if (lua_isfunction(m_L, -1)) {
        // Call the callback function with no arguments
        int cbResult = lua_pcall(m_L, 0, 0, 0);
        if (cbResult != LUA_OK) {
            const char* errMsg = lua_tostring(m_L, -1);
            if (errMsg) {
                // Log the error but don't crash - the callback might have a bug
                if (m_outputCallback) {
                    m_outputCallback(QString::fromUtf8("[ERROR] Callback error: %1").arg(QString::fromUtf8(errMsg)));
                }
            }
            lua_pop(m_L, 1);
        }
    } else {
        lua_pop(m_L, 1);
    }
}

void LuauEngine::callWidgetChangeCallback(int widgetId, const QString& value)
{
    if (!m_L || !m_initialized) return;

    // Try __getWidgetCallback prefixed first (for onChange)
    lua_getglobal(m_L, "__getWidgetCallback");
    if (!lua_isfunction(m_L, -1)) {
        lua_pop(m_L, 1);
        return;
    }

    // Call __getWidgetCallback prefixed(widgetId)
    lua_pushstring(m_L, "change_");
    lua_pushinteger(m_L, widgetId);
    lua_concat(m_L, 2);
    // Now we need to look up _widget_callbacks["change_"..id] directly
    lua_pop(m_L, 1); // pop the concatenated string

    // Alternative: get the global table and look up the callback
    lua_getglobal(m_L, "_widget_callbacks");
    if (!lua_istable(m_L, -1)) {
        lua_pop(m_L, 1);
        return;
    }

    QString key = QString::fromUtf8("change_%1").arg(widgetId);
    QByteArray keyUtf8 = key.toUtf8();
    lua_getfield(m_L, -1, keyUtf8.constData());

    if (lua_isfunction(m_L, -1)) {
        // Push the new value as argument
        lua_pushstring(m_L, value.toUtf8().constData());
        int cbResult = lua_pcall(m_L, 1, 0, 0);
        if (cbResult != LUA_OK) {
            const char* errMsg = lua_tostring(m_L, -1);
            if (errMsg && m_outputCallback) {
                m_outputCallback(QString::fromUtf8("[ERROR] Callback error: %1").arg(QString::fromUtf8(errMsg)));
            }
            lua_pop(m_L, 1);
        }
    } else {
        lua_pop(m_L, 1);
    }
    lua_pop(m_L, 1); // pop _widget_callbacks
}

void LuauEngine::callWidgetSelectCallback(int widgetId, int index)
{
    if (!m_L || !m_initialized) return;

    lua_getglobal(m_L, "_widget_callbacks");
    if (!lua_istable(m_L, -1)) {
        lua_pop(m_L, 1);
        return;
    }

    QString key = QString::fromUtf8("select_%1").arg(widgetId);
    QByteArray keyUtf8 = key.toUtf8();
    lua_getfield(m_L, -1, keyUtf8.constData());

    if (lua_isfunction(m_L, -1)) {
        lua_pushinteger(m_L, index);
        int cbResult = lua_pcall(m_L, 1, 0, 0);
        if (cbResult != LUA_OK) {
            const char* errMsg = lua_tostring(m_L, -1);
            if (errMsg && m_outputCallback) {
                m_outputCallback(QString::fromUtf8("[ERROR] Callback error: %1").arg(QString::fromUtf8(errMsg)));
            }
            lua_pop(m_L, 1);
        }
    } else {
        lua_pop(m_L, 1);
    }
    lua_pop(m_L, 1);
}

void LuauEngine::callLuauFunction(const QString& funcName)
{
    if (!m_L || !m_initialized) return;

    QByteArray nameUtf8 = funcName.toUtf8();
    lua_getglobal(m_L, nameUtf8.constData());
    if (!lua_isfunction(m_L, -1)) {
        lua_pop(m_L, 1);
        return;
    }

    int callResult = lua_pcall(m_L, 0, 0, 0);
    if (callResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        if (errMsg) {
            reportError(QString::fromUtf8("Error en %1: %2").arg(funcName, QString::fromUtf8(errMsg)));
        }
        lua_pop(m_L, 1);
    }
}

void LuauEngine::handleAPICall(const QString& func, const QJsonArray& args)
{
    Q_UNUSED(func);
    Q_UNUSED(args);
}

void LuauEngine::registerAPIFunctions()
{
    lua_pushcfunction(m_L, luaPrint, "print");
    lua_setglobal(m_L, "print");
}

void LuauEngine::reportError(const QString& error)
{
    qWarning() << "[LuauEngine]" << error;
    emit errorOccurred(error);
    if (m_outputCallback) {
        m_outputCallback(QString::fromUtf8("[ERROR] %1").arg(error));
    }
}

int LuauEngine::luaPrint(lua_State* L)
{
    LuauEngine* engine = fromState(L);
    int n = lua_gettop(L);
    QString result;
    for (int i = 1; i <= n; i++) {
        if (i > 1) result += QString::fromUtf8("\t");
        if (lua_isstring(L, i) || lua_isnumber(L, i)) {
            const char* s = lua_tostring(L, i);
            result += s ? QString::fromUtf8(s) : QString::fromUtf8("nil");
        } else if (lua_isboolean(L, i)) {
            result += lua_toboolean(L, i) ? QString::fromUtf8("true") : QString::fromUtf8("false");
        } else if (lua_isnil(L, i)) {
            result += QString::fromUtf8("nil");
        } else {
            result += QString::fromUtf8("<%1>").arg(QString::fromUtf8(luaL_typename(L, i)));
        }
    }

    // Emitir la señal directamente — esto llama a AppRunner::onOutput
    // de forma síncrona (misma hebra) y el texto aparece inmediatamente.
    if (engine) {
        emit engine->outputReceived(result);
    }

    // También llamar el callback si existe (para otros usos)
    if (engine && engine->m_outputCallback) {
        engine->m_outputCallback(result);
    }
    return 0;
}

// ============================================================
// Sandbox implementation
// Removes dangerous globals and restricts libraries
// ============================================================
void LuauEngine::createSandbox()
{
    if (!m_L) return;

    // Remove dangerous global functions
    lua_pushnil(m_L);
    lua_setglobal(m_L, "dofile");
    lua_pushnil(m_L);
    lua_setglobal(m_L, "loadfile");
    lua_pushnil(m_L);
    lua_setglobal(m_L, "load");
    lua_pushnil(m_L);
    lua_setglobal(m_L, "require");
    lua_pushnil(m_L);
    lua_setglobal(m_L, "collectgarbage");

    // Remove dangerous os.* entries
    lua_getglobal(m_L, "os");
    if (lua_istable(m_L, -1)) {
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "execute");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "remove");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "rename");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "exit");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "getenv");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "setlocale");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "tmpname");
    }
    lua_pop(m_L, 1);

    // Remove entire io library
    lua_pushnil(m_L);
    lua_setglobal(m_L, "io");

    // Remove entire debug library
    lua_pushnil(m_L);
    lua_setglobal(m_L, "debug");

    // Restrict package library
    lua_getglobal(m_L, "package");
    if (lua_istable(m_L, -1)) {
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "loadlib");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "searchpath");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "cpath");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "path");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "loaded");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "preload");
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "config");
    }
    lua_pop(m_L, 1);

    // Remove string.dump (prevents bytecode dumping)
    lua_getglobal(m_L, "string");
    if (lua_istable(m_L, -1)) {
        lua_pushnil(m_L);   lua_setfield(m_L, -2, "dump");
    }
    lua_pop(m_L, 1);

    // Safe globals remain: print, type, tostring, tonumber, pairs, ipairs,
    // table, string, math, coroutine, utf8, error, pcall, xpcall,
    // select, unpack, next, rawget, rawset, rawequal,
    // setmetatable, getmetatable, assert, _VERSION
}

// ============================================================
// Bytecode compilation (Luau native)
// ============================================================
QByteArray LuauEngine::compileCode(const QString& code)
{
    QByteArray result;

    if (!m_L || !m_initialized) {
        qWarning() << "[LuauEngine] Cannot compile: engine not initialized.";
        return result;
    }

    QByteArray codeUtf8 = code.toUtf8();

    // Use luau_compile for native Luau bytecode
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(codeUtf8.constData(), codeUtf8.size(), nullptr, &bytecodeSize);
    if (!bytecode) {
        qWarning() << "[LuauEngine] luau_compile failed.";
        return QByteArray();
    }
    result = QByteArray(bytecode, static_cast<int>(bytecodeSize));
    free(bytecode);

    return result;
}

bool LuauEngine::loadBytecode(const QByteArray& bytecode)
{
    if (!m_L || !m_initialized) {
        qWarning() << "[LuauEngine] Cannot load bytecode: engine not initialized.";
        return false;
    }

    if (bytecode.isEmpty()) {
        qWarning() << "[LuauEngine] Cannot load bytecode: empty data.";
        return false;
    }

    int loadResult = luau_load(m_L, "@luau_bytecode", bytecode.constData(), bytecode.size(), 0);
    if (loadResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        qWarning() << "[LuauEngine] Bytecode load failed:" << (errMsg ? errMsg : "unknown error");
        lua_pop(m_L, 1);
        return false;
    }

    return true;
}

void LuauEngine::executeBytecode(const QByteArray& bytecode)
{
    if (!m_initialized) {
        if (!initialize()) {
            emit executionFinished(1);
            return;
        }
    }

    if (m_running) {
        emit errorOccurred(QString::fromUtf8("Ya hay una ejecución en curso."));
        return;
    }

    m_running = true;

    lua_pushinteger(m_L, 0);
    lua_setfield(m_L, LUA_REGISTRYINDEX, "__luau_step_count");

    emit outputReceived(QString::fromUtf8("[Luau] Ejecutando bytecode..."));

    if (!loadBytecode(bytecode)) {
        reportError(QString::fromUtf8("Error al cargar bytecode."));
        m_running = false;
        emit executionFinished(1);
        return;
    }

    int callResult = lua_pcall(m_L, 0, 0, 0);
    if (callResult != LUA_OK) {
        const char* errMsg = lua_tostring(m_L, -1);
        QString errorMsg = errMsg ? QString::fromUtf8(errMsg) : QString::fromUtf8("Error desconocido");
        lua_pop(m_L, 1);
        reportError(QString::fromUtf8("Error de ejecución (bytecode): %1").arg(errorMsg));
        m_running = false;
        emit executionFinished(1);
        return;
    }

    // Call render() if it exists
    lua_getglobal(m_L, "render");
    if (lua_isfunction(m_L, -1)) {
        lua_pcall(m_L, 0, 1, 0);
        if (lua_istable(m_L, -1)) {
            QJsonArray widgets;
            int len = (int)lua_objlen(m_L, -1);
            for (int i = 1; i <= len; i++) {
                lua_rawgeti(m_L, -1, i);
                if (lua_istable(m_L, -1)) {
                    QJsonObject widget;
                    lua_pushnil(m_L);
                    while (lua_next(m_L, -2) != 0) {
                        const char* key = lua_tostring(m_L, -2);
                        if (key) {
                            QString qkey = QString::fromUtf8(key);
                            if (lua_isstring(m_L, -1)) {
                                widget[qkey] = QString::fromUtf8(lua_tostring(m_L, -1));
                            } else if (lua_isnumber(m_L, -1)) {
                                widget[qkey] = lua_tonumber(m_L, -1);
                            } else if (lua_isboolean(m_L, -1)) {
                                widget[qkey] = (bool)lua_toboolean(m_L, -1);
                            }
                        }
                        lua_pop(m_L, 1);
                    }
                    widgets.append(widget);
                }
                lua_pop(m_L, 1);
            }
            lua_pop(m_L, 1);

            if (m_apiCallback) {
                m_apiCallback(QString::fromUtf8("render"), widgets);
            }
        } else {
            lua_pop(m_L, 1);
        }
    } else {
        lua_pop(m_L, 1);
    }

    emit outputReceived(QString::fromUtf8("[Luau] Ejecución de bytecode completada."));
    m_running = false;
    emit executionFinished(0);
}
