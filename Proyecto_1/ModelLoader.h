#pragma once

#include "tinygltf/tiny_gltf.h"
#include <string>

bool LoadGLTF(const std::string& path,
    tinygltf::Model& model);
