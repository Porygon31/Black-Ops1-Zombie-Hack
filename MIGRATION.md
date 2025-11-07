# Guide de Migration - Ancien Code vers Nouveau Code

Ce document explique comment migrer de l'ancien code vers la nouvelle architecture moderne.

## Comparaison Rapide

### Ancien Code

```cpp
// Main.cpp - Ancien style
#include <Windows.h>
#include "Drawing.h"
#include "Offsets.h"

// Variables globales
HWND hwndBlackops1 = nullptr;
HDC hdcBlackops1 = nullptr;
HBRUSH Brush = nullptr;
float Matrix[16];

DWORD WINAPI mainThread(LPVOID lpParam) {
    hwndBlackops1 = FindWindow(0, "Call of Duty®: BlackOps");

    while (true) {
        hdcBlackops1 = GetDC(hwndBlackops1);

        memcpy(&Matrix, (PBYTE*)(viewMatrix), sizeof(Matrix));

        for (short int i = 0; i < 32; i++) {
            DWORD entity = *(DWORD*)(entityList + i * 0x8C);

            if (entity == NULL) continue;

            float enemyX = *(float*)(entity + 0x18);
            float enemyY = *(float*)(entity + 0x1C);
            float enemyZ = *(float*)(entity + 0x20);

            Vec3 entityLocation = {enemyX, enemyY, enemyZ};
            int health = *(int*)(entity + healthOffset);

            if (health < 1) continue;

            Vec2 vScreen;
            if (WorldToScreen(entityLocation, vScreen, Matrix, 1024, 768)) {
                Brush = CreateSolidBrush(RGB(158, 66, 244));
                DrawBorderBox(vScreen.x, vScreen.y, 50, 100, 1);
                DeleteObject(Brush);
            }
        }

        Sleep(1);
        DeleteObject(hdcBlackops1);
    }
    return 0;
}
```

### Nouveau Code

```cpp
// src/main.cpp - Style moderne
#include <bo1_esp/bo1_esp.hpp>

DWORD WINAPI mainThread(LPVOID) {
    // Configuration type-safe
    bo1::core::ESPConfig config;
    config.display.showBoxes = true;
    config.display.showHealth = true;
    config.display.boxColor = RGB(158, 66, 244);

    // Pas de variables globales - tout encapsulé
    bo1::core::ESPManager esp(config);

    // Gestion d'erreurs explicite
    if (!esp.initialize()) {
        return 1;
    }

    // Boucle propre avec RAII
    while (esp.isRunning()) {
        esp.update();  // Tout est géré en interne
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (GetAsyncKeyState(VK_END) & 1) {
            esp.stop();
        }
    }

    return 0;
}
// Pas de fuites de ressources - RAII s'en occupe automatiquement
```

## Changements Principaux

### 1. Variables Globales → Encapsulation

#### Avant
```cpp
// Variables globales partout
extern HWND hwndBlackops1;
extern HDC hdcBlackops1;
extern HBRUSH Brush;
extern float Matrix[16];
```

#### Après
```cpp
// Tout encapsulé dans des classes
class ESPManager {
private:
    HWND m_hwnd{};
    std::unique_ptr<Renderer> m_renderer;
    Matrix4x4 m_viewMatrix;
    ESPConfig m_config;
    // ...
};
```

### 2. Gestion Manuelle des Ressources → RAII

#### Avant
```cpp
HDC hdc = GetDC(hwnd);
// ... utilisation ...
DeleteObject(hdc);  // Risque d'oubli = fuite mémoire

HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
// ... utilisation ...
DeleteObject(brush);  // Doit être appelé manuellement
```

#### Après
```cpp
{
    DeviceContext dc(hwnd);
    // ... utilisation ...
}  // Libération automatique via le destructeur

{
    Brush brush(RGB(255, 0, 0));
    // ... utilisation ...
}  // Libération automatique
```

### 3. Pas de Vérification d'Erreurs → std::optional

#### Avant
```cpp
DWORD entity = *(DWORD*)(entityList + i * 0x8C);
if (entity == NULL) continue;  // Vérification manuelle

float enemyX = *(float*)(entity + 0x18);  // Peut crasher !
```

#### Après
```cpp
auto entity = Memory::read<uintptr_t>(entityList + i * 0x8C);
if (!entity.has_value() || entity.value() == 0) {
    continue;
}

auto position = entity.getPosition();  // Retourne std::optional<Vec3f>
if (!position.has_value()) {
    continue;
}
```

### 4. Structures C → Classes C++ Modernes

