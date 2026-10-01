#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class LuauEngine;

namespace QtAPI {
// Register native C functions that provide system info to Luau apps.
// These complement the Luau-side widget creators (createButton, etc.)
// by providing access to Qt system capabilities.
void registerQtAPI(LuauEngine* engine, int screenWidth = 800, int screenHeight = 600);
}
