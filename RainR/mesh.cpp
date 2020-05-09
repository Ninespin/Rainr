#include "mesh.h"
#include "vertex_buffer_object.h"
#include "element_buffer_object.h"

// TODO: ALLOCATOR

Mesh::Mesh(aiMesh& mesh)
	:mMesh(&mesh)
	, mVao(0)
	, mVerticesVBO(nullptr)
	, mNormalsVBO(nullptr)
	, mEbo(nullptr)
	, mEboSize(0)
{
	for(unsigned int i = 0; i < 26; i++)
	{
		fake[i] = i;
	}
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);

	mVerticesVBO = new VertexBufferObject<aiVector3D>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 3, mesh.mNumVertices, mesh.mVertices, false, ATTRIB_VERTEX);

	// vertex
	/*glGenBuffers(1, &mVboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, mVboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh.mNumVertices,
		mesh.mVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);
	glEnableVertexAttribArray(0);*/

	// uv
	/*GLuint screen_uv_buffer;
	glGenBuffers(1, &screen_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, screen_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_QUAD_UV_DATA),
		SCREEN_QUAD_UV_DATA, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), nullptr);
	glEnableVertexAttribArray(1);*/

	if(mesh.HasNormals())
	{
		
	}

	if(mesh.HasFaces())
	{
		for (unsigned int i = 0; i < mMesh->mNumFaces; i++)
		{
			mEboSize += mMesh->mFaces[i].mNumIndices;
		}

		

		mEbo = new ElementBufferObject<GLuint>(GL_STATIC_DRAW, 36/*mEboSize*/, reinterpret_cast<GLuint*>(&fake));//mesh.mFaces
		//mFlags |= Flags::ELEMENT_BUFFER;

		// element indices
		//glGenBuffers(1, &mEbo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.mNumFaces,
		//	NULL, GL_STATIC_DRAW);
	}
}


Mesh::~Mesh()
{
	delete mVerticesVBO;
	delete mNormalsVBO;
	delete mEbo;
}

void Mesh::bind()
{
	glBindVertexArray(mVao);
}
