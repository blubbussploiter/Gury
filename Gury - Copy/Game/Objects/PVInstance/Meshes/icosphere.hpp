/// Seed of Andromeda Icosphere Generator
/// Written by Frank McCoy
/// Use it for whatever, but remember where you got it from.
///
/// SEE: https://www.seedofandromeda.com/blogs/49-procedural-gas-giant-rendering-with-gpu-noise
/// SEE: http://pastebin.com/aFdWi5eQ

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <G3DAll.h>
// GLM is awesome ^ ^ ^

const static float GOLDEN_RATIO = 1.61803398875f;

const static int NUM_ICOSOHEDRON_VERTICES = 12;
const static Vector3 ICOSOHEDRON_VERTICES[12] = {
    Vector3(-1.0f, GOLDEN_RATIO, 0.0f),
    Vector3(1.0f, GOLDEN_RATIO, 0.0f),
    Vector3(-1.0f, -GOLDEN_RATIO, 0.0f),
    Vector3(1.0f, -GOLDEN_RATIO, 0.0f),

    Vector3(0.0f, -1.0f, GOLDEN_RATIO),
    Vector3(0.0f, 1.0f, GOLDEN_RATIO),
    Vector3(0.0f, -1.0, -GOLDEN_RATIO),
    Vector3(0.0f, 1.0f, -GOLDEN_RATIO),

    Vector3(GOLDEN_RATIO, 0.0f, -1.0f),
    Vector3(GOLDEN_RATIO, 0.0f, 1.0f),
    Vector3(-GOLDEN_RATIO, 0.0f, -1.0f),
    Vector3(-GOLDEN_RATIO, 0.0, 1.0f)
};

const static int NUM_ICOSOHEDRON_INDICES = 60;
const static uint32_t ICOSOHEDRON_INDICES[60] = {
    0, 11, 5,
    0, 5, 1,
    0, 1, 7,
    0, 7, 10,
    0, 10, 11,

    1, 5, 9,
    5, 11, 4,
    11, 10, 2,
    10, 7, 6,
    7, 1, 8,

    3, 9, 4,
    3, 4, 2,
    3, 2, 6,
    3, 6, 8,
    3, 8, 9,

    4, 9, 5,
    2, 4, 11,
    6, 2, 10,
    8, 6, 7,
    9, 8, 1
};

// Hash functions for the unordered map
class Vec3KeyFuncs {
public:
    size_t operator()(const Vector3& k)const {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const Vector3& a, const Vector3& b)const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

inline Vector3 findMidpoint(Vector3 vertex1, Vector3 vertex2) {
    return normalize(Vector3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
}

/// Generates an icosphere with radius 1.0f.
/// @param lod: Number of subdivisions
/// @param indices: Resulting indices for use with glDrawElements
/// @param positions: Resulting vertex positions
void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<Vector3>& positions) {
    std::vector<uint32_t> newIndices;
    newIndices.reserve(256);

    std::unordered_map<Vector3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;

    indices.resize(NUM_ICOSOHEDRON_INDICES);
    for (uint32_t i = 0; i < NUM_ICOSOHEDRON_INDICES; i++) {
        indices[i] = ICOSOHEDRON_INDICES[i];
    }
    positions.resize(NUM_ICOSOHEDRON_VERTICES);
    for (uint32_t i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++) {
        positions[i] = normalize(ICOSOHEDRON_VERTICES[i]);
        vertexLookup[normalize(ICOSOHEDRON_VERTICES[i])] = i;
    }

    for (size_t i = 0; i < (size_t)lod; i++) {
        for (size_t j = 0; j < indices.size(); j += 3) {
            /*
            j
            mp12   mp13
            j+1    mp23   j+2
            */
            // Defined in counter clockwise order
            Vector3 vertex1 = positions[indices[j + 0]];
            Vector3 vertex2 = positions[indices[j + 1]];
            Vector3 vertex3 = positions[indices[j + 2]];

            Vector3 midPoint12 = findMidpoint(vertex1, vertex2);
            Vector3 midPoint23 = findMidpoint(vertex2, vertex3);
            Vector3 midPoint13 = findMidpoint(vertex1, vertex3);

            uint32_t mp12Index;
            uint32_t mp23Index;
            uint32_t mp13Index;

            auto iter = vertexLookup.find(midPoint12);
            if (iter != vertexLookup.end()) { // It is in the map
                mp12Index = iter->second;
            }
            else { // Not in the map
                mp12Index = (uint32_t)positions.size();
                positions.push_back(midPoint12);
                vertexLookup[midPoint12] = mp12Index;
            }

            iter = vertexLookup.find(midPoint23);
            if (iter != vertexLookup.end()) { // It is in the map
                mp23Index = iter->second;
            }
            else { // Not in the map
                mp23Index = (uint32_t)positions.size();
                positions.push_back(midPoint23);
                vertexLookup[midPoint23] = mp23Index;
            }

            iter = vertexLookup.find(midPoint13);
            if (iter != vertexLookup.end()) { // It is in the map
                mp13Index = iter->second;
            }
            else { // Not in the map
                mp13Index = (uint32_t)positions.size();
                positions.push_back(midPoint13);
                vertexLookup[midPoint13] = mp13Index;
            }

            newIndices.push_back(indices[j]);
            newIndices.push_back(mp12Index);
            newIndices.push_back(mp13Index);

            newIndices.push_back(mp12Index);
            newIndices.push_back(indices[j + 1]);
            newIndices.push_back(mp23Index);

            newIndices.push_back(mp13Index);
            newIndices.push_back(mp23Index);
            newIndices.push_back(indices[j + 2]);

            newIndices.push_back(mp12Index);
            newIndices.push_back(mp23Index);
            newIndices.push_back(mp13Index);
        }
        indices.swap(newIndices);
        newIndices.clear();
    }

}

void reorder(float radius, std::vector<uint32_t>& indices, std::vector<Vector3>& positions, std::vector<Vector3>& positions_out)
{
    for (size_t i = 0; i < indices.size(); i++)
    {
        uint32 index = indices[i];

        positions_out.push_back(positions[index] * radius);
    }
}