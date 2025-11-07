# BO1 ESP - Modern C++ Edition

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Une bibliothèque moderne en C++17 pour créer des overlays ESP (Extra Sensory Perception) pour Call of Duty: Black Ops 1 Zombies, conçue pour être facilement intégrable dans n'importe quel projet.

## 🎯 Caractéristiques

### Architecture Moderne
- ✅ **C++17** avec les dernières fonctionnalités
- ✅ **Header-only library** - Facile à intégrer
- ✅ **RAII** pour la gestion automatique des ressources
- ✅ **Namespaces organisés** (`bo1::math`, `bo1::rendering`, `bo1::game`, `bo1::core`)
- ✅ **Smart pointers** et gestion mémoire sécurisée
- ✅ **Type-safe** avec `std::optional` pour les opérations pouvant échouer
- ✅ **constexpr** et **noexcept** pour l'optimisation
- ✅ **CMake** pour la construction multiplateforme

### Fonctionnalités
- 🎮 Affichage des boîtes autour des entités
- 💚 Affichage de la santé
- 🎨 Couleurs personnalisables
- ⚡ Performance optimisée
- 🔧 Configuration flexible à l'exécution

## 📁 Structure du Projet

```
Black-Ops1-Zombie-Hack/
├── include/bo1_esp/           # Headers publics
│   ├── bo1_esp.hpp           # Header principal (include-all)
│   ├── math/
│   │   ├── vector.hpp        # Vec2, Vec3, Vec4 avec opérateurs
│   │   └── matrix.hpp        # Matrix4x4 et projection
│   ├── rendering/
│   │   ├── gdi_resources.hpp # Wrappers RAII pour GDI
│   │   └── renderer.hpp      # Classe Renderer moderne
│   ├── game/
│   │   ├── memory.hpp        # Utilitaires de lecture mémoire sécurisée
│   │   ├── offsets.hpp       # Offsets du jeu (constexpr)
│   │   └── entity.hpp        # Classe Entity encapsulée
│   └── core/
│       ├── config.hpp        # Configuration ESP
│       └── esp_manager.hpp   # Gestionnaire principal
├── src/
│   └── main.cpp              # Point d'entrée DLL
├── examples/                  # Exemples d'utilisation
│   ├── basic_usage.cpp
│   └── custom_config.cpp
├── CMakeLists.txt            # Build system moderne
└── README_MODERN.md          # Cette documentation
```

## 🚀 Installation et Utilisation

### Méthode 1 : Header-Only (Recommandé)

Copiez simplement le dossier `include/bo1_esp/` dans votre projet :

```cpp
#include <bo1_esp/bo1_esp.hpp>

int main() {
    bo1::core::ESPManager esp;

    if (!esp.initialize()) {
        return 1;
    }

    while (esp.isRunning()) {
        esp.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
```

### Méthode 2 : Avec CMake

#### En tant que sous-projet

```cmake
# Dans votre CMakeLists.txt
add_subdirectory(path/to/Black-Ops1-Zombie-Hack)
target_link_libraries(your_target PRIVATE bo1_esp)
```

#### Installation système

```bash
# Build et installation
mkdir build && cd build
cmake ..
cmake --build . --config Release
cmake --install . --prefix /path/to/install

# Dans votre CMakeLists.txt
find_package(bo1_esp REQUIRED)
target_link_libraries(your_target PRIVATE bo1::esp)
```

### Méthode 3 : Build de la DLL

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

# La DLL sera dans build/Release/BO1_ESP.dll
```

## 📚 Guide d'Utilisation

### Configuration de Base

```cpp
#include <bo1_esp/bo1_esp.hpp>

// Configuration par défaut
bo1::core::ESPConfig config;

// Personnalisation
config.display.showBoxes = true;
config.display.showHealth = true;
config.display.boxColor = RGB(255, 0, 0);  // Rouge
config.display.textColor = RGB(255, 255, 255);  // Blanc

bo1::core::ESPManager esp(config);
```

### Utilisation des Vecteurs

```cpp
using namespace bo1::math;

