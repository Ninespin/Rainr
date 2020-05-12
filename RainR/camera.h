#pragma once
#include <mat4x4.hpp>

class Camera
{
public:
	Camera(float fov_rad, float aspectRatio, float near, float far);
	~Camera();
	void rebuildProjection();
	void rebuildView();
	void buildViewProjection();
	void translate(float x, float y, float z);
	void translate(glm::vec3 trans);
	void setTranslation(float x, float y, float z);
	void setTranslation(glm::vec3 trans);
	void rotate(float yaw, float pitch, float roll = 0);
	void rotate(glm::vec3 ypr);
	void setYPR(float yaw, float pitch, float roll = 0);
	void setYPR(glm::vec3 ypr);
	float* getViewProjection();

public:
	float mFOV;
	float mAspectRatio;
	float mNear;
	float mFar;
	glm::mat4 mViewProjection;
	glm::mat4 mProjection;
	glm::mat4 mView;
	glm::vec3 mPosition;
	glm::vec3 mUp;
	glm::vec3 mForward;

private:
	glm::vec3 mYPR;
};

