#pragma once

#include "mesh.h"
#include <vector>
#include <cmath>
#include <memory>

// Generates a sphere Mesh with position + normal + texCoord.
// Caller must have an active OpenGL context before calling setup() on the returned Mesh.
inline std::unique_ptr<Mesh> generateSphere(unsigned int xSegments = 64, unsigned int ySegments = 64)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;
    for (unsigned int y = 0; y <= ySegments; ++y)
    {
        for (unsigned int x = 0; x <= xSegments; ++x)
        {
            float xSegment = static_cast<float>(x) / xSegments;
            float ySegment = static_cast<float>(y) / ySegments;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            vertices.emplace_back(
                QVector3D(xPos, yPos, zPos),
                QVector3D(xPos, yPos, zPos),
                QVector2D(xSegment, ySegment)
            );
        }
    }

    // Generate triangle list indices (3 indices per triangle)
    for (unsigned int y = 0; y < ySegments; ++y)
    {
        for (unsigned int x = 0; x < xSegments; ++x)
        {
            unsigned int topLeft     = y       * (xSegments + 1) + x;
            unsigned int topRight    = y       * (xSegments + 1) + x + 1;
            unsigned int bottomLeft  = (y + 1) * (xSegments + 1) + x;
            unsigned int bottomRight = (y + 1) * (xSegments + 1) + x + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return std::make_unique<Mesh>(vertices, indices);
}
