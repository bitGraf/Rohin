#include <enpch.hpp>

#include "MD5MeshLoader.hpp"

namespace Engine {

    namespace md5 {

        /*
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
        */

        void PrepareNormals(Model* model, Mesh& mesh);
        void PrepareMesh(Model* model, Mesh& mesh);
        void PrepareMesh(Model* model, Mesh& mesh, const FrameSkeleton& skel);

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
                            joint.orientation.reconstructW_Left();

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
                    math::vec3 rotPos = math::TransformPointByQuaternion(joint.orientation, weight.pos);

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

            /*
            for (int n = 0; n < mesh.Verts.size(); n++) {
                Vert& vert = mesh.Verts[n];

                math::vec3 normal = vert.normal.normalize();

                // reset normal to find bind-pose normal in joint space?
                //vert.normal = math::vec3(0);
                //
                //// put the bind-pose normal into joint-local space
                //// so the animated normal can be computed faster later
                //for (int j = 0; j < vert.countWeight; j++) {
                //    const Weight& weight = mesh.Weights[vert.startWeight + j];
                //    const Joint& joint = model->Joints[weight.joint];
                //    vert.normal += math::vec3(joint.orientation * math::quat(normal,0) * math::inv(joint.orientation)) * weight.bias;
                //}
            }
            */
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
                    }
                    else if (param == "numJoints") {
                        md5File >> anim->numJoints;
                        assert(anim->numJoints > 0);
                        anim->JointInfos.reserve(anim->numJoints);
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
                                garb >> bound.max.z >> bound.max.y >> bound.max.z >> garb;
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

                            frame.orientation.reconstructW_Left();

                            anim->BaseFrames.push_back(frame);
                        }
                        md5File >> garb;
                        std::getline(md5File, garb);
                    }
                    else if (param == "frame") {
                        FrameData frame;
                        md5File >> frame.frameID >> garb;
                        std::getline(md5File, garb);

                        for (int n = 0; n < anim->numAnimComponents; n++) {
                            float data;
                            md5File >> data;
                            frame.frameData.push_back(data);
                        }

                        anim->Frames.push_back(frame);

                        BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, anim->BaseFrames, frame);

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
                assert(anim->numJoints == anim->BaseFrames.size());
                assert(anim->numFrames == anim->Frames.size());
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

                animatedJoint.parentID = jointInfo.parentID;

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
                animatedJoint.orientation.reconstructW_Left();

                if (animatedJoint.parentID >= 0) {
                    SkeletonJoint& parentJoint = skeleton.Joints[animatedJoint.parentID];
                    math::vec3 rotPos = math::TransformPointByQuaternion(parentJoint.orientation, animatedJoint.position);
                
                    animatedJoint.position = parentJoint.position + rotPos;
                
                    // backwards since original quaternions are left-handed I think...
                    animatedJoint.orientation = animatedJoint.orientation * parentJoint.orientation;
                
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

                finalJoint.parentID = joint0.parentID;

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

        // TODO: this should happen on the gpu every frame?
        void PrepareMesh(Model* model, Mesh& mesh, const FrameSkeleton& skel){
            for (unsigned int i = 0; i < mesh.Verts.size(); ++i){
                const Vert& vert = mesh.Verts[i];

                math::vec3 pos; //TODO: what are these used for??
                math::vec3 normal;

                pos = math::vec3(0);
                normal = math::vec3(0);

                for (int j = 0; j < vert.countWeight; ++j)
                {
                    const Weight& weight = mesh.Weights[vert.startWeight + j];
                    const SkeletonJoint& joint = skel.Joints[weight.joint];

                    math::vec3 rotPos = math::TransformPointByQuaternion(joint.orientation, weight.pos);
                    pos += (joint.position + rotPos) * weight.bias;

                    normal += math::TransformPointByQuaternion(joint.orientation, vert.normal) * weight.bias;
                }
            }
        }
    }
}