# Guide d'Intégration - BO1 ESP Library

Ce guide explique comment intégrer la bibliothèque BO1 ESP dans différents types de projets.

## Table des Matières

1. [Intégration Rapide](#intégration-rapide)
2. [Intégration avec CMake](#intégration-avec-cmake)
3. [Intégration sans CMake](#intégration-sans-cmake)
4. [Utilisation en tant que Sous-module Git](#utilisation-en-tant-que-sous-module-git)
5. [Intégration dans un Framework Existant](#intégration-dans-un-framework-existant)
6. [Exemples de Projets](#exemples-de-projets)

---

## Intégration Rapide

### Option 1 : Copier les Headers (Plus Simple)

1. Copiez le dossier `include/bo1_esp/` dans votre projet
2. Ajoutez le chemin d'include à votre compilateur
3. Incluez le header principal

```cpp
#include <bo1_esp/bo1_esp.hpp>

int main() {
    bo1::core::ESPManager esp;
    esp.initialize();
    // ...
    return 0;
}
```

### Option 2 : Header-Only avec Lien Symbolique

```bash
# Depuis votre projet
ln -s /path/to/Black-Ops1-Zombie-Hack/include/bo1_esp ./include/

# Dans votre code
#include <bo1_esp/bo1_esp.hpp>
```

---

## Intégration avec CMake

### Méthode 1 : add_subdirectory (Recommandé)

```cmake
# Votre CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(MonProjet)

# Ajouter le sous-répertoire
add_subdirectory(external/Black-Ops1-Zombie-Hack)

# Créer votre exécutable/bibliothèque
add_executable(mon_app main.cpp)

# Lier avec bo1_esp
target_link_libraries(mon_app PRIVATE bo1_esp)
```

### Méthode 2 : FetchContent (Depuis Git)

```cmake
include(FetchContent)

FetchContent_Declare(
    bo1_esp
    GIT_REPOSITORY https://github.com/votre-repo/Black-Ops1-Zombie-Hack.git
    GIT_TAG main
)

FetchContent_MakeAvailable(bo1_esp)

add_executable(mon_app main.cpp)
target_link_libraries(mon_app PRIVATE bo1_esp)
```

### Méthode 3 : find_package (Installation Système)

```bash
# Installation de la bibliothèque
cd Black-Ops1-Zombie-Hack
mkdir build && cd build
cmake ..
cmake --install . --prefix /usr/local
```

```cmake
# Dans votre projet
find_package(bo1_esp REQUIRED)

add_executable(mon_app main.cpp)
target_link_libraries(mon_app PRIVATE bo1::esp)
```

---

## Intégration sans CMake

### Visual Studio (Solution .sln)

1. **Ajouter le chemin d'include** :
   - Propriétés du projet → C/C++ → Général → Répertoires Include supplémentaires
   - Ajouter : `$(SolutionDir)Black-Ops1-Zombie-Hack\include`

2. **Dans votre code** :
```cpp
#include <bo1_esp/bo1_esp.hpp>
```

### Makefile Manuel

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I./Black-Ops1-Zombie-Hack/include

mon_app: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) main.cpp -o mon_app

clean:
	rm -f mon_app
```

### Compilation en Ligne de Commande

```bash
# GCC/Clang
g++ -std=c++17 -I./Black-Ops1-Zombie-Hack/include main.cpp -o mon_app

# MSVC
cl /std:c++17 /I".\Black-Ops1-Zombie-Hack\include" main.cpp
```

---

## Utilisation en tant que Sous-module Git

### Configuration Initiale

```bash
# Ajouter comme sous-module
git submodule add https://github.com/votre-repo/Black-Ops1-Zombie-Hack.git external/bo1_esp

# Initialiser et mettre à jour
git submodule update --init --recursive
```

### CMakeLists.txt

```cmake
# Ajouter le sous-module
add_subdirectory(external/bo1_esp)

target_link_libraries(votre_target PRIVATE bo1_esp)
```

### Cloner un Projet avec Sous-modules

```bash
git clone --recurse-submodules https://github.com/votre-repo/votre-projet.git
```

---

## Intégration dans un Framework Existant

### Exemple 1 : Framework de Cheats Modulaire

```cpp
// cheat_module.hpp
#pragma once

class ICheatModule {
public:
    virtual ~ICheatModule() = default;
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
    virtual const char* getName() const = 0;
};

// esp_module.cpp
#include "cheat_module.hpp"
#include <bo1_esp/bo1_esp.hpp>
#include <memory>

class ESPModule : public ICheatModule {
public:
    void initialize() override {
        bo1::core::ESPConfig config;
        config.display.showBoxes = true;
        config.display.showHealth = true;

        m_esp = std::make_unique<bo1::core::ESPManager>(config);
        m_esp->initialize();
    }

    void update() override {
        if (m_esp) {
            m_esp->update();
        }
    }

    void shutdown() override {
        if (m_esp) {
            m_esp->stop();
        }
    }

    const char* getName() const override {
        return "ESP Module";
    }

private:
    std::unique_ptr<bo1::core::ESPManager> m_esp;
};

// Enregistrement du module
extern "C" __declspec(dllexport) ICheatModule* CreateModule() {
    return new ESPModule();
}
```

### Exemple 2 : Système de Menu

```cpp
// menu_system.cpp
#include <bo1_esp/bo1_esp.hpp>

class MenuSystem {
public:
    void initialize() {
        m_esp = std::make_unique<bo1::core::ESPManager>();
        m_esp->initialize();
    }

    void handleInput() {
        if (GetAsyncKeyState(VK_F1) & 1) {
            toggleESPBoxes();
        }
        if (GetAsyncKeyState(VK_F2) & 1) {
            toggleESPHealth();
        }
    }

    void update() {
        if (m_esp && m_espEnabled) {
            m_esp->update();
        }
    }

private:
    void toggleESPBoxes() {
        auto config = m_esp->getConfig();
        config.display.showBoxes = !config.display.showBoxes;
        m_esp->setConfig(config);
    }

    void toggleESPHealth() {
        auto config = m_esp->getConfig();
        config.display.showHealth = !config.display.showHealth;
        m_esp->setConfig(config);
    }

    std::unique_ptr<bo1::core::ESPManager> m_esp;
    bool m_espEnabled{true};
};
```

### Exemple 3 : Intégration avec ImGui

```cpp
#include <bo1_esp/bo1_esp.hpp>
#include <imgui.h>

class ESPSettings {
public:
    void renderUI() {
        ImGui::Begin("ESP Settings");

        auto config = m_esp->getConfig();
        bool modified = false;

        if (ImGui::Checkbox("Show Boxes", &config.display.showBoxes)) {
            modified = true;
        }

        if (ImGui::Checkbox("Show Health", &config.display.showHealth)) {
            modified = true;
        }

        float color[3] = {
            GetRValue(config.display.boxColor) / 255.0f,
            GetGValue(config.display.boxColor) / 255.0f,
            GetBValue(config.display.boxColor) / 255.0f
        };

        if (ImGui::ColorEdit3("Box Color", color)) {
            config.display.boxColor = RGB(
                color[0] * 255,
                color[1] * 255,
                color[2] * 255
            );
            modified = true;
        }

        if (modified) {
            m_esp->setConfig(config);
        }

        ImGui::End();
    }

private:
    std::unique_ptr<bo1::core::ESPManager> m_esp;
};
```

---

## Exemples de Projets

### Projet Minimal (DLL)

```
MonProjetESP/
├── CMakeLists.txt
├── src/
│   └── main.cpp
└── external/
    └── bo1_esp/      (lien symbolique ou sous-module)
```

**CMakeLists.txt** :
```cmake
cmake_minimum_required(VERSION 3.15)
project(MonProjetESP)

add_subdirectory(external/bo1_esp)

add_library(mon_esp SHARED src/main.cpp)
target_link_libraries(mon_esp PRIVATE bo1_esp)

set_target_properties(mon_esp PROPERTIES
    OUTPUT_NAME "MonESP"
    PREFIX ""
    SUFFIX ".dll"
)
```

**src/main.cpp** :
```cpp
#include <bo1_esp/bo1_esp.hpp>
#include <Windows.h>

DWORD WINAPI MainThread(LPVOID) {
    bo1::core::ESPManager esp;

    if (!esp.initialize()) {
        return 1;
    }

    while (esp.isRunning()) {
        esp.update();
        Sleep(1);

        if (GetAsyncKeyState(VK_END) & 1) {
            esp.stop();
        }
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
```

### Projet avec Configuration Personnalisée

```cpp
// config.hpp
#pragma once
#include <bo1_esp/core/config.hpp>

namespace myproject {

inline bo1::core::ESPConfig createDefaultConfig() {
    bo1::core::ESPConfig config;

    config.display.showBoxes = true;
    config.display.showHealth = true;
    config.display.boxColor = RGB(255, 100, 50);
    config.display.textColor = RGB(255, 255, 255);
    config.performance.updateInterval = std::chrono::milliseconds(5);

    return config;
}

} // namespace myproject

// main.cpp
#include "config.hpp"
#include <bo1_esp/bo1_esp.hpp>

int main() {
    auto config = myproject::createDefaultConfig();
    bo1::core::ESPManager esp(config);

    // ...
}
```

---

## Utilisation de Composants Individuels

Vous n'êtes pas obligé d'utiliser l'ESP complet. Vous pouvez utiliser uniquement les composants dont vous avez besoin :

### Utiliser Seulement les Vecteurs

```cpp
#include <bo1_esp/math/vector.hpp>

using namespace bo1::math;

Vec3f calculateDistance(const Vec3f& a, const Vec3f& b) {
    return b - a;
}
```

### Utiliser Seulement le Renderer

```cpp
#include <bo1_esp/rendering/renderer.hpp>

HWND hwnd = FindWindowA(nullptr, "MonJeu");
bo1::rendering::Renderer renderer(hwnd);

renderer.beginFrame();
renderer.drawText(100, 100, "Hello!");
renderer.endFrame();
```

### Utiliser Seulement la Lecture Mémoire

```cpp
#include <bo1_esp/game/memory.hpp>

auto health = bo1::game::Memory::read<int>(0x12345678);
if (health) {
    std::cout << "Health: " << health.value() << "\n";
}
```

---

## Dépannage

### Problème : Headers Introuvables

**Solution** : Vérifiez que le chemin d'include est correct :

```cmake
target_include_directories(votre_target PRIVATE
    ${CMAKE_SOURCE_DIR}/path/to/Black-Ops1-Zombie-Hack/include
)
```

### Problème : Erreurs de Liaison

**Solution** : La bibliothèque est header-only, aucune liaison n'est nécessaire. Assurez-vous d'inclure les headers nécessaires.

### Problème : Version C++ Incompatible

**Solution** : Assurez-vous d'utiliser C++17 ou supérieur :

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

---

## Ressources Supplémentaires

- [README Principal](README_MODERN.md)
- [Documentation API](docs/api/)
- [Exemples](examples/)

---

**Note** : Cette bibliothèque est conçue pour être flexible et s'intégrer facilement dans n'importe quel type de projet. Si vous avez des questions sur l'intégration, consultez les exemples fournis.
