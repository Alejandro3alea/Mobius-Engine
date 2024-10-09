#include "Renderable.h"
#include "GfxMgr.h"

Renderable::Renderable()
{
	GfxMgr->mRenderComps.push_back(this);
}

Renderable::~Renderable()
{
	std::remove(GfxMgr->mRenderComps.begin(), GfxMgr->mRenderComps.end(), this);
}


AlphaRenderable::AlphaRenderable()
{
	GfxMgr->mAlphaRenderComps.push_back(this);
}

AlphaRenderable::~AlphaRenderable()
{
	std::remove(GfxMgr->mAlphaRenderComps.begin(), GfxMgr->mAlphaRenderComps.end(), this);
}