#### Avant
```cpp
struct Vec3 {
    float x, y, z;
};

// Pas d'opérateurs, pas de méthodes
Vec3 v1 = {1.0f, 2.0f, 3.0f};
// Impossible de faire : v1 + v2
```

#### Après
```cpp
template<typename T>
struct Vector3 {
    T x{}, y{}, z{};

    constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] T length() const noexcept {
        return std::sqrt(x * x + y * y + z * z);
    }
    // ... autres opérateurs et méthodes
};

Vec3f v1{1.0f, 2.0f, 3.0f};
Vec3f v2{4.0f, 5.0f, 6.0f};
Vec3f v3 = v1 + v2;  // Fonctionne !
float len = v1.length();  // Méthodes disponibles
```

### 5. Fonctions Globales → Méthodes de Classe

#### Avant
```cpp
// Drawing.cpp
void DrawBorderBox(int x, int y, int w, int h, int thickness) {
    // Utilise des variables globales
    DrawFilledRect(x, y, w, thickness);
    // ...
}

// Utilisation
DrawBorderBox(100, 100, 200, 300, 2);
```

#### Après
```cpp
class Renderer {
public:
    void drawBorder(int x, int y, int width, int height,
                    int thickness, COLORREF color) {
        // Utilise les membres de la classe, pas de globales
        Brush brush(color);
        // ...
    }
};

// Utilisation
renderer.drawBorder(100, 100, 200, 300, 2, RGB(255, 0, 0));
```

### 6. Magic Numbers → Constexpr

#### Avant
```cpp
DWORD entityList = 0x01BCE4D0;  // Variable mutable
DWORD healthOffset = 0x184;

for (short int i = 0; i < 32; i++) {  // Nombre magique
    DWORD entity = *(DWORD*)(entityList + i * 0x8C);  // 0x8C ?
}
```

#### Après
```cpp
struct Offsets {
    static constexpr uintptr_t ENTITY_LIST = 0x01BCE4D0;
    static constexpr uintptr_t ENTITY_SIZE = 0x8C;
    static constexpr int MAX_ENTITIES = 32;

    struct Entity {
        static constexpr uintptr_t HEALTH = 0x184;
    };
};

for (int i = 0; i < Offsets::MAX_ENTITIES; ++i) {
    uintptr_t address = Offsets::ENTITY_LIST + i * Offsets::ENTITY_SIZE;
}
```

### 7. Pas de Namespace → Namespaces Organisés

#### Avant
```cpp
// Tout dans le namespace global
void DrawString(...);
struct Vec3 { ... };
class Entity { ... };
```

#### Après
```cpp
namespace bo1 {
    namespace math {
        struct Vec3 { ... };
    }
    namespace rendering {
        class Renderer { ... };
    }
    namespace game {
        class Entity { ... };
    }
    namespace core {
        class ESPManager { ... };
    }
}

// Utilisation avec alias
using namespace bo1::math;
Vec3f position;
```

### 8. Configuration Hard-Codée → Struct de Configuration

#### Avant
```cpp
// Changements nécessitent recompilation
COLORREF TextCOLOR = RGB(255, 255, 255);
DrawString(100, 20, RGB(255, 0, 0), "Text");
```

#### Après
```cpp
// Configuration runtime
ESPConfig config;
config.display.textColor = RGB(255, 255, 255);
config.display.injectionMessageColor = RGB(255, 0, 0);

ESPManager esp(config);

// Modification à l'exécution
auto newConfig = esp.getConfig();
newConfig.display.textColor = RGB(0, 255, 0);
esp.setConfig(newConfig);
```

## Migration Étape par Étape

### Étape 1 : Remplacer les Vecteurs

```cpp
// Ancien
Vec3 pos = {x, y, z};

// Nouveau
bo1::math::Vec3f pos{x, y, z};
```

### Étape 2 : Remplacer la Lecture Mémoire

```cpp
// Ancien
DWORD entity = *(DWORD*)(address);
if (entity == NULL) return;

// Nouveau
auto entity = bo1::game::Memory::read<uintptr_t>(address);
if (!entity.has_value()) return;
```

### Étape 3 : Remplacer le Rendering

```cpp
// Ancien
HDC hdc = GetDC(hwnd);
HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
// ... dessiner ...
DeleteObject(brush);
DeleteObject(hdc);

// Nouveau
bo1::rendering::Renderer renderer(hwnd);
renderer.beginFrame();
renderer.drawBorder(x, y, w, h, 2, RGB(255, 0, 0));
renderer.endFrame();
// Pas de DeleteObject nécessaire !
```

