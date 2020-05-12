#include "camera.h"
#include <gtc/matrix_transform.hpp>


Camera::Camera(float fov, float aspectRatio, float near, float far)
	: mFOV(fov)
	, mAspectRatio(aspectRatio)
	, mNear(near)
	, mFar(far)
	, mViewProjection()
	, mProjection(glm::perspective(mFOV, mAspectRatio, mNear, mFar))
	, mView(1.0f)
	, mPosition(0.0f)
	, mUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, mForward(glm::vec3(0.0, 0.0, -1.0f))
	, mYPR(0.0f, 0.0f, 0.0f)
{
	rebuildProjection();
	rebuildView();
	buildViewProjection();
}


Camera::~Camera()
{
}

void Camera::rebuildProjection()
{
	mProjection = glm::perspective(mFOV, mAspectRatio, mNear, mFar);
}

void Camera::rebuildView()
{
	mForward.x = cos(glm::radians(mYPR.x)) * cos(glm::radians(mYPR.y));
	mForward.y = sin(glm::radians(mYPR.y));
	mForward.z = sin(glm::radians(mYPR.x)) * cos(glm::radians(mYPR.y));
	mForward = glm::normalize(mForward);
	mView = glm::lookAt(mPosition, mPosition + mForward, mUp);
}

void Camera::buildViewProjection()
{
	mViewProjection = mProjection * mView;
}

void Camera::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}

void Camera::translate(glm::vec3 trans)
{
	mPosition += trans;
	rebuildView();
	buildViewProjection();
}

void Camera::setTranslation(float x, float y, float z)
{
	setTranslation(glm::vec3(x, y, z));
}

void Camera::setTranslation(glm::vec3 trans)
{
	mPosition = trans;
	rebuildView();
	buildViewProjection();
}

void Camera::rotate(float yaw, float pitch, float roll)
{
	rotate(glm::vec3(yaw, pitch, roll));
}

void Camera::rotate(glm::vec3 ypr)
{
	const float pitchCap = 89.0f;
	mYPR += ypr;
	if (mYPR.y > pitchCap)
	{
		mYPR.y = pitchCap;
	}
	else if (mYPR.y < -pitchCap)
	{
		mYPR.y = -pitchCap;
	}

	rebuildView();
	buildViewProjection();
}

void Camera::setYPR(float yaw, float pitch, float roll)
{
	setYPR(glm::vec3(yaw, pitch, roll));
}

void Camera::setYPR(glm::vec3 ypr)
{
	mYPR = ypr;
	rebuildView();
	buildViewProjection();
}

float* Camera::getViewProjection()
{
	return &mViewProjection[0][0];
}
