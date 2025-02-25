#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glfw3.h>

#include "uniform.h"
//#include "light.h"

ShaderProgram::ShaderProgram(GLuint init_LoadedShaderProgram,
    GLuint init_modelMatrixLocation,
    GLuint init_viewMatrixLocation,
    GLuint init_projectionMatrixLocation,
    GLuint init_lightPosition_worldspaceLocation,
    GLuint init_LaLocation,
    GLuint init_LdLocation,
    GLuint init_LsLocation,
    GLuint init_powerLocation,
    GLuint init_kcLocation,
    GLuint init_klLocation,
    GLuint init_kqLocation,
    GLuint init_KaLocation,
    GLuint init_KdLocation,
    GLuint init_KsLocation,
    GLuint init_KeLocation,
    GLuint init_NsLocation,
    GLuint init_textureSamplerDiffuse,
    GLuint init_textureSamplerSpecular,
    GLuint init_textureSamplerAmbient,
    GLuint init_textureSamplerEmissive,
    GLuint init_textureSamplerRoughness,
    GLuint init_textureSamplerNormal,
    GLuint init_textureSampler1,
    GLuint init_textureSampler2,
    GLuint init_textureSampler3,
    GLuint init_textureSampler4,
    std::vector<GLuint> init_lightPosition_worldspaceLocations,
    std::vector<GLuint> init_LaLocations,
    std::vector<GLuint> init_LdLocations,
    std::vector<GLuint> init_LsLocations,
    std::vector<GLuint> init_powerLocations,
    std::vector<GLuint> init_kcLocations,
    std::vector<GLuint> init_klLocations,
    std::vector<GLuint> init_kqLocations) : LoadedShaderProgram(init_LoadedShaderProgram),
    modelMatrixLocation(init_modelMatrixLocation),
    viewMatrixLocation(init_viewMatrixLocation),
    projectionMatrixLocation(init_projectionMatrixLocation),
    lightPosition_worldspaceLocation(init_lightPosition_worldspaceLocation),
    LaLocation(init_LaLocation),
    LdLocation(init_LdLocation),
    LsLocation(init_LsLocation),
    powerLocation(init_powerLocation),
    kcLocation(init_kcLocation),
    klLocation(init_klLocation),
    kqLocation(init_kqLocation),
    KaLocation(init_KaLocation),
    KdLocation(init_KdLocation),
    KsLocation(init_KsLocation),
    KeLocation(init_KeLocation),
    NsLocation(init_NsLocation),
    textureSamplerDiffuse(init_textureSamplerDiffuse),
    textureSamplerSpecular(init_textureSamplerSpecular),
    textureSamplerAmbient(init_textureSamplerAmbient),
    textureSamplerEmissive(init_textureSamplerEmissive),
    textureSamplerRoughness(init_textureSamplerRoughness),
    textureSamplerNormal(init_textureSamplerNormal),
    textureSampler1(init_textureSampler1),
    textureSampler2(init_textureSampler2),
    textureSampler3(init_textureSampler3),
    textureSampler4(init_textureSampler4),
    lightPosition_worldspaceLocations(init_lightPosition_worldspaceLocations),
    LaLocations(init_LaLocations),
    LdLocations(init_LdLocations),
    LsLocations(init_LsLocations),
    powerLocations(init_powerLocations),
    kcLocations(init_kcLocations),
    klLocations(init_klLocations),
    kqLocations(init_kqLocations) {
    ;
}

//void ShaderProgram::

void ShaderProgram::GetUniformLocation() {
    modelMatrixLocation = glGetUniformLocation(LoadedShaderProgram, "M");
    viewMatrixLocation = glGetUniformLocation(LoadedShaderProgram, "V");
    projectionMatrixLocation = glGetUniformLocation(LoadedShaderProgram, "P");

    lightPosition_worldspaceLocation = glGetUniformLocation(LoadedShaderProgram, "light.lightPosition_worldspace");
    LaLocation = glGetUniformLocation(LoadedShaderProgram, "light.La");
    LdLocation = glGetUniformLocation(LoadedShaderProgram, "light.Ld");
    LsLocation = glGetUniformLocation(LoadedShaderProgram, "light.Ls");
    powerLocation = glGetUniformLocation(LoadedShaderProgram, "light.power");
    kcLocation = glGetUniformLocation(LoadedShaderProgram, "light.kc");
    klLocation = glGetUniformLocation(LoadedShaderProgram, "light.kl");
    kqLocation = glGetUniformLocation(LoadedShaderProgram, "light.kq");

    KaLocation = glGetUniformLocation(LoadedShaderProgram, "mtl.Ka");
    KdLocation = glGetUniformLocation(LoadedShaderProgram, "mtl.Kd");
    KsLocation = glGetUniformLocation(LoadedShaderProgram, "mtl.Ks");
    KeLocation = glGetUniformLocation(LoadedShaderProgram, "mtl.Ke");
    NsLocation = glGetUniformLocation(LoadedShaderProgram, "mtl.Ns");

    textureSamplerDiffuse = glGetUniformLocation(LoadedShaderProgram, "diffuseSampler");
    textureSamplerSpecular = glGetUniformLocation(LoadedShaderProgram, "specularSampler");
    textureSamplerAmbient = glGetUniformLocation(LoadedShaderProgram, "ambientSampler");
    textureSamplerEmissive = glGetUniformLocation(LoadedShaderProgram, "emissiveSampler");
    textureSamplerRoughness = glGetUniformLocation(LoadedShaderProgram, "roughnessSampler");
    textureSamplerNormal = glGetUniformLocation(LoadedShaderProgram, "normalSampler");

    textureSampler1 = glGetUniformLocation(LoadedShaderProgram, "Tex1Sampler");
    textureSampler2 = glGetUniformLocation(LoadedShaderProgram, "Tex2Sampler");
    textureSampler3 = glGetUniformLocation(LoadedShaderProgram, "Tex3Sampler");
    textureSampler4 = glGetUniformLocation(LoadedShaderProgram, "Tex4Sampler");
}

