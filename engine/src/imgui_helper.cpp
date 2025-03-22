#include "imgui_helper.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>

#include "camera/Camera.h"
#include "World.h"

#include "object/Skeleton.h"
#include "object/Joint.h"
#include "object/Cloth.h"
#include "object/ParticleEmitter.h"

#include "application.h"
#include "WindowManager.h"

const float PAD = 8.0f;

void showTimeWidget();
void showWindowWidget();

void ImGuiHelper::setup()
{
	/**
	 * Helpful reference:
	 * https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
	 */

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

	ImGui_ImplGlfw_InitForOpenGL(Application::getInstance()->getWindowManager()->getNativeWindow(), true);
	ImGui_ImplOpenGL3_Init();
}

void ImGuiHelper::startFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiHelper::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiHelper::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void showTimeWidget()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;

	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + PAD;

	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 0.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver, window_pos_pivot);
	// ImGui::SetNextWindowSizeConstraints(ImVec2(100, -1), ImVec2(100, -1), NULL, NULL);
	// ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background

	ImGui::Begin("Statistics", NULL, window_flags);
	ImGui::Text("FPS: %.1f", io.Framerate);
	ImGui::Text("Wall time: %.3f", Application::getInstance()->getWallTime());

	ImGui::End();
	// ImGui::PopStyleVar();
}

