#include <glm/glm.hpp>
#include <glfw3.h>
#include "light.h"
//#include <common/light.h>

struct Material {
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;
    glm::vec4 Ke;
    float Ns;
};

class ShaderProgram {
public:

    //GLFWwindow* window;
    GLuint LoadedShaderProgram;
    GLuint modelMatrixLocation;
    GLuint viewMatrixLocation;
    GLuint projectionMatrixLocation;

    GLuint lightPosition_worldspaceLocation;

    GLuint LaLocation;
    GLuint LdLocation;
    GLuint LsLocation;
    GLuint powerLocation;
    GLuint kcLocation;
    GLuint klLocation;
    GLuint kqLocation;

    GLuint KaLocation;
    GLuint KdLocation;
    GLuint KsLocation;
    GLuint KeLocation;
    GLuint NsLocation;

    GLuint textureSamplerDiffuse;
    GLuint textureSamplerSpecular;
    GLuint textureSamplerAmbient;
    GLuint textureSamplerEmissive;
    GLuint textureSamplerRoughness;
    GLuint textureSamplerNormal;

    GLuint textureSampler1;
    GLuint textureSampler2;
    GLuint textureSampler3;
    GLuint textureSampler4;

    std::vector<GLuint> lightPosition_worldspaceLocations;
    std::vector<GLuint> LaLocations;
    std::vector<GLuint> LdLocations;
    std::vector<GLuint> LsLocations;
    std::vector<GLuint> powerLocations;
    std::vector<GLuint> kcLocations;
    std::vector<GLuint> klLocations;
    std::vector<GLuint> kqLocations;

    // Constructor
    ShaderProgram(GLuint init_LoadedShaderProgram = 0,
        GLuint init_modelMatrixLocation = 0,
        GLuint init_viewMatrixLocation = 0,
        GLuint init_projectionMatrixLocation = 0,
        GLuint init_lightPosition_worldspaceLocation = 0,
        GLuint init_LaLocation = 0,
        GLuint init_LdLocation = 0,
        GLuint init_LsLocation = 0,
        GLuint init_powerLocation = 0,
        GLuint init_kcLocation = 0,
        GLuint init_klLocation = 0,
        GLuint init_kqLocation = 0,
        GLuint init_KaLocation = 0,
        GLuint init_KdLocation = 0,
        GLuint init_KsLocation = 0,
        GLuint init_KeLocation = 0,
        GLuint init_NsLocation = 0,
        GLuint init_textureSamplerDiffuse = 0,
        GLuint init_textureSamplerSpecular = 0,
        GLuint init_textureSamplerAmbient = 0,
        GLuint init_textureSamplerEmissive = 0,
        GLuint init_textureSamplerRoughness = 0,
        GLuint init_textureSamplerNormal = 0,
        GLuint init_textureSampler1 = 0, //simple texture sampler;
        GLuint init_textureSampler2 = 0, //free extra
        GLuint init_textureSampler3 = 0, //free extra
        GLuint init_textureSampler4 = 0, //free extra
        std::vector<GLuint> init_lightPosition_worldspaceLocations = {},
        std::vector<GLuint> init_LaLocations = {},
        std::vector<GLuint> init_LdLocations = {},
        std::vector<GLuint> init_LsLocations = {},
        std::vector<GLuint> init_powerLocations = {},
        std::vector<GLuint> init_kcLocations = {},
        std::vector<GLuint> init_klLocations = {},
        std::vector<GLuint> init_kqLocations = {}
    );

    //void getSamplers

    void GetUniformLocation();

    void GetUniformLocationMultipleLights(std::vector<Light*> lightVector);

    void uploadLight(Light* light);

    void uploadMultipleLights(std::vector<Light*> lightVector);

    void uploadMaterial(Material mtl = Material());

};
