# Autonomous car made with RPLidar A1 and RPI3

## Roadmap
- [x] Read lidar data
- [x] Preprocess data (e.g. range cutoff and voxel filter)
- [x] Create controller
- [ ] Pose estimation
    - [ ] Simple velocity based guess
    - [ ] Odometry
- [x] Simple local map (occupancy grid)
- [ ] Ceres scan matching
    - [ ] Cost function
    - [ ] Residuals
    - [ ] Setup parameters (x, y, theta)
- [ ] Motion filter
- [ ] Submap insertion
- [ ] Create/store keyframes
- [ ] Pose graph optimization
    - [ ] Pose vertices
    - [ ] Submap vertices
    - [ ] Pose <-> submap edges
    - [ ] Submap <-> submap edges (loop closure)
    - [ ] Loop closure

