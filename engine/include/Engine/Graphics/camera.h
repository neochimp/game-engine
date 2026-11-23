#pragma once

#include <glm/glm.hpp>

namespace gl {
    class Camera {
    public:
        Camera();
        ~Camera() = default;

        void setPosition(glm::vec3 position);
        void setLook(glm::vec3 look);
        void setAspectRatio(float aspect_ratio);

        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] glm::mat4 getProjection() const;
        [[nodiscard]] glm::mat4 getProjection(float aspect_ratio) const;

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] glm::vec3 getLook() const;
        [[nodiscard]] glm::vec3 getRight() const;
        [[nodiscard]] glm::vec3 getUp() const;

    private:
        glm::vec3 position_;
        glm::vec3 look_;
        glm::vec3 up_;
        glm::vec3 world_up_;
        float near_;
        float far_;
        float fov_;
        float aspect_ratio_;

    };
};