Vec3f position{100.0f, 200.0f, 300.0f};
Vec3f velocity{1.0f, 0.0f, 0.0f};

// Opérateurs surchargés
Vec3f newPos = position + velocity;
float distance = position.length();
Vec3f normalized = position.normalized();
float dot = position.dot(velocity);
```

### Rendering Personnalisé

```cpp
#include <bo1_esp/rendering/renderer.hpp>

HWND hwnd = FindWindowA(nullptr, "Game Window");

bo1::rendering::Renderer::Config config;
config.defaultTextColor = RGB(0, 255, 0);
config.fontSize = 16;

bo1::rendering::Renderer renderer(hwnd, config);

renderer.beginFrame();
renderer.drawText(100, 100, "Hello ESP!");
renderer.drawBorder(50, 50, 200, 100, 2, RGB(255, 0, 0));
renderer.endFrame();
```

### Lecture Mémoire Sécurisée

```cpp
#include <bo1_esp/game/memory.hpp>

uintptr_t address = 0x12345678;

// Lecture sécurisée avec std::optional
auto health = bo1::game::Memory::read<int>(address);
if (health.has_value()) {
    std::cout << "Health: " << health.value() << "\n";
}

// Lecture de tableau
float matrix[16];
if (bo1::game::Memory::readArray(address, matrix)) {
    // Utiliser le tableau...
}
```

### Gestion des Entités

```cpp
#include <bo1_esp/game/entity.hpp>

bo1::game::Entity entity(entityAddress);

if (entity.isAlive()) {
    auto pos = entity.getPosition();
    auto health = entity.getHealth();

    if (pos && health) {
        std::cout << "Position: " << pos->x << ", " << pos->y << ", " << pos->z << "\n";
        std::cout << "Health: " << health.value() << "\n";
    }
}
```

## 🔧 Configuration Avancée

### Structure de Configuration Complète

```cpp
bo1::core::ESPConfig config;

// Affichage
config.display.showBoxes = true;
config.display.showHealth = true;
config.display.showDistance = false;
config.display.showInjectionMessage = true;
config.display.injectionMessageDuration = std::chrono::milliseconds(500);
config.display.boxColor = RGB(158, 66, 244);
config.display.textColor = RGB(255, 255, 255);
config.display.healthColor = RGB(0, 255, 0);
config.display.boxThickness = 2;

// Performance
config.performance.updateInterval = std::chrono::milliseconds(1);
config.performance.maxEntities = 32;

// Écran
config.screen.width = 1920;
config.screen.height = 1080;

// Fenêtre
config.window.title = "Call of Duty®: BlackOps";
```

### Modification à l'Exécution

```cpp
bo1::core::ESPManager esp(config);
esp.initialize();

// Modifier la configuration pendant l'exécution
auto newConfig = esp.getConfig();
newConfig.display.boxColor = RGB(255, 0, 0);
esp.setConfig(newConfig);
```

## 🎓 Exemples

### Exemple 1 : ESP Simple

```cpp
#include <bo1_esp/bo1_esp.hpp>

