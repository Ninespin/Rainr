#include "mesh.h"
#include "vertex_buffer_object.h"
#include "element_buffer_object.h"
#include <assimp/material.h>
#include <iostream>

// TODO: ALLOCATOR

aiColor4D Mesh::DEFAULT_MESH_COLOR = aiColor4D(1, 1, 1, 1);

Mesh::Mesh(aiMesh& mesh)
	:mMesh(&mesh)
	, mVao(0)
	, mVerticesVBO(nullptr)
	, mNormalsVBO(nullptr)
	, mColorsVBO(nullptr)
	, mEbo(nullptr)
	, mEboSize(0)
	, mIndicesData()
{
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);

	mVerticesVBO = new VertexBufferObject<aiVector3D>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 3, mesh.mNumVertices, mesh.mVertices, false, ATTRIB_VERTEX);


	if(!mesh.HasVertexColors(0))
	{
		mColorsVBO = new VertexBufferObject<aiColor4D>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 4, 1, &DEFAULT_MESH_COLOR, false, ATTRIB_COLOR, 1);
	}
	else
	{
		mColorsVBO = new VertexBufferObject<aiColor4D>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 4, mesh.mNumVertices, mesh.mColors[0], false, ATTRIB_COLOR);
	}

	if(mesh.HasNormals())
	{
		mNormalsVBO = new VertexBufferObject<aiVector3D>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 3, mesh.mNumVertices, mesh.mNormals, false, ATTRIB_NORMALS);
	}

	if(mesh.HasFaces())
	{
		for (unsigned int i = 0; i < mMesh->mNumFaces; i++)
		{
			mEboSize += mMesh->mFaces[i].mNumIndices;
			for(unsigned int j = 0; j < mMesh->mFaces[i].mNumIndices; j++)
			{
				mIndicesData.push_back(mMesh->mFaces[i].mIndices[j]);
			}
		}
		mEbo = new ElementBufferObject<GLuint>(GL_STATIC_DRAW, mEboSize, mIndicesData.data());
	}
}


Mesh::~Mesh()
{
	delete mVerticesVBO;
	delete mNormalsVBO;
	delete mColorsVBO;
	delete mEbo;
}

void Mesh::bind()
{
	glBindVertexArray(mVao);
}
