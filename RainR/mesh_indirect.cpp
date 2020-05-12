#include "mesh_indirect.h"

MeshIndirect::MeshIndirect(aiMesh& mesh, unsigned numInstances)
	: Mesh(mesh)
	, mCommand()
{
	mCommand = { mEboSize, numInstances, 0, 0, 0 };
	glGenBuffers(1, &mIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &mCommand, GL_STATIC_DRAW);
}

MeshIndirect::~MeshIndirect()
{
	glDeleteBuffers(1, &mIndirectBuffer);
}

void MeshIndirect::draw()
{
	bind();
	glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
}
