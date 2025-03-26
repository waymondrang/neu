#include "physics_renderer.h"

#include "renderer.h"

#include "math/Vector.h"

// CHECK OUT EXAMPLE: https://github.com/jrouwe/JoltPhysics/blob/master/TestFramework/Renderer/DebugRendererImp.h

using namespace Neu;

void PhysicsRenderer::DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor)
{
	Renderer::drawLine(
		Vector3f(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()),
		Vector3f(inTo.GetX(), inTo.GetY(), inTo.GetZ()),
		Vector4f(inColor.r, inColor.g, inColor.b, inColor.a)
	);
}

void PhysicsRenderer::DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow)
{
	Renderer::drawTriangle(
		Vector3f(inV1.GetX(), inV1.GetY(), inV1.GetZ()),
		Vector3f(inV2.GetX(), inV2.GetY(), inV2.GetZ()),
		Vector3f(inV3.GetX(), inV3.GetY(), inV3.GetZ()),
		Vector4f(inColor.r, inColor.g, inColor.b, inColor.a)
	);
}

void PhysicsRenderer::DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight)
{
	// TODO: NOT IMPLEMENTED
}