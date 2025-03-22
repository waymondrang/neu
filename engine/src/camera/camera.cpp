#include "Camera.h"

#include "application.h"

#include "WindowManager.h"

Camera::Camera(const Vector3f &position, const Vector3f &target, const Vector3f &up)
{
    this->position = position;
    
    direction = normalize(position - target);

    globalUp = normalize(up);
    globalRight = normalize(cross(direction, globalUp));
    globalDirection = normalize(cross(globalUp, globalRight));

    // https://stackoverflow.com/a/33920320
    pitch = -atan2f(dot(cross(direction, globalDirection), normalize(cross(direction, globalUp))), dot(globalDirection, direction));
    yaw = 0.0f;

    /**
     * This makes me uncomfortable (true)
     */
    //Application* application = Application::getInstance();

    //application->getWindowManager()->registerFramebufferSizeCallback([this](int width, int height) {
    //    setResolution(Vector2i(width, height));
    //});

    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::updateProjectionMatrix()
{
    projectionMatrix[0] = 1.0f / ((float)s * (float)resolution.x / (float)resolution.y);
    projectionMatrix[5] = 1.0f / (float)s;
    projectionMatrix[10] = (float)-(z_far / (z_far - z_near));
    projectionMatrix[11] = -1.0f;
    projectionMatrix[14] = (float)-(z_far * z_near / (z_far - z_near));
}

void Camera::updateViewMatrix()
{
    /**
     * The camera rotates by first updating the right vector, which
     * involves rotating the global right vector about the global up vector.
     * The direction vector, which depends on the right vector, is then
     * updated using the global direction vector (forward-pointing vector
     * relative to the initial direction and up vector) and the global up
     * vector. Finally, the up vector is updated by taking the cross product
     * of the calculated right and direction vectors.
     *
     * The vectors are not normalized because they do not effect the
     * camera's state; they are updated every iteration. Therefore the error
     * from performing the rotations can be ignored.
     *
     * Note: Implementing roll would require some thinking about which order
     * to apply the rotations.
     */

    right = rotate(globalRight, yaw, globalUp);
    direction = rotate(rotate(globalDirection, yaw, globalUp), pitch, right);
    up = cross(right, direction);

    /**
     * The view matrix is essentially performing a basis change
     * and a translation from the camera space to the world space.
     * This is why the basis change (3x3) matrix is transposed and
     * the translation (4th column) is negated.
     */

    viewMatrix(0, 0) = right.x;
    viewMatrix(1, 0) = up.x;
    viewMatrix(2, 0) = direction.x;
    viewMatrix(3, 0) = 0.0f;

    viewMatrix(0, 1) = right.y;
    viewMatrix(1, 1) = up.y;
    viewMatrix(2, 1) = direction.y;
    viewMatrix(3, 1) = 0.0f;

    viewMatrix(0, 2) = right.z;
    viewMatrix(1, 2) = up.z;
    viewMatrix(2, 2) = direction.z;
    viewMatrix(3, 2) = 0.0f;

    viewMatrix(0, 3) = -dot(right, position);
    viewMatrix(1, 3) = -dot(up, position);
    viewMatrix(2, 3) = -dot(direction, position);
    viewMatrix(3, 3) = 1.0f;
}