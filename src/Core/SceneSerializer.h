/**
 * SceneSerializer.h
 * Serializes and deserializes scene state to/from YAML files.
 * Uses YAML format for human-readability and tool compatibility.
 */

#ifndef RUNA_CORE_SCENESERIALIZER_H
#define RUNA_CORE_SCENESERIALIZER_H

#include "../RunaAPI.h"
#include <string>
#include <memory>

// Forward declarations
namespace Runa {
    class Scene;
    namespace ECS {
        class EntityRegistry;
    }
}

// Forward declarations for YAML (will be included in .cpp)
namespace YAML {
    class Emitter;
    class Node;
}

namespace Runa {

    /**
     * SceneSerializer - Handles saving and loading scene state to/from YAML files.
     * 
     * Format:
     * - Human-readable YAML
     * - Compatible with many tools (editors, validators, etc.)
     * - Supports ECS entities with all components
     * - Supports scene-specific metadata
     */
    class RUNA_API SceneSerializer {
    public:
        SceneSerializer() = default;
        ~SceneSerializer() = default;

        /**
         * Save the current scene state to a YAML file.
         * @param scene The scene to serialize
         * @param filePath Path to save the YAML file
         * @return true if successful, false otherwise
         */
        static bool saveScene(Scene& scene, const std::string& filePath);

        /**
         * Load scene state from a YAML file.
         * @param scene The scene to populate
         * @param filePath Path to the YAML file
         * @return true if successful, false otherwise
         */
        static bool loadScene(Scene& scene, const std::string& filePath);

    private:
        // Helper methods for serialization
        static void serializeRegistry(ECS::EntityRegistry& registry, YAML::Emitter& out);
        static void deserializeRegistry(ECS::EntityRegistry& registry, const YAML::Node& node, Scene& scene);
        
        // Component serialization helpers (unused for now, but kept for future extensibility)
        static void serializeComponent(const std::string& componentName, const void* component, YAML::Emitter& out);
        static void deserializeComponent(const std::string& componentName, void* component, const YAML::Node& node);
    };

}

#endif // RUNA_CORE_SCENESERIALIZER_H
