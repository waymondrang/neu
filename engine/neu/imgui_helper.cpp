#include "imgui_helper.h"

#include <string>

#include "application.h"
#include "components/rigid_body.h"
#include "components/transform.h"
#include "scene/camera.h"
#include "scene/game_object.h"
#include "scene/scene.h"
#include "utility/profiler.h"
#include "window_manager.h"

using namespace Neu;

const float PAD = 8.0f;

ImGuiHelper::ImGuiHelper() {}

void ImGuiHelper::setup() {
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
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

  ImGui_ImplGlfw_InitForOpenGL(
      Application::getInstance()->getWindowManager()->getNativeWindow(), true);
  ImGui_ImplOpenGL3_Init();
}

void ImGuiHelper::startFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiHelper::endFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void ImGuiHelper::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiHelper::show() {
  // ImGui::ShowDemoWindow();

  showMenuBar();

  showEntityEditorWindow();
  showFramerateWidget();

  showProfilerWindow();
}

void ImGuiHelper::showMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Show Profiler")) {
        if (showProfiler)
          ImGui::SetWindowFocus("Profiler");
        else
          showProfiler = true;
      }

      if (ImGui::MenuItem("Show Entity Editor")) {
        if (showEntityEditor)
          ImGui::SetWindowFocus("Entity Editor");
        else
          showEntityEditor = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void ImGuiHelper::showFramerateWidget() {
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

  const ImGuiViewport* viewport = ImGui::GetMainViewport();

  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;
  ImVec2 window_pos, window_pos_pivot;

  window_pos.x = work_pos.x + work_size.x - PAD;
  window_pos.y = work_pos.y + PAD;

  window_pos_pivot.x = 1.0f;
  window_pos_pivot.y = 0.0f;

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  ImGui::Begin("Statistics", NULL, window_flags);
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::Text("Clock: %.2f", Application::getInstance()->getWallTime());

  ImGui::End();
}

void GameObjectEditor::draw() {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Actions")) {
      ImGui::MenuItem("Insert Object");
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  ///////////////
  // LEFT SIDE //
  ///////////////

  if (ImGui::BeginChild("##tree", ImVec2(120, 0),
                        ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders |
                            ImGuiChildFlags_NavFlattened)) {
    if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg)) {
      for (GameObject* gameObject :
           Application::getInstance()->getWorld()->getGameObjects()) {
        drawGameObject(gameObject);
      }

      ImGui::EndTable();
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();

  ////////////////
  // RIGHT SIDE //
  ////////////////

  ImGui::BeginChild("##properties", ImVec2(0, 0), false,
                    ImGuiWindowFlags_HorizontalScrollbar);

  ImGui::BeginGroup();

  if (focusedGameObject) {
    ImGui::Text("Properties of %s", focusedGameObject->getName().c_str());
    ImGui::Separator();

    // ImGui::Checkbox("Visible", focusedGameObject->getVisiblePointer());

    // if (focusedGameObject->getType() == GameObjectType::SKELETON)
    //{
    //	Skeleton* skeleton = dynamic_cast<Skeleton*>(focusedGameObject);

    //	if (skeleton->getAnimation())
    //		ImGui::Checkbox("Animate", skeleton->getAnimatePointer());
    //}
    // else if (focusedGameObject->getType() == GameObjectType::CLOTH)
    //{
    //	Cloth* cloth = dynamic_cast<Cloth*>(focusedGameObject);

    //	ImGui::PushItemWidth(80);
    //	ImGui::InputFloat("Spring Constant", cloth->getSpringConstantPointer());
    //	ImGui::InputFloat("Damping Constant",
    // cloth->getDampingConstantPointer()); 	ImGui::InputFloat("Wind
    // Velocity", cloth->getWindVelocityPointer());
    // ImGui::InputFloat("Simulation Timestep",
    // cloth->getFixedDeltaTimePointer()); 	ImGui::PopItemWidth();

    //	if (ImGui::Button("Reset"))
    //	{
    //		cloth->reset();
    //	}
    //}
    // else if (focusedGameObject->getType() ==
    // GameObjectType::PARTICLE_EMITTER)
    //{
    //	ParticleEmitter* particleEmitter =
    // dynamic_cast<ParticleEmitter*>(focusedGameObject);

    //	// ImGui::PushItemWidth(90);
    //	ImGui::Text("Particle count: %d", particleEmitter->getParticleCount());
    //	ImGui::InputInt("creation rate",
    // particleEmitter->getParticleCreationRatePointer());
    //	ImGui::SliderFloat3("position",
    // particleEmitter->getInitialPositionPointer()->ptr(), -10.0f, 10.0f,
    //"%.2f"); 	ImGui::SliderFloat3("position.variance",
    // particleEmitter->getInitialPositionVariancePointer()->ptr(),
    //-10.0f, 10.0f, "%.2f"); 	ImGui::SliderFloat3("velocity",
    // particleEmitter->getInitialVelocityPointer()->ptr(), -10.0f, 10.0f,
    //"%.2f"); 	ImGui::SliderFloat3("velocity.variance",
    // particleEmitter->getInitialVelocityVariancePointer()->ptr(),
    //-10.0f, 10.0f, "%.2f"); 	ImGui::SliderFloat("lifespan",
    // particleEmitter->getInitialLifeSpanPointer(), 0.0f, 10.0f, "%.2f");
    //	ImGui::SliderFloat("lifespan.variance",
    // particleEmitter->getInitialLifeSpanVariancePointer(), 0.0f, 10.0f,
    //"%.2f"); 	ImGui::SliderFloat("radius",
    // particleEmitter->getParticleRadiusPointer(), 0.0f, 1.0f, "%.2f");
    //	ImGui::SliderFloat("radius.variance",
    // particleEmitter->getParticleRadiusVariancePointer(), 0.0f, 1.0f, "%.2f");
    //	ImGui::SliderFloat("radius.lower bound",
    // particleEmitter->getParticleRadiusLowerBoundPointer(), 0.0f, 1.0f,
    //"%.2f");

    //	ImGui::ColorEdit4("color",
    // particleEmitter->getParticleColorPointer()->ptr());
    //	ImGui::SliderFloat("color.red variance",
    // particleEmitter->getParticleColorRedVariancePointer(), 0.0f, 1.0f,
    //"%.2f"); 	ImGui::SliderFloat("color.green variance",
    // particleEmitter->getParticleColorGreenVariancePointer(), 0.0f, 1.0f,
    //"%.2f"); 	ImGui::SliderFloat("color.blue variance",
    // particleEmitter->getParticleColorBlueVariancePointer(), 0.0f, 1.0f,
    //"%.2f"); 	ImGui::SliderFloat("color.alpha variance",
    // particleEmitter->getParticleColorAlphaVariancePointer(), 0.0f, 1.0f,
    //"%.2f"); 	ImGui::SliderFloat("color.uniform variance",
    // particleEmitter->getParticleColorUniformVariancePointer(), 0.0f, 1.0f,
    //"%.2f");

    //	ImGui::SliderFloat3("gravity",
    // particleEmitter->getGravityPointer()->ptr(), -10.0f, 10.0f, "%.2f");
    //	ImGui::SliderFloat("air density",
    // particleEmitter->getAirDensityPointer(), 0.0f, 1000.0f, "%.2f");
    //	ImGui::SliderFloat("drag coefficient",
    // particleEmitter->getDragCoefficientPointer(), 0.0f, 1.0f, "%.2f");
    //	ImGui::SliderFloat("collision elasticity",
    // particleEmitter->getCollisionElasticityPointer(), 0.0f, 1.0f, "%.2f");
    //	ImGui::SliderFloat("collision friction",
    // particleEmitter->getCollisionFrictionPointer(), 0.0f, 2.0f, "%.2f");

    //	// ImGui::PopItemWidth();

    //	if (ImGui::Button("Reset"))
    //	{
    //		particleEmitter->reset();
    //	}
    //}

    if (focusedGameObject->hasComponent<RigidBodyComponent>()) {
      TransformComponent& rigidBody =
          focusedGameObject->getComponent<TransformComponent>();

      ImGui::Text("HAS RIGID BODY COMPONENT");

      //	TransformComponent &transform =
      // focusedGameObject->getComponent<TransformComponent>();

      //	// POSITION
      //	ImGui::Separator();
      //	ImGui::Text("Position");
      //	ImGui::PushItemWidth(-24);
      //	ImGui::SliderFloat("X##position", &transform.position.x,
      //-10.0f, 10.0f, "%.2f"); 	ImGui::SliderFloat("Y##position",
      //&transform.position.y, -10.0f, 10.0f, "%.2f");
      //	ImGui::SliderFloat("Z##position", &transform.position.z,
      //-10.0f, 10.0f, "%.2f"); 	ImGui::PopItemWidth();

      //	// ROTATION
      //	ImGui::Separator();
      //	ImGui::Text("Rotation");
      //	ImGui::PushItemWidth(-24);
      //	ImGui::SliderFloat("X##rotation", &transform.rotation.x,
      //-10.0f, 10.0f, "%.2f"); 	ImGui::SliderFloat("Y##rotation",
      //&transform.rotation.y, -10.0f, 10.0f, "%.2f");
      //	ImGui::SliderFloat("Z##rotation", &transform.rotation.z,
      //-10.0f, 10.0f, "%.2f"); 	ImGui::PopItemWidth();

      //	// SCALE
      //	ImGui::Separator();
      //	ImGui::Text("Scale");
      //	ImGui::PushItemWidth(-24);
      //	ImGui::SliderFloat("X##scale", &transform.scale.x, 0.1f, 10.0f,
      //"%.2f"); 	ImGui::SliderFloat("Y##scale", &transform.scale.y,
      // 0.1f, 10.0f,
      //"%.2f"); 	ImGui::SliderFloat("Z##scale", &transform.scale.z,
      // 0.1f, 10.0f,
      //"%.2f"); 	ImGui::PopItemWidth();
    }
  }

  ImGui::EndGroup();

  ImGui::EndChild();
}

void GameObjectEditor::drawGameObject(GameObject* gameObject) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::PushID(gameObject->getID());

  ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_None;
  treeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
  treeFlags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;

  if (gameObject == focusedGameObject) treeFlags |= ImGuiTreeNodeFlags_Selected;

  // if (gameObject->getType() == GameObjectType::JOINT)
  //{
  //	if (dynamic_cast<Joint*>(gameObject)->getChildren().size() == 0)
  //		treeFlags |= ImGuiTreeNodeFlags_Leaf;
  // }
  // else
  //{
  treeFlags |= ImGuiTreeNodeFlags_Leaf;
  //}

  // format string "name (id)"
  std::string nodeLabel =
      gameObject->getName() + " (" + std::to_string(gameObject->getID()) + ")";
  bool node_open = ImGui::TreeNodeEx(nodeLabel.c_str(), treeFlags);

  if (ImGui::IsItemClicked()) {
    if (focusedGameObject == gameObject)
      focusedGameObject = NULL;
    else
      focusedGameObject = gameObject;
  }

  if (node_open) {
    // if (gameObject->getType() == GameObjectType::SKELETON)
    //{
    //	Skeleton* skeleton = dynamic_cast<Skeleton*>(gameObject);
    //	Joint* rootJoint = skeleton->getRootJoint();

    //	drawGameObject(rootJoint);
    //}
    // else if (gameObject->getType() == GameObjectType::JOINT)
    //{
    //	Joint* joint = dynamic_cast<Joint*>(gameObject);

    //	for (Joint* child : joint->getChildren())
    //		drawGameObject(child);
    //}

    ImGui::TreePop();  // This is required at the end of the if block
  }

  ImGui::PopID();
}

