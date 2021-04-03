#include <enpch.hpp>

#include "MD5MeshLoader.hpp"

namespace Engine {

    namespace md5 {

        bool startsWith(const std::string& str, const std::string& substr) {
            return strncmp(str.c_str(), substr.c_str(), substr.size()) == 0;
        }

        void CalcQuatW(math::quat& quat) {
            float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
            if (t < 0.0f)
            {
                quat.w = 0.0f;
            }
            else
            {
                //quat.w = -sqrtf(t); // TODO: why is this negative????
                quat.w = sqrtf(t);
            }
        }

        void PrepareMesh(Model* model, Mesh& mesh);
        void PrepareNormals(Model* model, Mesh& mesh);

        bool LoadMD5MeshFile(const std::string& filename, Model* model) {
            if (model == nullptr) return false;

            // get file pointer
            std::ifstream md5File(filename);

            if (md5File) {
                // read param by param
                std::string param, garb;
                md5File >> param;

                while (!md5File.eof()) {
                    if (param == "MD5Version") {
                        int md5Version;
                        md5File >> md5Version;
                        assert(md5Version == 10);
                    }
                    else if (param == "commandline") {
                        std::getline(md5File, garb);
                    }
                    else if (param == "numJoints") {
                        md5File >> model->numJoints;
                        assert(model->numJoints > 0);
                        model->Joints.reserve(model->numJoints);
                    }
                    else if (param == "numMeshes") {
                        md5File >> model->numMeshes;
                        assert(model->numMeshes > 0);
                        model->Meshes.reserve(model->numMeshes);
                    }
                    else if (param == "joints") {
                        assert(model->numJoints > 0);
                        std::getline(md5File, garb); // go to next line
                        Joint joint;
                        for (int n = 0; n < model->numJoints; n++) {
                            md5File >> garb;
                            joint.name = garb.substr(1, garb.size() - 2);
                            md5File >> joint.parent >>
                                garb >> joint.position.x >> joint.position.y >> joint.position.z >> garb >>
                                garb >> joint.orientation.x >> joint.orientation.y >> joint.orientation.z >> garb;
                            CalcQuatW(joint.orientation);

                            model->Joints.push_back(joint);
                            
                            std::getline(md5File, garb);
                        }
                        md5File >> garb;
                    }
                    else if (param == "mesh") {
                        assert(model->numMeshes > 0);
                        Mesh mesh;
                        int numVerts, numTris, numWeights;

                        md5File >> garb;
                        md5File >> param;
                        while (param != "}") {
                            if (param == "shader") {
                                md5File >> garb;
                                mesh.Shader = garb.substr(1, garb.size() - 2);
                                std::getline(md5File, garb);
                            }
                            else if (param == "numverts") {
                                md5File >> numVerts;
                                mesh.Verts.reserve(numVerts);
                                std::getline(md5File, garb);

                                for (int n = 0; n < numVerts; n++) {
                                    Vert vert;
                                    int index;
                                    md5File >> garb >> index;
                                    assert(n == index);
                                    md5File >> garb >> vert.uv.x >> vert.uv.y >> garb;
                                    md5File >> vert.startWeight >> vert.countWeight;

                                    mesh.Verts.push_back(vert);
                                }
                            }
                            else if (param == "numtris") {
                                md5File >> numTris;
                                mesh.Tris.reserve(numTris);
                                std::getline(md5File, garb);

                                for (int n = 0; n < numTris; n++) {
                                    Tri tri;
                                    int index;
                                    md5File >> garb >> index;
                                    assert(n == index);
                                    md5File >> tri.vertIndex[0] >> tri.vertIndex[1] >> tri.vertIndex[2];

                                    mesh.Tris.push_back(tri);
                                }
                            }
                            else if (param == "numweights") {
                                md5File >> numWeights;
                                mesh.Weights.reserve(numWeights);
                                std::getline(md5File, garb);

                                for (int n = 0; n < numWeights; n++) {
                                    Weight weight;
                                    int index;
                                    md5File >> garb >> index;
                                    assert(n == index);
                                    md5File >> weight.joint >> weight.bias;
                                    md5File >> garb >> weight.pos.x >> weight.pos.y >> weight.pos.z >> garb;

                                    std::getline(md5File, garb);

                                    mesh.Weights.push_back(weight);
                                }
                            }
                            else {
                                std::getline(md5File, garb);
                            }

                            md5File >> param;
                        }
                        PrepareMesh(model, mesh);
                        PrepareNormals(model, mesh);

                        model->Meshes.push_back(mesh);
                    }
                    md5File >> param;
                }
                assert(model->numJoints == model->Joints.size());
                assert(model->numMeshes == model->Meshes.size());

                md5File.close();
                return true;
            }

            return false;
        }


