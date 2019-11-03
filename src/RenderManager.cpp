#include "RenderManager.hpp"

RenderManager::RenderManager() {

}

RenderManager::~RenderManager() {

}

void RenderManager::update(double dt) {

}

void RenderManager::handleMessage(Message msg) {

}

void RenderManager::destroy() {

}

void RenderManager::setShader(stringID id) {
    Message msg;
    msg.type = Message::Type::renderEvent;
    msg.text = std::to_string(id);
    this->putMessage(msg);
}

void RenderManager::sys_create(ConfigurationManager* configMgr) {
    m_shader.create("static_pbr.vert", "static_pbr.frag", "mainShader");
}

void RenderManager::renderScene(Scene* scene) {
    m_shader.use();

    m_shader.setLights(); // TODO: remove this

    setCamera(&scene->camera);
    for (auto ent : scene->m_entities) {
        // Render entity ent
        if (ent.m_mesh != nullptr) {
            setCurrentMesh(ent.m_mesh);
            if (ent.m_material != nullptr) {
                setCurrentMaterial(ent.m_material);
            }

            setTransforms(&ent.position, &ent.orientation, &ent.scale);

            renderPrimitive(ent.m_mesh);
        }
    }
}


void RenderManager::setCamera(Camera* camera) {
    camera->updateProjectionMatrix();
    camera->updateViewMatrix();

    m_shader.setMat4("viewMatrix", camera->viewMatrix);
    m_shader.setMat4("projectionMatrix", camera->projectionMatrix);
    m_shader.setVec3("cam_pos", camera->position);
}

void RenderManager::setCurrentMesh(const TriangleMesh* mesh) {
    glBindVertexArray(mesh->VAO);
}

void RenderManager::setCurrentMaterial(const Material* material) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->baseColorTexture.glTexID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material->normalTexture.glTexID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material->metallicRoughnessTexture.glTexID);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, material->occlusionTexture.glTexID);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, material->emissiveTexture.glTexID);

    m_shader.setMaterial("material", material);
}

void RenderManager::setTransforms(
    const math::vec3* pos,
    const math::mat3* orientation,
    const math::vec3* scale) {

    mat4 modelMatrix = (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(*pos, 1)) *
        mat4(*orientation) * 
        mat4(scale->x, scale->y, scale->z, 1));

    m_shader.setMat4("modelMatrix", modelMatrix);
}

void RenderManager::renderPrimitive(const TriangleMesh* mesh) {
    glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
}