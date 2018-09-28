

#include <string>
#include <vector>
#include "Shared.h"
#include "Window.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/hash.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

#pragma once


namespace std {
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

//mesh data for 3D rendering
struct Mesh
{
	//Data for mesh (vertices, indices and material)
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	//const std::vector<Vertex> vertices = {
	//	{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
	//{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
	//{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f } },
	//{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f } }
	//};
	//
	//const std::vector<uint16_t> indices = {
	//	0, 1, 2, 2, 3, 0
	//	//4, 5, 6, 6, 7, 4
	//};
	//**Material to be added at later time **
	//ModelMaterial material;

	//buffers for mesh
	vk::wrappers::Buffer vertexBuffer;
	vk::wrappers::Buffer indicesBuffer;


	//Method to load data into vertices datastructure
	void LoadMeshFromFile(std::string modelPath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str())) {
			throw std::runtime_error(err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex = {};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
};

