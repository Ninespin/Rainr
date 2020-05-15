#include "mesh_register.h"

MeshRegister* MeshRegister::instance_ = nullptr;

MeshRegister* MeshRegister::instance()
{
	if(!instance_)
	{
		instance_ = new MeshRegister();
	}
	return instance_;
}

void MeshRegister::destroy()
{
	delete instance_;
}

MeshRegister::MeshRegister() = default;


MeshRegister::~MeshRegister()
{
	for(auto& mesh: mMeshes)
	{
		delete mesh.second;
	}
	mMeshes.clear();
}

unsigned MeshRegister::registerMesh(const aiMesh* mesh, unsigned int index)
{
	mMeshes[index] = new Mesh(mesh);
	return index;
}

void MeshRegister::deleteMesh(unsigned index)
{
	delete mMeshes[index];
	mMeshes.erase(index);
}

Mesh* MeshRegister::getMeshByIndex(unsigned index)
{
	auto itr = mMeshes.find(index);
	if(itr != mMeshes.end())
	{
		return mMeshes[index];
	}
	return nullptr;
	
}
