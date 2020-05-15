#pragma once
#include <assimp/mesh.h>
#include "mesh.h"
#include <map>

class MeshRegister
{
	static MeshRegister* instance_;
public:
	static MeshRegister* instance();
	static void destroy();
	MeshRegister();
	~MeshRegister();
	unsigned int registerMesh(const aiMesh* mesh, unsigned int index);
	void deleteMesh(unsigned int index);
	Mesh* getMeshByIndex(unsigned int index);
	
private:
	std::map<unsigned int, Mesh*> mMeshes;
};