        void PrepareMesh(Model* model, Mesh& mesh) {
            for (int n = 0; n < mesh.Verts.size(); n++) {
                Vert& vert = mesh.Verts[n];

                for (int j = 0; j < vert.countWeight; j++) {
                    Weight& weight = mesh.Weights[vert.startWeight + j];
                    Joint& joint = model->Joints[weight.joint];

                    // Convert the weight position from Joint local space to object space
                    math::vec3 rotPos = joint.orientation * weight.pos;

                    vert.position += (joint.position + rotPos) * weight.bias;
                }
            }
        }

        void PrepareNormals(Model* model, Mesh& mesh) {
            // calcualte normal for each triangle face
            for (int n = 0; n < mesh.Tris.size(); n++) {
                math::vec3 v0 = mesh.Verts[mesh.Tris[n].vertIndex[0]].position;
                math::vec3 v1 = mesh.Verts[mesh.Tris[n].vertIndex[1]].position;
                math::vec3 v2 = mesh.Verts[mesh.Tris[n].vertIndex[2]].position;

                math::vec3 normal = (v2 - v0).cross(v1 - v0);

                mesh.Verts[mesh.Tris[n].vertIndex[0]].normal += normal;
                mesh.Verts[mesh.Tris[n].vertIndex[1]].normal += normal;
                mesh.Verts[mesh.Tris[n].vertIndex[2]].normal += normal;
            }

            // normal normals(getting a per-vertex smoothed normal)

            for (int n = 0; n < mesh.Verts.size(); n++) {
                Vert& vert = mesh.Verts[n];

                math::vec3 normal = vert.normal.normalize();

                // reset normal to find bind-pose normal in joint space?
                vert.normal = math::vec3(0);

                // put the bind-pose normal into joint-local space
                // so the animated normal can be computed faster later
                for (int j = 0; j < vert.countWeight; j++) {
                    const Weight& weight = mesh.Weights[vert.startWeight + j];
                    const Joint& joint = model->Joints[weight.joint];
                    vert.normal += (normal * joint.orientation) * weight.bias;
                }
            }
        }


        ///////////////////////////////////////
        // MD5Material ////////////////////////
        ///////////////////////////////////////

        bool LoadMD5MaterialDefinitionFile(const std::string& filename, std::unordered_map<std::string, Material>& map) {
            // get file pointer
            std::ifstream file(filename);

            if (file) {
                // read param by param
                std::string param, garb;
                file >> param;
                int numMaterials;

                while (!file.eof()) {
                    if (param == "MD5Version") {
                        int md5Version;
                        file >> md5Version;
                        assert(md5Version == 10);
                    }
                    else if (param == "commandline") {
                        std::getline(file, garb);
                    }
                    else if (param == "numMaterials") {
                        file >> numMaterials;
                        assert(numMaterials > 0);
                    }
                    else if (param == "material") {
                        assert(numMaterials > 0);
                        file >> garb;
                        file >> param;

                        Material mat;
                        while (param != "}") {
                            if (param == "shader") {
                                file >> garb; // "shader"
                                mat.name = garb.substr(1, garb.size() - 2);
                                std::getline(file, garb);
                            }
                            else if (param == "diffusemap") {
                                file >> garb; //"path/to/image.ext"
                                mat.diffusemap = garb.substr(1, garb.size() - 2);
                                std::getline(file, garb);
                            }
                            else if (param == "normalmap") {
                                file >> garb; //"path/to/image.ext"
                                mat.normalmap = garb.substr(1, garb.size() - 2);
                                std::getline(file, garb);
                            }
                            else if (param == "specularmap") {
                                file >> garb; //"path/to/image.ext"
                                mat.specularmap = garb.substr(1, garb.size() - 2);
                                std::getline(file, garb);
                            }
                            else {
                                std::getline(file, garb);
                            }

                            file >> param;
                        }

                        if (map.find(mat.name) == map.end()) {
                            // new material definition
                            map.emplace(mat.name, mat);
                        }
                    }
                    
                    file >> param;
                }

                file.close();
            }

            return false;
        }
    }
}