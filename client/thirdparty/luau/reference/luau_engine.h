#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <functional>

// ============================================================
// Luau VM Integration
// ============================================================
// Requires Luau compiled from github.com/luau-lang/luau
// Headers: luau/VM/include/ (lua.h, lualib.h, luaconf.h)
//          luau/Compiler/include/Luau/ (luacode.h)
// Libraries: libluau.a, libluacode.a
// ============================================================

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

class LuauEngine : public QObject {
    Q_OBJECT
public:
    explicit LuauEngine(QObject* parent = nullptr);
    ~LuauEngine() override;

    bool initialize();
    void executeCode(const QString& code);
    void executeBytecode(const QByteArray& bytecode);
    QByteArray compileCode(const QString& code);
    bool loadBytecode(const QByteArray& bytecode);
    void stop();
    bool isRunning() const;

    void setOutputCallback(std::function<void(const QString&)> callback);
    void setAPICallback(std::function<void(const QString&, const QJsonArray&)> callback);

    // Call a widget callback stored in the Luau registry by widget ID
    void callWidgetCallback(int widgetId);

    // Call a named Luau function with no arguments (e.g. "onTick", "onStart")
    void callLuauFunction(const QString& funcName);

    // Call onChange callback for a slider/textInput widget
    void callWidgetChangeCallback(int widgetId, const QString& value);

    // Call onSelect callback for a list widget
    void callWidgetSelectCallback(int widgetId, int index);

    // Get the raw lua_State (for qt_api registration)
    lua_State* luaState() const { return m_L; }

    void handleAPICall(const QString& func, const QJsonArray& args);

signals:
    void outputReceived(const QString& text);
    void executionFinished(int exitCode);
    void errorOccurred(const QString& error);

private:
    lua_State* m_L;
    bool m_running;
    bool m_initialized;
    int m_instructionLimit;
    std::function<void(const QString&)> m_outputCallback;
    std::function<void(const QString&, const QJsonArray&)> m_apiCallback;

    static int luaPrint(lua_State* L);

    friend void qt_register_native(lua_State* L, int w, int h);

    void createSandbox();
    void registerAPIFunctions();
    void reportError(const QString& error);
    LuauEngine* self() { return this; }

    static LuauEngine* fromState(lua_State* L);

    // Luau wrapper that provides the app API (createButton, render, etc.)
    static const char* appAPIWrapper();
};
