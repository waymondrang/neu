#pragma once

#include "math/Vector.h"

namespace Neu
{
	class Scene;
	
	// ACTS SIMILARLY TO A SYSTEM, IDEALLY CONTAINS NO STATE
	class Renderer
	{
	public:
		void renderScene(Scene* scene);

		void toggleWireframe();
		//void viewNormals();
		//void viewDiffuse();
		//void toggleShadows();

		// TO SUPPORT PHYSICS RENDERER
		static void drawLine(Vector3f start, Vector3f end, Vector4f color);
		static void drawTriangle(Vector3f v1, Vector3f v2, Vector3f v3, Vector4f color);

	protected:
		bool wireframeMode = false;

		// OTHER SETTINGS LIKE SSAO, BLOOM, DOF (EVENTUALLY)
		// HONESTLY MOVE THESE TO CAMERA COMPONENT
	};

}