void ShaderProgram::GetUniformLocationMultipleLights(std::vector<Light*> lightVector ) {
/*
    for (int i = 0; i < lightVector.size(); i++) {
        std::string lightNumberString = "lightVector[" + std::to_string(i) + "]";
        
        const char* lightNumberPosition = ((std::string)lightNumberString + ".lightPosition_worldspace").c_str();
        lightPosition_worldspaceLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberPosition);

        const char* lightNumberLa = ((std::string)lightNumberString + ".La").c_str();
        LaLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberLa);

        const char* lightNumberLd = ((std::string)lightNumberString + ".Ld").c_str();
        LdLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberLd);

        const char* lightNumberLs = ((std::string)lightNumberString + ".Ls").c_str();
        LsLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberLs);

        const char* lightNumberPower = ((std::string)lightNumberString + ".power").c_str();
        powerLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberPower);

        const char* lightNumberkc = ((std::string)lightNumberString + ".kc").c_str();
        kcLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberkc);

        const char* lightNumberkl = ((std::string)lightNumberString + ".kl").c_str();
        klLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberkl);

        const char* lightNumberkq = ((std::string)lightNumberString + ".kq").c_str();
        kqLocation = glGetUniformLocation(LoadedShaderProgram, lightNumberkq);
    }
//*/
 
// Create vectors to store uniform locations for each light

    for (int i = 0; i < lightVector.size(); i++) {
        std::string lightNumberString = "lightVector[" + std::to_string(i) + "]";

        lightPosition_worldspaceLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".lightPosition_worldspace").c_str()));
        LaLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".La").c_str()));
        LdLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".Ld").c_str()));
        LsLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".Ls").c_str()));
        powerLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".power").c_str()));
        kcLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".kc").c_str()));
        klLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".kl").c_str()));
        kqLocations.push_back(glGetUniformLocation(LoadedShaderProgram, (lightNumberString + ".kq").c_str()));
    }

    // Store the vectors of uniform locations
    lightPosition_worldspaceLocations = lightPosition_worldspaceLocations;
    LaLocations = LaLocations;
    LdLocations = LdLocations;
    LsLocations = LsLocations;
    powerLocations = powerLocations;
    kcLocations = kcLocations;
    klLocations = klLocations;
    kqLocations = kqLocations;
//*/
}

void ShaderProgram::uploadLight(Light* light) {

    glUniform4f(LaLocation, light->La.r, light->La.g, light->La.b, light->La.a);
    glUniform4f(LdLocation, light->Ld.r, light->Ld.g, light->Ld.b, light->Ld.a);
    glUniform4f(LsLocation, light->Ls.r, light->Ls.g, light->Ls.b, light->Ls.a);
    glUniform3f(lightPosition_worldspaceLocation, light->lightPosition_worldspace.x,
        light->lightPosition_worldspace.y, light->lightPosition_worldspace.z);
    glUniform1f(powerLocation, light->power);
    glUniform1f(kcLocation, light->kc);
    glUniform1f(klLocation, light->kl);
    glUniform1f(kqLocation, light->kq);
}

void ShaderProgram::uploadMultipleLights(std::vector<Light*> lightVector) {
    for (int i = 0; i < lightVector.size(); i++) {
        glUniform4f(LaLocations[i], lightVector[i]->La.r, lightVector[i]->La.g, lightVector[i]->La.b, lightVector[i]->La.a);
        glUniform4f(LdLocations[i], lightVector[i]->Ld.r, lightVector[i]->Ld.g, lightVector[i]->Ld.b, lightVector[i]->Ld.a);
        glUniform4f(LsLocations[i], lightVector[i]->Ls.r, lightVector[i]->Ls.g, lightVector[i]->Ls.b, lightVector[i]->Ls.a);
        glUniform3f(lightPosition_worldspaceLocations[i], lightVector[i]->lightPosition_worldspace.x,
            lightVector[i]->lightPosition_worldspace.y, lightVector[i]->lightPosition_worldspace.z);
        glUniform1f(powerLocations[i], lightVector[i]->power);
        glUniform1f(kcLocations[i], lightVector[i]->kc);
        glUniform1f(klLocations[i], lightVector[i]->kl);
        glUniform1f(kqLocations[i], lightVector[i]->kq);
    }
}

void ShaderProgram::uploadMaterial(Material mtl){
    glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
    glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
    glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
    glUniform4f(KeLocation, mtl.Ke.r, mtl.Ke.g, mtl.Ke.b, mtl.Ke.a);
    glUniform1f(NsLocation, mtl.Ns);
}