# Visualization
The Project named "Visualization" is the code for implementing the scale-free 3D terrain visualization base on detail-increment model. The code was written in C++ programming language. And it uses the OpenSceneGraph engine to achieve 3D representation of terrain. The development platform for the project is Microsoft Visual Studio 2010. The version of OpenSceneGraph used in this project is 3.2.1. Before use the code, you need to configure the library environment for OpenSceneGraph and Pthread.

# File description
The code consists of a set of classes in Tin11.cpp which implement the algorithm. And the test data 1400.txt provides the corrdinates
 of 1400 points for experiment. The primary development system was windows10, and the testing platform was AMD Radeon R7 430 and Intel i7-6700. The algorithm is implemented in C++ and visualized with the OpenSceneGraph engine.

# Algorithm description
The program first constructs the triangulated irregular network based on point cloud data. And the network is divided into four parts. The gaps of four sub-blocks are connected to get the complete mesh. Then, the edge collapsing operation is performed based on the complete mesh, and the incremental information is stored. Finally, the mesh model of arbitrary scale can be reconstructed with incremental information.