void ImGuiHelper::showEntityEditorWindow() {
  if (!showEntityEditor) return;

  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
  windowFlags |= ImGuiWindowFlags_MenuBar;

  ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

  if (!ImGui::Begin("Entity Editor", &showEntityEditor, windowFlags)) {
    ImGui::End();
    return;
  }

  entityEditor.draw();

  ImGui::End();
};

static void buildProfilerTree(int sectionID,
                              std::vector<Neu::ProfilerSection*>& section) {
  for (int childSectionID : section[sectionID]->children) {
    ImGui::Indent();
    ImGui::Text(
        "%s: %.3f ms", section[childSectionID]->name.c_str(),
        Neu::Profiler::getInstance().getTimeForSection(childSectionID).count() *
            1000);
    buildProfilerTree(childSectionID, section);
    ImGui::Unindent();
  }
}

void ImGuiHelper::showProfilerWindow() {
  if (!showProfiler) return;

  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

  if (!ImGui::Begin("Profiler", &showProfiler, windowFlags)) {
    ImGui::End();
    return;
  }

  std::vector<Neu::ProfilerSection*> profilerData =
      Profiler::getInstance().getProfilerData();

  if (profilerData.size() == 0) {
    ImGui::Text("No profiler data available");
  } else {
    ImGui::Text("%s: %.3f ms", profilerData[0]->name.c_str(),
                Profiler::getInstance().getTimeForSection(0).count() * 1000);
    buildProfilerTree(0, profilerData);
  }

  ImGui::End();
}