DWORD WINAPI espThread(LPVOID) {
    bo1::core::ESPManager esp;

    if (!esp.initialize()) {
        return 1;
    }

    while (esp.isRunning()) {
        esp.update();
        Sleep(1);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, espThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
```

### Exemple 2 : ESP avec Touche de Contrôle

```cpp
while (esp.isRunning()) {
    // F1 : Toggle boxes
    if (GetAsyncKeyState(VK_F1) & 1) {
        auto cfg = esp.getConfig();
        cfg.display.showBoxes = !cfg.display.showBoxes;
        esp.setConfig(cfg);
    }

    // F2 : Toggle health
    if (GetAsyncKeyState(VK_F2) & 1) {
        auto cfg = esp.getConfig();
        cfg.display.showHealth = !cfg.display.showHealth;
        esp.setConfig(cfg);
    }

    // END : Quitter
    if (GetAsyncKeyState(VK_END) & 1) {
        esp.stop();
    }

    esp.update();
    Sleep(1);
}
```

## 🏗️ Intégration dans Vos Projets

### 1. Projet Standalone

```cpp
// my_esp_tool.cpp
#include <bo1_esp/bo1_esp.hpp>

int main() {
    std::cout << "BO1 ESP v" << bo1::Version::STRING << "\n";

    bo1::core::ESPManager esp;
    // ... votre logique

    return 0;
}
```

### 2. Plugin pour un Loader

```cpp
// plugin_interface.hpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual void init() = 0;
    virtual void update() = 0;
};

// bo1_esp_plugin.cpp
#include "plugin_interface.hpp"
#include <bo1_esp/bo1_esp.hpp>

class BO1ESPPlugin : public IPlugin {
    bo1::core::ESPManager m_esp;

public:
    void init() override {
        m_esp.initialize();
    }

    void update() override {
        m_esp.update();
    }
};

extern "C" __declspec(dllexport) IPlugin* CreatePlugin() {
    return new BO1ESPPlugin();
}
```

### 3. Framework de Cheats

```cpp
// cheat_framework.hpp
#include <bo1_esp/bo1_esp.hpp>
#include <memory>
#include <vector>

class CheatFramework {
    std::unique_ptr<bo1::core::ESPManager> m_esp;
    // autres modules...

public:
    void initializeModules() {
        // Configuration ESP
        bo1::core::ESPConfig espConfig;
        espConfig.display.showBoxes = true;

        m_esp = std::make_unique<bo1::core::ESPManager>(espConfig);
        m_esp->initialize();
    }

    void run() {
        while (isRunning()) {
            if (m_esp) m_esp->update();
            // autres modules...
        }
    }
};
```

## 🔍 Avantages de cette Architecture

### 1. Sécurité et Fiabilité
- **RAII** : Pas de fuites de ressources GDI
- **std::optional** : Gestion explicite des erreurs
- **Type safety** : Moins d'erreurs à la compilation
- **const correctness** : Intentions claires

### 2. Performance
- **constexpr** : Calculs à la compilation
- **noexcept** : Optimisations du compilateur
- **Move semantics** : Pas de copies inutiles
- **Inline headers** : Optimisation LTO

### 3. Maintenabilité
- **Namespaces** : Organisation logique
- **Encapsulation** : Interfaces claires
- **Documentation** : Commentaires Doxygen
- **Tests** : Architecture testable

### 4. Réutilisabilité
- **Header-only** : Intégration facile
- **CMake** : Build portable
- **Modularité** : Utilisez seulement ce dont vous avez besoin
- **Extensibilité** : Facile à étendre

## 📝 Différences avec l'Ancien Code

| Aspect | Ancien Code | Nouveau Code |
|--------|------------|--------------|
| Variables globales | Oui, partout | Aucune, encapsulation |
| Gestion ressources | Manuelle | RAII automatique |
| Erreurs | Pas de vérification | std::optional |
| Organisation | Fichiers plats | Namespaces hiérarchiques |
| Réutilisabilité | Difficile | Header-only, CMake |
| Build system | Solution VS uniquement | CMake multiplateforme |
| Documentation | Commentaires basiques | Doxygen complet |
| Performance | Bonne | Optimisée (constexpr, noexcept) |
| Type safety | Faible | Fort (templates, std::optional) |

## 🛠️ Compilation

### Visual Studio

```bash
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release
```

### MinGW

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
```

### Options CMake

```bash
cmake -DBO1_ESP_BUILD_EXAMPLES=ON ..
cmake -DBO1_ESP_BUILD_TESTS=ON ..
```

## 📖 Documentation API

Générez la documentation avec Doxygen :

```bash
doxygen Doxyfile
```

## 🤝 Contribution

Ce code est fourni comme exemple d'architecture moderne en C++. Utilisez-le pour apprendre les bonnes pratiques.

## ⚖️ License

Ce projet est à des fins éducatives pour démontrer les pratiques modernes en C++.

## 📧 Support

Pour des questions sur l'architecture C++, consultez la documentation dans les headers.

---

**Note** : Cette bibliothèque démontre les meilleures pratiques modernes en C++17 et peut être adaptée pour n'importe quel projet nécessitant des overlays graphiques ou de la lecture mémoire.