void GameObjectPropertyEditor::draw()
{
	// Left side: draw tree
	// - Currently using a table to benefit from RowBg feature
	if (ImGui::BeginChild("##tree", ImVec2(120, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
	{
		// ImGui::SetNextItemWidth(-FLT_MIN);
		// ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
		// ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
		// if (ImGui::InputTextWithHint("##Filter", "incl,-excl", textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll))
		//     textFilter.Build();
		// ImGui::PopItemFlag();

		if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg))
		{
			for (GameObject* gameObject : Application::getInstance()->getWorld()->getGameObjects())
				//if (textFilter.PassFilter(gameObject->getName().c_str()))
				drawGameObject(gameObject);

			ImGui::EndTable();
		}
	}

	ImGui::EndChild();

	// Right side: draw properties
	ImGui::SameLine();

	ImGui::BeginChild("##properties", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::BeginGroup(); // Lock X position

	if (focusedGameObject)
	{
		ImGui::Text("Properties of %s", focusedGameObject->getName().c_str());
		ImGui::Separator();

		ImGui::Checkbox("Visible", focusedGameObject->getVisiblePointer());

		if (focusedGameObject->getType() == GameObjectType::SKELETON)
		{
			Skeleton* skeleton = dynamic_cast<Skeleton*>(focusedGameObject);

			if (skeleton->getAnimation())
				ImGui::Checkbox("Animate", skeleton->getAnimatePointer());
		}
		else if (focusedGameObject->getType() == GameObjectType::CLOTH)
		{
			Cloth* cloth = dynamic_cast<Cloth*>(focusedGameObject);

			ImGui::PushItemWidth(80);
			ImGui::InputFloat("Spring Constant", cloth->getSpringConstantPointer());
			ImGui::InputFloat("Damping Constant", cloth->getDampingConstantPointer());
			ImGui::InputFloat("Wind Velocity", cloth->getWindVelocityPointer());
			ImGui::InputFloat("Simulation Timestep", cloth->getFixedDeltaTimePointer());
			ImGui::PopItemWidth();

			if (ImGui::Button("Reset"))
			{
				cloth->reset();
			}
		}
		else if (focusedGameObject->getType() == GameObjectType::PARTICLE_EMITTER)
		{
			ParticleEmitter* particleEmitter = dynamic_cast<ParticleEmitter*>(focusedGameObject);

			// ImGui::PushItemWidth(90);
			ImGui::Text("Particle count: %d", particleEmitter->getParticleCount());
			ImGui::InputInt("creation rate", particleEmitter->getParticleCreationRatePointer());
			ImGui::SliderFloat3("position", particleEmitter->getInitialPositionPointer()->ptr(), -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat3("position.variance", particleEmitter->getInitialPositionVariancePointer()->ptr(), -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat3("velocity", particleEmitter->getInitialVelocityPointer()->ptr(), -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat3("velocity.variance", particleEmitter->getInitialVelocityVariancePointer()->ptr(), -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("lifespan", particleEmitter->getInitialLifeSpanPointer(), 0.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("lifespan.variance", particleEmitter->getInitialLifeSpanVariancePointer(), 0.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("radius", particleEmitter->getParticleRadiusPointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("radius.variance", particleEmitter->getParticleRadiusVariancePointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("radius.lower bound", particleEmitter->getParticleRadiusLowerBoundPointer(), 0.0f, 1.0f, "%.2f");

			ImGui::ColorEdit4("color", particleEmitter->getParticleColorPointer()->ptr());
			ImGui::SliderFloat("color.red variance", particleEmitter->getParticleColorRedVariancePointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("color.green variance", particleEmitter->getParticleColorGreenVariancePointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("color.blue variance", particleEmitter->getParticleColorBlueVariancePointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("color.alpha variance", particleEmitter->getParticleColorAlphaVariancePointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("color.uniform variance", particleEmitter->getParticleColorUniformVariancePointer(), 0.0f, 1.0f, "%.2f");

			ImGui::SliderFloat3("gravity", particleEmitter->getGravityPointer()->ptr(), -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("air density", particleEmitter->getAirDensityPointer(), 0.0f, 1000.0f, "%.2f");
			ImGui::SliderFloat("drag coefficient", particleEmitter->getDragCoefficientPointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("collision elasticity", particleEmitter->getCollisionElasticityPointer(), 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("collision friction", particleEmitter->getCollisionFrictionPointer(), 0.0f, 2.0f, "%.2f");

			// ImGui::PopItemWidth();

			if (ImGui::Button("Reset"))
			{
				particleEmitter->reset();
			}
		}

		// POSITION
		ImGui::Separator();
		ImGui::Text("Position");
		ImGui::PushItemWidth(-24);
		ImGui::SliderFloat("X##position", focusedGameObject->getPositionPointer()->ptr(), -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Y##position", focusedGameObject->getPositionPointer()->ptr() + 1, -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Z##position", focusedGameObject->getPositionPointer()->ptr() + 2, -10.0f, 10.0f, "%.2f");
		ImGui::PopItemWidth();

		// ROTATION
		ImGui::Separator();
		ImGui::Text("Rotation");
		ImGui::PushItemWidth(-24);
		ImGui::SliderFloat("X##rotation", focusedGameObject->getRotationPointer()->ptr(), -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Y##rotation", focusedGameObject->getRotationPointer()->ptr() + 1, -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Z##rotation", focusedGameObject->getRotationPointer()->ptr() + 2, -10.0f, 10.0f, "%.2f");
		ImGui::PopItemWidth();

		// SCALE
		ImGui::Separator();
		ImGui::Text("Scale");
		ImGui::PushItemWidth(-24);
		ImGui::SliderFloat("X##scale", focusedGameObject->getScalePointer()->ptr(), 0.1f, 10.0f, "%.2f");
		ImGui::SliderFloat("Y##scale", focusedGameObject->getScalePointer()->ptr() + 1, 0.1f, 10.0f, "%.2f");
		ImGui::SliderFloat("Z##scale", focusedGameObject->getScalePointer()->ptr() + 2, 0.1f, 10.0f, "%.2f");
		ImGui::PopItemWidth();
	}

	ImGui::EndGroup();

	ImGui::EndChild();
}

void GameObjectPropertyEditor::drawGameObject(GameObject* gameObject)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::PushID(gameObject->getID());

	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
	tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (gameObject == focusedGameObject)
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	if (gameObject->getType() == GameObjectType::JOINT)
	{
		if (dynamic_cast<Joint*>(gameObject)->getChildren().size() == 0)
			tree_flags |= ImGuiTreeNodeFlags_Leaf;
	}
	else
	{
		tree_flags |= ImGuiTreeNodeFlags_Leaf;
	}

	// format string "name (id)"
	std::string nodeLabel = gameObject->getName() + " (" + std::to_string(gameObject->getID()) + ")";
	bool node_open = ImGui::TreeNodeEx(nodeLabel.c_str(), tree_flags);

	if (ImGui::IsItemClicked())
	{
		if (focusedGameObject == gameObject)
			focusedGameObject = NULL;
		else
			focusedGameObject = gameObject;
	}

	if (node_open)
	{
		if (gameObject->getType() == GameObjectType::SKELETON)
		{
			Skeleton* skeleton = dynamic_cast<Skeleton*>(gameObject);
			Joint* rootJoint = skeleton->getRootJoint();

			drawGameObject(rootJoint);
		}
		else if (gameObject->getType() == GameObjectType::JOINT)
		{
			Joint* joint = dynamic_cast<Joint*>(gameObject);

			for (Joint* child : joint->getChildren())
				drawGameObject(child);
		}

		ImGui::TreePop(); // This is required at the end of the if block
	}

	ImGui::PopID();
}

void ImGuiHelper::showGamePropertyEditor()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Game Object Property Editor", NULL, ImGuiWindowFlags_NoCollapse);

	gameObjectPropertyEditor.draw();

	ImGui::End();
};

void buildProfilerTree(int sectionID, std::vector<ProfilerSection*>& section)
{
	for (int childSectionID : section[sectionID]->children)
	{
		ImGui::Indent();
		ImGui::Text("%s: %.3f ms", section[childSectionID]->name.c_str(), Profiler::getInstance().getTimeForSection(childSectionID).count() * 1000);
		buildProfilerTree(childSectionID, section);
		ImGui::Unindent();
	}
}

void showWindowWidget()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings;

	const Application* application = Application::getInstance();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;

	window_pos.x = work_pos.x + PAD;
	window_pos.y = work_pos.y + PAD;

	window_pos_pivot.x = 0.0f;
	window_pos_pivot.y = 0.0f;

	const int windowWidth = 480;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver, window_pos_pivot);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Debug Window", NULL, window_flags))
	{
		ImGui::End();
		return;
	}

	ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::CollapsingHeader("World Settings"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

		if (ImGui::Button("Toggle Wireframe (E)"))
			application->getWorld()->toggleWireframe();

		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

		ImGui::BeginTable("Camera Position", 3, tableFlags);

		ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%.2f", application->getCamera()->getPosition().x);
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%.2f", application->getCamera()->getPosition().y);
		ImGui::TableSetColumnIndex(2);
		ImGui::Text("%.2f", application->getCamera()->getPosition().z);
		ImGui::EndTable();

		// --------------------------------------------- //

		ImGui::BeginTable("Camera Rotation", 2, tableFlags);

		ImGui::TableSetupColumn("Pitch", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Yaw", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%.2f", application->getCamera()->getPitch());
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%.2f", application->getCamera()->getYaw());
		ImGui::EndTable();

		ImGui::Separator();

		if (ImGui::Button("Toggle Wireframe (E)"))
		{
			application->getWorld()->toggleWireframe();
		}

		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::PushItemWidth(-100);

		for (Light* light : application->getWorld()->getLights())
		{
			ImGui::PushID(light->getID());

			ImGui::SeparatorText(light->getName().c_str());
			ImGui::SliderFloat("X", light->getPositionRef()->ptr(), -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Y", light->getPositionRef()->ptr() + 1, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Z", light->getPositionRef()->ptr() + 2, -10.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::ColorEdit3("Color", light->getColorRef()->ptr());

			ImGui::PopID();

			ImGui::Separator();
		}

		ImGui::PopItemWidth();
	}

	if (ImGui::CollapsingHeader("Profiler"))
	{
		std::vector<ProfilerSection*> profilerData = Profiler::getInstance().getProfilerData();

		if (profilerData.size() == 0)
		{
			ImGui::Text("No profiler data available");
		}
		else
		{
			ImGui::Text("%s: %.3f ms", profilerData[0]->name.c_str(), Profiler::getInstance().getTimeForSection(0).count() * 1000);
			buildProfilerTree(0, profilerData);
		}
	}

	ImGui::End();
	// ImGui::PopStyleVar();
}