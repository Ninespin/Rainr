#pragma once
#include "mesh.h"


class MeshIndirect: public Mesh
{
	typedef struct
	{
		GLuint vertexCount;
		GLuint instanceCount;
		GLuint firstVertex;
		GLuint baseVertex;
		GLuint baseInstance;
	}DrawElementsIndirectCommand;
public:
	MeshIndirect(aiMesh& mesh, unsigned int numInstances = 1);
	~MeshIndirect();
	void draw() override;

public:
	DrawElementsIndirectCommand mCommand;
	GLuint mIndirectBuffer;
};