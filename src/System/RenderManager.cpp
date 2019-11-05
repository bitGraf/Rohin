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
    //msg.type = Message::Type::renderEvent;
    msg.text = std::to_string(id);
    sendMessage(msg);
}

CoreSystem* RenderManager::create() {
    m_mainShader.create("static_pbr.vert", "static_pbr.frag", "mainShader");
    m_lineShader.create("line.vert", "line.frag", "lineShader");

    return this;
}

void RenderManager::renderScene(Scene* scene) {
    /* Render Line Entities */
    m_lineShader.use();
    setCamera(&m_lineShader, &scene->camera);
    renderGrid(&m_lineShader, *scene->gridVAO, *scene->numVerts);

    /* Render Poly Entities */
    m_mainShader.use();
    m_mainShader.setLights(
        &scene->sun,
        scene->pointLights,
        scene->spotLights
    ); // TODO: remove this
    setCamera(&m_mainShader, &scene->camera);

    /* Render Entities */
    for (auto ent : scene->m_entities) {
        // Render entity ent
        if (ent.m_mesh != nullptr) {
            setCurrentMesh(ent.m_mesh);
            if (ent.m_material != nullptr) {
                setCurrentMaterial(&m_mainShader, ent.m_material);
            }

            setTransforms(&m_mainShader, &ent.position, &ent.orientation, &ent.scale);
            renderPrimitive(ent.m_mesh);
        }
    }
}


void RenderManager::setCamera(Shader* shader, Camera* camera) {
    camera->updateProjectionMatrix();
    camera->updateViewMatrix();

    shader->setMat4("viewMatrix", camera->viewMatrix);
    shader->setMat4("projectionMatrix", camera->projectionMatrix);
    shader->setVec3("cam_pos", camera->position);
}

void RenderManager::setCurrentMesh(const TriangleMesh* mesh) {
    glBindVertexArray(mesh->VAO);
}

void RenderManager::setCurrentMaterial(Shader* shader, const Material* material) {
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

    shader->setMaterial("material", material);
}

void RenderManager::setTransforms(
    Shader* shader,
    const math::vec3* pos,
    const math::mat3* orientation,
    const math::vec3* scale) {

    mat4 modelMatrix = (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(*pos, 1)) *
        mat4(*orientation) * 
        mat4(scale->x, scale->y, scale->z, 1));

    shader->setMat4("modelMatrix", modelMatrix);
}

void RenderManager::renderPrimitive(const TriangleMesh* mesh) {
    glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
}

void RenderManager::renderGrid(Shader* shader, GLuint vao, GLuint numVerts) {
    shader->setVec4("color", vec4(0,1,0,1));
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, numVerts);
}