// =============================================================
// ARX ENGINE - Third Party Master Include
// Centraliza todos los includes de librerias third_party
// NO editar la carpeta third_Party - este archivo las referencia
// NOTA: Ya no se usa MiniAudio, todo el audio via Raylib
// =============================================================
#ifndef ARX_THIRD_PARTY_HPP
#define ARX_THIRD_PARTY_HPP

// --- RAYLIB (Rendering, Window, Input, Audio) ---
#include "raylib.h"

// --- GLM (Matematicas 3D) ---
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"

// --- ASSIMP (Carga de modelos 3D) ---
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

// --- JOLT PHYSICS (Fisica) ---
#include "Jolt/Jolt.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseSystem.h"
#include "Jolt/Physics/Collision/NarrowPhase/NarrowPhaseSystem.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/PlaneShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/HeightFieldShape.h"
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"
#include "Jolt/Physics/Collision/Shape/MutableCompoundShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Constraints/HingeConstraint.h"
#include "Jolt/Physics/Constraints/SliderConstraint.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/Constraints/SixDOFConstraint.h"
#include "Jolt/Physics/Constraints/DistanceConstraint.h"
#include "Jolt/Physics/Constraints/SpringSettings.h"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"

// --- IMGUI (UI inmediata) ---
#include "imgui.h"

// --- ZLIB (Compresion, incluida en raylib) ---
#include <zlib.h>

#endif // ARX_THIRD_PARTY_HPP
