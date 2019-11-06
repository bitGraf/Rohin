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
    m_skyboxShader.create("skybox.vert", "skybox.frag", "skyboxShader");
    m_fullscreenShader.create("fullscreen_quad.vert", "fullscreen_quad.frag", "fullscreenShader");

    const float fullscreenVerts[] = {
        -1, -1,
        -1,  1,
         1, -1,

         1, -1,
        -1,  1,
         1,  1
    };

    GLuint fullscreenVBO;
    glGenVertexArrays(1, &fullscreenVAO);
    glGenBuffers(1, &fullscreenVBO);
    glBindVertexArray(fullscreenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenVerts),
        &fullscreenVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);

    glBindVertexArray(0);



    stbi_set_flip_vertically_on_load(true);
    int height, width, nrChannels;

    //Generate OpenGL Texture
    glGenTextures(1, &fullscreenTexture);
    glBindTexture(GL_TEXTURE_2D, fullscreenTexture);
    // set the texture wrapping/filtering options

    //Load image data
    unsigned char *data = stbi_load("Data/Images/GiantToad.png",
        &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture []" << std::endl;
    }

    stbi_image_free(data);

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

    /* Render skybox */
    m_skyboxShader.use();
    renderSkybox(&m_skyboxShader, &scene->camera, &scene->skybox);

    /* Render fullscreen quad */
    m_fullscreenShader.use();

    m_fullscreenShader.setInt("tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene->envMap.hdrEquirect);

    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
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

void RenderManager::renderSkybox(Shader* shader, Camera* camera, SkyBox* skybox) {
    camera->updateProjectionMatrix();
    camera->updateViewMatrix();

    shader->setMat4("viewMatrix", mat4(mat3(camera->viewMatrix)));
    shader->setMat4("projectionMatrix", camera->projectionMatrix);

    shader->setInt("skybox", 0);
    skybox->bind(GL_TEXTURE0);

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(SkyBox::skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}