### Étape 4 : Remplacer la Boucle Principale

```cpp
// Ancien
while (true) {
    hdcBlackops1 = GetDC(hwndBlackops1);

    // ... code de rendu ...

    Sleep(1);
    DeleteObject(hdcBlackops1);
}

// Nouveau
bo1::core::ESPManager esp;
esp.initialize();

while (esp.isRunning()) {
    esp.update();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
```

## Avantages de la Migration

| Aspect | Avant | Après | Gain |
|--------|-------|-------|------|
| Fuites mémoire | Possibles | Impossibles (RAII) | ✅ Fiabilité |
| Crashes | Fréquents (nullptr) | Rares (std::optional) | ✅ Stabilité |
| Maintenabilité | Difficile (globals) | Facile (encapsulation) | ✅ Maintenance |
| Réutilisabilité | Faible | Élevée (header-only) | ✅ Portabilité |
| Performance | Bonne | Meilleure (constexpr) | ✅ Optimisation |
| Type safety | Faible | Fort | ✅ Sécurité |
| Tests | Difficile | Facile (DI) | ✅ Qualité |

## Code Mapping Complet

| Ancien Fichier | Nouveau Fichier | Changement Principal |
|---------------|-----------------|---------------------|
| `Vectors.h` | `include/bo1_esp/math/vector.hpp` | Templates + opérateurs |
| `WorldToScreen.h/cpp` | `include/bo1_esp/math/matrix.hpp` | Méthode de classe |
| `Drawing.h/cpp` | `include/bo1_esp/rendering/renderer.hpp` | Classe + RAII |
| `Offsets.h` | `include/bo1_esp/game/offsets.hpp` | constexpr |
| `Main.cpp` | `include/bo1_esp/core/esp_manager.hpp` | Encapsulation complète |
| `dllmain.cpp` | `src/main.cpp` | Simplifié |

## Exemple Complet de Migration

### Fonction Avant

```cpp
// Ancien Main.cpp
DWORD WINAPI mainThread(LPVOID lpParam) {
    hwndBlackops1 = FindWindow(0, "Call of Duty®: BlackOps");

    while (true) {
        hdcBlackops1 = GetDC(hwndBlackops1);
        memcpy(&Matrix, (PBYTE*)(viewMatrix), sizeof(Matrix));

        for (short int i = 0; i < 32; i++) {
            DWORD entity = *(DWORD*)(entityList + i * 0x8C);
            if (entity == NULL) continue;

            float x = *(float*)(entity + 0x18);
            float y = *(float*)(entity + 0x1C);
            float z = *(float*)(entity + 0x20);
            int health = *(int*)(entity + 0x184);

            if (health < 1) continue;

            Vec3 pos = {x, y, z};
            Vec2 screen;

            if (WorldToScreen(pos, screen, Matrix, 1024, 768)) {
                Brush = CreateSolidBrush(RGB(158, 66, 244));
                DrawBorderBox(screen.x, screen.y, 50, 100, 1);
                DeleteObject(Brush);

                char text[64];
                sprintf_s(text, "HP: %d", health);
                DrawString(screen.x, screen.y, RGB(255, 255, 255), text);
            }
        }

        Sleep(1);
        DeleteObject(hdcBlackops1);
    }
    return 0;
}
```

### Fonction Après

```cpp
// Nouveau src/main.cpp
DWORD WINAPI mainThread(LPVOID) {
    using namespace bo1;

    // Configuration
    core::ESPConfig config;
    config.display.showBoxes = true;
    config.display.showHealth = true;
    config.display.boxColor = RGB(158, 66, 244);
    config.display.textColor = RGB(255, 255, 255);

    // Initialisation
    core::ESPManager esp(config);
    if (!esp.initialize()) {
        return 1;
    }

    // Boucle principale
    while (esp.isRunning()) {
        esp.update();  // Toute la logique est dans ESPManager
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (GetAsyncKeyState(VK_END) & 1) {
            esp.stop();
        }
    }

    return 0;
}
// Tout est automatiquement nettoyé !
```

## Conclusion

La nouvelle architecture offre :

1. **Sécurité** : Plus de fuites de ressources grâce à RAII
2. **Fiabilité** : Gestion d'erreurs explicite avec std::optional
3. **Maintenabilité** : Code organisé avec namespaces et encapsulation
4. **Performance** : Optimisations avec constexpr et noexcept
5. **Réutilisabilité** : Header-only library facile à intégrer

Le code est maintenant **production-ready** et peut être intégré dans n'importe quel projet moderne en C++.
