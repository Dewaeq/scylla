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

![IMG20250826001707](https://github.com/user-attachments/assets/f8900b40-c287-48ae-af7e-b43ed75dfbe4)
![IMG20250826175657](https://github.com/user-attachments/assets/efdc6d15-8abf-4d0d-a6e1-cf2d23907e97)
