#include "Mesh.h"
#include "Shader/Shader.h"
#include "Model.h"

void Mesh::Render(Shader* shader) const
{
	for (unsigned i = 0; i < mPrimitives.size(); i++)
	{
		const Primitive& currPrim = mPrimitives[i];
		if (currPrim.mMatIdx != -1)
			shader->SetUniform("uMaterial", mModel->mMaterials[currPrim.mMatIdx]);

		// draw mesh
		glBindVertexArray(currPrim.mVAO);
		if (currPrim.mUsingIndices)
			glDrawElements(currPrim.mRenderMode, static_cast<GLsizei>(currPrim.mCount), currPrim.mIdxType, (GLvoid*)currPrim.mIdxOffset);
		else
			glDrawArrays(currPrim.mRenderMode, 0, static_cast<GLsizei>(currPrim.mCount));
	}
}
