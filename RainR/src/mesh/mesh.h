#pragma once
#include <GL/glew.h>
#include <assimp/mesh.h>
#include <vector>
#include "buffer/vertex_buffer_object.h"
#include "buffer/element_buffer_object.h"
#include "buffer/vertex_array_buffer.h"

class Mesh
{
public:

	enum AttribLocations: unsigned int
	{
		ATTRIB_VERTEX = 0,
		ATTRIB_NORMALS = 2,
		ATTRIB_COLOR = 3,
		ATTRIB_SECONDARY_COLOR = 4,
		ATTRIB_FOG_COORD = 5,
		ATTRIB_MULTI_TEX_COORD_0 = 8,
		ATTRIB_MULTI_TEX_COORD_1,
		ATTRIB_MULTI_TEX_COORD_2,
		ATTRIB_MULTI_TEX_COORD_3,
		ATTRIB_MULTI_TEX_COORD_4,
		ATTRIB_MULTI_TEX_COORD_5,
		ATTRIB_MULTI_TEX_COORD_6,
		ATTRIB_MULTI_TEX_COORD_7,
	};
public:
	Mesh(const aiMesh* mesh);
	virtual ~Mesh();
	virtual void bind();
	virtual void draw();
public:
	const aiMesh* mMesh;
	VertexArrayBuffer* mVao;
	VertexBufferObject<aiVector3D>* mVerticesVBO;
	VertexBufferObject<aiVector3D>* mNormalsVBO;
	VertexBufferObject<aiColor4D>* mColorsVBO;
	ElementBufferObject<GLuint>* mEbo;
	unsigned int mEboSize;
	std::vector<unsigned int> mIndicesData;
protected:
	static aiColor4D DEFAULT_MESH_COLOR;

};

