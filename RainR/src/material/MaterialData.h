#pragma once
#include "util/vectors.h"

class MaterialData {
public:
	Vec3 diffuse = {0, 0, 0};
	float padd0;
	Vec3 specular = { 0, 0, 0 };
	float padd1;
	Vec3 ambient = { 0, 0, 0 };
	float padd2;
	Vec3 emissive = { 0, 0, 0 };
	float opacity = 1;				// opacity of the material [0, 1]
	float shininess = 0;			// exponent of phong specular equation
	float shininessStrength = 1;	// scales the specular color

	/*MaterialData() = default;
	MaterialData(const MaterialData& copy)
		: diffuse({ copy.diffuse.x, copy.diffuse.y, copy.diffuse.z })
		, specular({ copy.specular.x, copy.specular.y, copy.specular.z })
		, ambient({ copy.ambient.x, copy.ambient.y, copy.ambient.z })
		, emissive({ copy.emissive.x, copy.emissive.y, copy.emissive.z })
		, opacity(copy.opacity)
		, shininess(copy.shininess)
		, shininessStrength(copy.shininessStrength)
	{
	}

	MaterialData& operator=(const MaterialData& other)
	{
		diffuse = other.diffuse;
		specular = other.specular;
		ambient = other.ambient;
		emissive = other.emissive;
		opacity = other.opacity;
		shininess = other.shininess;
		shininessStrength = other.shininessStrength;
	}*/
};
