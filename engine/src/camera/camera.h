#ifndef CAMERA_H
#define CAMERA_H

#include "math/vector.h"
#include "math/matrix.h"

#define MAX_PITCH 1.5533430343f
#define DEFAULT_HEIGHT 600
#define DEFAULT_WIDTH 800

class Application;

// handles camera and view matrix
class Camera
{
protected:
	/**
	 * The field of view of the camera.
	 */
	float s = 1.0f;

	/**
	 * The near clipping plane.
	 */
	float z_near = 0.1f;

	/**
	 * The far clipping plane.
	 */
	float z_far = 100.0f;

	/**
	 * The pitch (up and down rotation) of the camera.
	 */
	float pitch = 0.0f;

	/**
	 * The yaw (left and right rotation) of the camera.
	 */
	float yaw = 0.0f;

	/**
	 * The resolution of the camera (width, height).
	 */
	Vector2i resolution = Vector2i(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	/**
	 * The position of the camera in world space.
	 */
	Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);

	/**
	 * The global up unit vector. It describes the up
	 * direction of the world space.
	 */
	Vector3f globalUp = Vector3f(0.0f, 1.0f, 0.0f);

	/**
	 * The global direction unit vector.
	 */
	Vector3f globalDirection = Vector3f(0.0f, 0.0f, -1.0f);

	/**
	 * The global right unit vector.
	 */
	Vector3f globalRight = Vector3f(1.0f, 0.0f, 0.0f);

	/**
	 * The unit vector pointing in the direction the camera is facing.
	 */
	Vector3f direction;

	/**
	 * The unit vector pointing to the right of the camera.
	 */
	Vector3f right;

	/**
	 * The unit vector pointing up from the camera.
	 */
	Vector3f up;

	/**
	 * The view matrix. This matrix transforms world space
	 * coordinates to camera space coordinates.
	 */
	Matrix4x4f viewMatrix;

	/**
	 * The projection matrix. This matrix transforms camera
	 * space coordinates to clip space coordinates.
	 */
	float projectionMatrix[16] = { 0.0f };

	/**
	 * Updates the view matrix based on the camera's position and orientation.
	 */
	void updateViewMatrix();

	/**
	 * Updates the projection matrix based on the camera's properties.
	 */
	void updateProjectionMatrix();

public:
	/**
	 * Explicit constructor for the camera.
	 *
	 * @param position The position of the camera in world space.
	 * @param target The point the camera is looking at.
	 * @param up The up direction of the world space.
	 */
	Camera(const Vector3f& position, const Vector3f& target, const Vector3f& up);

	////////////////////////////////
	// Inline Getters and Setters //
	////////////////////////////////

	float getPitch() const { return pitch; }
	float getYaw() const { return yaw; }

	Vector2i getResolution() const { return resolution; }
	void setResolution(int width, int height) { setResolution(Vector2i(width, height)); }
	void setResolution(const Vector2i& resolution) { this->resolution = resolution; updateProjectionMatrix(); }

	void setPosition(const Vector3f& position) { this->position = position; updateViewMatrix(); }
	void setPitch(float pitch) { this->pitch = pitch; updateViewMatrix(); }
	void setYaw(float yaw) { this->yaw = yaw; updateViewMatrix(); }

	Vector3f getPosition() const { return position; }

	Vector3f getDirection() const { return direction; }
	Vector3f getRight() const { return right; }
	Vector3f getUp() const { return up; }

	Vector3f getGlobalUp() const { return globalUp; }
	Vector3f getGlobalDirection() const { return globalDirection; }
	Vector3f getGlobalRight() const { return globalRight; }

	Matrix4x4f getViewMatrix() const { return viewMatrix; }

	const float* getProjectionMatrix() const { return projectionMatrix; }

	/////////////////////
	// Virtual Methods //
	/////////////////////

	virtual void update(float deltaTime) = 0;
};

#endif