# neu

> [!IMPORTANT]
> This project is a work-in-progress. The engine is currently adopting an entity-component system and is missing several features that are being re-implemented.

Neu aims to be a performant, data-driven 3D C++ rendering engine.

# Features

- [x] Mesh Skeletons
- [x] Skeletal Animation
- [x] Cloth Simulation
- [x] Particle Systems
- [x] Rigid Body Physics (Jolt Physics)
- [x] Forward Renderer
- [x] Multiple Lights
- [x] Real-Time Profiler

## Features In Progress

- [ ] Entity-Component System
- [ ] Materials/Textures
- [ ] Physically-Based Rendering
- [ ] Shadow Mapping
- [ ] Asset Import
- [ ] Real-Time Entity Editor (Previously Game Object Editor)
- [ ] API Documentation and Samples
- [ ] Post-Processing Effects

# Libraries

Neu relies on the following libraries:

- Glad
- GLFW
- Jolt Physics
- ImGui

# Screenshots

![Rigid Bodies with Visible AABB](/assets/img/neu_refactor.png)

![Imported Mesh with Skeleton and Multiple Lights](/assets/img/neu_multiple_lights.png)
