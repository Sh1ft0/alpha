/**
Copyright 2014-2015 Jason R. Wendlandt

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics/Model.h"
#include "Graphics/Mesh.h"

/**
 * Create a Mesh for each aiMesh
 */
alpha::Mesh processMesh(aiMesh * mesh, const aiScene * /*scene*/)
{
    printf("parsing a mesh.\r\n");

    std::vector<alpha::Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        alpha::Vertex v;

        // process vertex positions
        printf("%f, %f, %f,\r\n", mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // load mesh vertex position
        v.position.x = mesh->mVertices[i].x;
        v.position.y = mesh->mVertices[i].y;
        v.position.z = mesh->mVertices[i].z;

        // load mesh normal coordinates
        v.normal.x = mesh->mNormals[i].x;
        v.normal.y = mesh->mNormals[i].y;
        v.normal.z = mesh->mNormals[i].z;

        vertices.push_back(v);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int k = 0; k < face.mNumIndices; ++k)
        {
            printf("%u, ", face.mIndices[k]);
            indices.push_back(face.mIndices[k]);
        }
        printf("\r\n");
    }
    
    // process material
    
    printf("done parsing mesh\r\n");

    return alpha::Mesh(vertices, indices);
}

/**
 * Process all meshes at this node, an recurse through child nodes.
 */
void processNode(aiNode * node, const aiScene * scene, std::vector<alpha::Mesh> meshes)
{
    printf("parsing a node\r\n");

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, meshes);
    }

    printf("done parsing node.\r\n");
}

/**
 * Import the model path provided by the user, and process the root node.
 */
int main(int /*argc*/, char * argv[])
{
    // assume second argument is a path to a model
    char * path = argv[1];
    printf("Importing model: %s\r\n", path);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Error !! %s\r\n", importer.GetErrorString());
    }
    else
    {
        printf("Loading model ...\r\n");
        std::vector<alpha::Mesh> meshes;
        processNode(scene->mRootNode, scene, meshes);
        alpha::Model mod(meshes);
    }

    return 0;
}
