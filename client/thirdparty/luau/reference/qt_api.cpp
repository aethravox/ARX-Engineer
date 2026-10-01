#include "qt_api.h"
#include "luau_engine.h"
#include <QDebug>
#include <QUrl>
#include <QTimer>
#include <QEventLoop>

#ifdef HAVE_QT_MULTIMEDIA
#include <QMediaPlayer>
#include <QAudioOutput>
#endif

static int qt_obtenerAncho(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "__qt_screen_width");
    int w = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_pushinteger(L, w);
    return 1;
}

static int qt_obtenerAlto(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "__qt_screen_height");
    int h = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_pushinteger(L, h);
    return 1;
}

// reproducirAudio(url): reproduce un archivo de audio desde una URL o ruta local.
// Requiere Qt Multimedia instalado (sudo apt install qt5-multimedia-dev).
// Si no está disponible, la función no hace nada pero no falla.
static int qt_reproducirAudio(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    if (!url) return 0;

    QString audioUrl = QString::fromUtf8(url);
    qDebug() << "[QtAPI] Reproduciendo audio:" << audioUrl;

#ifdef HAVE_QT_MULTIMEDIA
    QMediaPlayer* player = new QMediaPlayer();
    QAudioOutput* audioOutput = new QAudioOutput();
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl(audioUrl));
    player->play();

    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, [player, audioOutput](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia) {
            player->deleteLater();
            audioOutput->deleteLater();
        }
    });
#else
    qDebug() << "[QtAPI] Audio no disponible - instala qt5-multimedia-dev";
#endif

    return 0;
}

// esperar(segundos): bloquea la ejecución Luau por N segundos.
// Usa QEventLoop para no congelar la interfaz Qt mientras espera.
static int qt_esperar(lua_State* L) {
    double segundos = luaL_checknumber(L, 1);
    if (segundos <= 0) return 0;

    int ms = static_cast<int>(segundos * 1000);
    qDebug() << "[QtAPI] Esperando" << ms << "ms";

    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();

    return 0;
}

// Friend function declared in LuauEngine - has access to private members
void qt_register_native(lua_State* L, int screenWidth, int screenHeight) {
    if (!L) return;

    lua_pushinteger(L, screenWidth);
    lua_setfield(L, LUA_REGISTRYINDEX, "__qt_screen_width");
    lua_pushinteger(L, screenHeight);
    lua_setfield(L, LUA_REGISTRYINDEX, "__qt_screen_height");

    lua_pushcfunction(L, qt_obtenerAncho, "obtenerAncho");
    lua_setglobal(L, "obtenerAncho");
    lua_pushcfunction(L, qt_obtenerAlto, "obtenerAlto");
    lua_setglobal(L, "obtenerAlto");
    lua_pushcfunction(L, qt_reproducirAudio, "reproducirAudio");
    lua_setglobal(L, "reproducirAudio");
    lua_pushcfunction(L, qt_esperar, "esperar");
    lua_setglobal(L, "esperar");

    qDebug() << "[QtAPI] Funciones nativas registradas: obtenerAncho, obtenerAlto, reproducirAudio, esperar";
}

namespace QtAPI {

void registerQtAPI(LuauEngine* engine, int screenWidth, int screenHeight)
{
    if (!engine) return;
    qt_register_native(engine->luaState(), screenWidth, screenHeight);
}

}
