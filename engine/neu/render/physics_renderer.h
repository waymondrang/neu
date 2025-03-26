#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

using namespace JPH;

namespace Neu
{

	class PhysicsRenderer : public DebugRendererSimple
	{
	public:
		virtual void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override;

		virtual void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow) override;

		virtual void DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight) override;
	};

}