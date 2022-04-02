#include <enpch.hpp>

#include "MD5MeshLoader.hpp"

namespace Engine {

    namespace md5 {

        struct Vert_File {
            math::vec2 uv;
            int startWeight;
            int countWeight;
        };
        struct Weight {
            int joint;
            float bias; // [0.0 1.0]
            math::vec3 pos;
        };

        void PrepareMesh(Model* model, Mesh& mesh, const std::vector<Vert_File>& fileVerts, const std::vector<Weight>& fileWeights);
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
                        std::getline(md5File, garb);
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
                            joint.orientation.reconstructW();

                            math::mat4 trans;
                            math::CreateTransform(trans, joint.orientation, joint.position, math::vec3(1));
                            math::CreateViewFromTransform(joint.invTransform, trans);

                            model->Joints.push_back(joint);
                            
                            std::getline(md5File, garb);
                        }
                        md5File >> garb;
                    }
                    else if (param == "mesh") {
                        assert(model->numMeshes > 0);
                        Mesh mesh;
                        int numVerts, numTris, numWeights;

                        std::vector<Vert_File> vertsFromFile;
                        std::vector<Weight> weightsFromFile;

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
                                vertsFromFile.reserve(numVerts);
                                std::getline(md5File, garb);

                                for (int n = 0; n < numVerts; n++) {
                                    Vert_File vert;
                                    int index;
                                    md5File >> garb >> index;
                                    assert(n == index);
                                    md5File >> garb >> vert.uv.x >> vert.uv.y >> garb;
                                    md5File >> vert.startWeight >> vert.countWeight;

                                    vertsFromFile.push_back(vert);
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
                                weightsFromFile.reserve(numWeights);
                                std::getline(md5File, garb);

                                for (int n = 0; n < numWeights; n++) {
                                    Weight weight;
                                    int index;
                                    md5File >> garb >> index;
                                    assert(n == index);
                                    md5File >> weight.joint >> weight.bias;
                                    md5File >> garb >> weight.pos.x >> weight.pos.y >> weight.pos.z >> garb;

                                    std::getline(md5File, garb);

                                    weightsFromFile.push_back(weight);
                                }
                            }
                            else {
                                std::getline(md5File, garb);
                            }

                            md5File >> param;
                        }
                        PrepareMesh(model, mesh, vertsFromFile, weightsFromFile);
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


        void PrepareMesh(Model* model, Mesh& mesh, const std::vector<Vert_File>& fileVerts, const std::vector<Weight>& fileWeights) {
            mesh.Verts.resize(fileVerts.size());

            for (int n = 0; n < mesh.Verts.size(); n++) {
                const Vert_File& vert_info = fileVerts[n];
                Vert& vert = mesh.Verts[n];

                vert.uv = vert_info.uv;

                // calc transformed position
                int numWeights = std::min(4, vert_info.countWeight);
                for (int j = 0; j < numWeights; j++) {
                    const Weight& weight = fileWeights[vert_info.startWeight + j];
                    Joint& joint = model->Joints[weight.joint];

                    // Convert the weight position from Joint local space to object space
                    math::vec3 rotPos = math::TransformPointByQuaternion(joint.orientation, weight.pos);

                    vert.position += (joint.position + rotPos) * weight.bias;

                    vert.boneIndices[j] = weight.joint;
                    vert.boneWeights[j] = weight.bias;
                }
                //ENGINE_LOG_WARN("Mesh {0}, vertex {1} total weight: {2}", "mesh_name", n, total_weight_val);

                if (vert_info.countWeight > 4) {
                    // TODO: renormalize BEFORE we calculate the effect of each weight
                    ENGINE_LOG_WARN("Mesh {0}, vertex {1} has {2} weights. renormalizing to 4", "mesh_name_hehe", n, vert_info.countWeight);
                    float inv_sum = 1.0f / (vert.boneWeights[0] + vert.boneWeights[1] + vert.boneWeights[2] + vert.boneWeights[3]);
                    vert.boneWeights *= inv_sum;
                }
            }
        }

        void PrepareNormals(Model* model, Mesh& mesh) {
            math::vec3 *tan1 = new math::vec3[mesh.Verts.size() * 2]; // tangents
            math::vec3 *tan2 = tan1 + mesh.Verts.size();              // bitangents

            // calcualte normal/tangent/bitangent for each triangle face
            for (int n = 0; n < mesh.Tris.size(); n++) {
                const int i0 = mesh.Tris[n].vertIndex[0];
                const int i1 = mesh.Tris[n].vertIndex[1];
                const int i2 = mesh.Tris[n].vertIndex[2];

                const math::vec3& v0 = mesh.Verts[i0].position;
                const math::vec3& v1 = mesh.Verts[i1].position;
                const math::vec3& v2 = mesh.Verts[i2].position;

                const math::vec2& w0 = mesh.Verts[i0].uv;
                const math::vec2& w1 = mesh.Verts[i1].uv;
                const math::vec2& w2 = mesh.Verts[i2].uv;

                float x1 = v1.x - v0.x;
                float y1 = v1.y - v0.y;
                float z1 = v1.z - v0.z;
                float x2 = v2.x - v0.x;
                float y2 = v2.y - v0.y;
                float z2 = v2.z - v0.z;

                float s1 = w1.x - w0.x;
                float t1 = w1.y - w0.y;
                float s2 = w2.x - w0.x;
                float t2 = w2.y - w0.y;

                float r = 1.0f / (s1 * t2 - s2 * t1);

                math::vec3 sDir((t2*x1 - t1*x2)*r, (t2*y1 - t1*y2)*r, (t2*z1 - t1*z2)*r);
                math::vec3 tDir((s1*x2 - s2*x1)*r, (s1*y2 - s2*y1)*r, (s1*z2 - s2*z1)*r);

                tan1[i0] += sDir;
                tan1[i1] += sDir;
                tan1[i2] += sDir;

                tan2[i0] += tDir;
                tan2[i1] += tDir;
                tan2[i2] += tDir;

                math::vec3 normal = (v2 - v0).cross(v1 - v0); // TODO: hehe

                mesh.Verts[i0].normal += normal;
                mesh.Verts[i1].normal += normal;
                mesh.Verts[i2].normal += normal;
            }

            for (int n = 0; n < mesh.Verts.size(); n++) {
                const math::vec3& t = tan1[n];
                const math::vec3& b = tan2[n];
                Vert& vert = mesh.Verts[n];

                vert.normal.normalize();

                // calc handedness
                float w = vert.normal.cross(t).dot(b) < 0.0f ? -1.0f : 1.0f;

                // Gram-Schmidt orthagonalize
                vert.tangent = math::vec4((t - vert.normal*vert.normal.dot(t)).get_unit(), w);
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



        ///////////////////////////////////////
        // MD5Anim ////////////////////////////
        ///////////////////////////////////////

        bool LoadMD5AnimFile(const std::string& filename, Animation* anim) {
            if (anim == nullptr) return false;

            // get file pointer
            std::ifstream md5File(filename);

            if (md5File) {
                // read param by param
                std::string param, garb;
                md5File >> param;

                std::vector<BaseFrame>       BaseFrames;
                std::vector<FrameData>       Frames;

                while (!md5File.eof()) {
                    if (param == "MD5Version") {
                        int md5Version;
                        md5File >> md5Version;
                        assert(md5Version == 10);
                    }
                    else if (param == "commandline") {
                        std::getline(md5File, garb);
                    }
                    else if (param == "numFrames") {
                        md5File >> anim->numFrames;
                        assert(anim->numFrames > 0);
                        Frames.reserve(anim->numFrames);
                    }
                    else if (param == "numJoints") {
                        md5File >> anim->numJoints;
                        assert(anim->numJoints > 0);
                        anim->JointInfos.reserve(anim->numJoints);
                        BaseFrames.reserve(anim->numJoints);
                    }
                    else if (param == "frameRate") {
                        md5File >> anim->FrameRate;
                        assert(anim->FrameRate > 0);
                    }
                    else if (param == "numAnimatedComponents") {
                        md5File >> anim->numAnimComponents;
                        assert(anim->numAnimComponents > 0);
                    }
                    else if (param == "hierarchy") {
                        assert(anim->numJoints > 0);
                        std::getline(md5File, garb); // go to next line
                        JointInfo joint;
                        for (int n = 0; n < anim->numJoints; n++) {
                            md5File >> garb;
                            joint.name = garb.substr(1, garb.size() - 2);
                            md5File >> joint.parentID >> joint.flags >> joint.startIndex;
                            std::getline(md5File, garb);

                            anim->JointInfos.push_back(joint);
                        }
                        md5File >> garb;
                    }
                    else if (param == "bounds") {
                        md5File >> garb;
                        std::getline(md5File, garb);

                        for (int n = 0; n < anim->numFrames; n++) {
                            Bound bound;
                            md5File >> garb >> bound.min.x >> bound.min.y >> bound.min.z >> garb >>
                                garb >> bound.max.x >> bound.max.y >> bound.max.z >> garb;
                            anim->Bounds.push_back(bound);
                        }
                        md5File >> garb;
                        std::getline(md5File, garb);
                    }
                    else if (param == "baseframe") {
                        md5File >> garb;
                        std::getline(md5File, garb);

                        for (int n = 0; n < anim->numJoints; n++) {
                            BaseFrame frame;
                            md5File >> garb >> frame.position.x >> frame.position.y >> frame.position.z >> garb >>
                                garb >> frame.orientation.x >> frame.orientation.y >> frame.orientation.z >> garb;

                            frame.orientation.reconstructW();

                            BaseFrames.push_back(frame);
                        }
                        md5File >> garb;
                        std::getline(md5File, garb);
                    }
                    else if (param == "frame") {
                        FrameData frame;
                        int frameID;
                        md5File >> frameID >> garb;
                        std::getline(md5File, garb);

                        for (int n = 0; n < anim->numAnimComponents; n++) {
                            float data;
                            md5File >> data;
                            frame.frameData.push_back(data);
                        }

                        Frames.push_back(frame);

                        BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, BaseFrames, frame);

                        md5File >> garb;
                        std::getline(md5File, garb);
                    }

                    md5File >> param;
                }
                // alloc memory for animated skeleton
                anim->AnimatedSkeleton.Joints.assign(anim->numJoints, SkeletonJoint());
                
                anim->frameDuration = 1.0f / (float)anim->FrameRate;
                anim->animDuration = anim->frameDuration * (float)anim->numFrames;
                anim->animTime = 0.0f;

                assert(anim->numJoints == anim->JointInfos.size());
                assert(anim->numFrames == anim->Bounds.size());
                assert(anim->numJoints == BaseFrames.size());
                assert(anim->numFrames == Frames.size());
                assert(anim->numFrames == anim->Skeletons.size());

                md5File.close();
                return true;
            }

            return false;
        }

        // gets called on model load only
        void BuildFrameSkeleton(
            std::vector<FrameSkeleton>& skeletons,
            const std::vector<JointInfo>& jointInfos,
            const std::vector<BaseFrame>& baseFrames,
            const FrameData& frameData) {

            FrameSkeleton skeleton;

            for (u32 j = 0; j < jointInfos.size(); j++) {
                u32 n = 0;

                const JointInfo& jointInfo = jointInfos[j];
                SkeletonJoint animatedJoint = baseFrames[j];

                if (jointInfo.flags & 1) { // Pos.x
                    animatedJoint.position.x = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                if (jointInfo.flags & 2) { // Pos.y
                    animatedJoint.position.y = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                if (jointInfo.flags & 4) { // Pos.z
                    animatedJoint.position.z = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                if (jointInfo.flags & 8) { // Orient.x
                    animatedJoint.orientation.x = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                if (jointInfo.flags & 16) { // Orient.y
                    animatedJoint.orientation.y = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                if (jointInfo.flags & 32) { // Orient.z
                    animatedJoint.orientation.z = frameData.frameData[jointInfo.startIndex + n];
                    n++;
                }
                animatedJoint.orientation.reconstructW();

                if (jointInfo.parentID >= 0) {
                    SkeletonJoint& parentJoint = skeleton.Joints[jointInfo.parentID];
                    math::vec3 rotPos = math::TransformPointByQuaternion(parentJoint.orientation, animatedJoint.position);
                
                    animatedJoint.position = parentJoint.position + rotPos;
                
                    animatedJoint.orientation = parentJoint.orientation * animatedJoint.orientation;
                
                    animatedJoint.orientation.normalize();
                }

                skeleton.Joints.push_back(animatedJoint);
            }

            skeletons.push_back(skeleton);
        }

        void InterpolateSkeletons(FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, float interpolate) {
            for (int j = 0; j < finalSkeleton.Joints.size(); j++) {
                SkeletonJoint& finalJoint = finalSkeleton.Joints[j];
                const SkeletonJoint& joint0 = skeleton0.Joints[j];
                const SkeletonJoint& joint1 = skeleton1.Joints[j];

                finalJoint.position = math::lerp(joint0.position, joint1.position, interpolate);
                finalJoint.orientation = math::fast_slerp(joint0.orientation, joint1.orientation, interpolate);

                math::CreateTransform(finalJoint.transform, finalJoint.orientation, finalJoint.position, math::vec3(1));
            }
        }

        // gets called every frame - updates current skelton
        void UpdateMD5Animation(Animation* anim, float deltaTime) {
            if (anim->numFrames < 1) return;

            anim->animTime += deltaTime;

            // TODO: simpler modulo of frame time I think is possible
            while (anim->animTime > anim->animDuration) anim->animTime -= anim->animDuration;
            while (anim->animTime < 0.0f) anim->animTime += anim->animDuration;

            // find frames we are between
            float frameNum = anim->animTime * (float)anim->FrameRate;
            int frame0 = (int)floorf(frameNum);
            int frame1 = (int)ceilf(frameNum);
            frame0 %= anim->numFrames;  // TODO: is this redundant?
            frame1 %= anim->numFrames;

            float interp = fmodf(anim->animTime, anim->frameDuration) / anim->frameDuration;

            //frame0 = 2;
            //frame1 = 3;
            //interp = 0.0f;

            InterpolateSkeletons(anim->AnimatedSkeleton, anim->Skeletons[frame0], anim->Skeletons[frame1], interp);
        }
    }
}