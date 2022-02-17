/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define TINYOBJLOADER_IMPLEMENTATION

#define GLM_ENABLE_EXPERIMENTAL
#include "mesh.hpp"

#include <glm/gtx/hash.hpp>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace std
{
    template <>
    struct hash<plaincraft_render_engine::Vertex>
    {
        size_t operator()(plaincraft_render_engine::Vertex const &vertex) const
        {
            size_t seed = 0;
            plaincraft_common::hash_combine(seed, vertex.position, vertex.normal, vertex.color, vertex.text_coordinates);
            return seed;
        }
    };
}

namespace plaincraft_render_engine
{
    Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> indices)
        : vertices_(std::move(vertices)), indices_(std::move(indices))
    {
    }

    Mesh::Mesh(Mesh &&other) : vertices_(std::move(other.vertices_)), indices_(std::move(other.indices_))
    {
    }

    Mesh &Mesh::operator=(Mesh &&other)
    {
        if (&other == this)
        {
            return *this;
        }

        this->vertices_ = std::move(other.vertices_);
        this->indices_ = std::move(other.indices_);

        return *this;
    }

    std::unique_ptr<Mesh> Mesh::LoadWavefront(const char *data)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warning, error;

        std::stringstream data_stream(data);

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, &data_stream))
        {
            throw std::runtime_error(warning + error);
        }

        Mesh mesh;
        mesh.vertices_.clear();
        mesh.indices_.clear();

        std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0)
                {
                    vertex.text_coordinates = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(mesh.vertices_.size());
                    mesh.vertices_.push_back(vertex);
                }
                mesh.indices_.push_back(unique_vertices[vertex]);
            }
        }

        return std::make_unique<Mesh>(std::move(mesh));
    }
}