// Include C++ headers
#include <iostream>
#include <chrono>
#include <unordered_map>

// Include Fade2D
//#include <Fade_2D.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 
#include <common/uniform.h>
//loading the post processing functions
#include <common/postProcessing.h>

using namespace std;
using namespace glm;
//using namespace GEOM_FADE2D;

// Function prototypes
// structural
void initialize();
void createContext();
void mainLoop();
void free();

// app-specific
void drawSceneObjects(mat4 viewMatrix, mat4 projectionMatrix);


//#define W_WIDTH 1024
//#define W_HEIGHT 768
#define W_WIDTH 1920
#define W_HEIGHT 1080
//#define W_WIDTH 1280
//#define W_HEIGHT 720
#define TITLE "ArtGallery"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

//framebuffer height/width ratio = 1.26 --> 1.259259
#define PORTAL_WIDTH 857        //1032//813//857
#define PORTAL_HEIGHT 1080	   //1300//1024//1080


// Global Variables
GLFWwindow* window;

// camera & lights
Camera* camera, * morningRoomCamera, * peireneFountainCamera, * destroyedRoomCamera, * greatDrawingRoomCamera, * metallicCaveCamera, * studioRoomCamera;
Light* studiolight, *metallicCaveLight1, *metallicCaveLight2, *metallicCaveLight3;
vector<Light*> MCLightVector;
vector<Light*> lightSpheresLightVector;
//for shaders
ShaderProgram* defaultShader, *depthShader, *miniMapShader, *lightSphereShader;
GLuint shaderProgram, depthProgram, miniMapProgram, lightSphereProgram;
ShaderProgram* morningRoomShader, * greatDrawingRoomShader, * peireneFountainShader, * metallicCaveShader, * destroyedRoomShader, * portalShader, * transitionShader;
GLuint  morningRoomShaderProgram, greatDrawingRoomShaderProgram, peireneFountainShaderProgram, metallicCaveShaderProgram, 
destroyedRoomShaderProgram, portalShaderProgram, transitionShaderProgram;
vector<ShaderProgram*> shaderObjectVector;

// for models
Drawable* studioRoom;
Drawable *morningRoom, *greatDrawingRoom, *peireneFountain, *metallicCave, *destroyedRoom;
Drawable* model2;
Drawable* plane;
Drawable* lightsphere1;
Drawable* lightbulb1, *lightbulb2, *lightbulb3;
Drawable* painting;
Drawable* portalMR, * portalGDR, * portalPF, * portalMC; //portal MorningRoom, GreatDrawingRoom, PeireneFountain, MetallicCave
Drawable* portalFrame;
GLuint studioRoomDiffuseTexture, studioRoomEmissionTexture, studioRoomRoughnessTexture;//modelSpecularTexture; //FOR ROUND ROOM
GLuint morningRoomTexture, greatDrawingRoomTexture;
GLuint peireneFountainTexture;
GLuint metallicCaveNormalTexture, metallicCaveInternalGroundAOTexture, metallicCaveRoughnessTexture;
GLuint destroyedRoomTexture;
// for Post-Processing
// Floyd-Steinberg Dithering
GLuint preDitheredMorningRoomTexture; //, ditheredTexture

GLuint readPBO_morningRoom, writePBO_morningRoom;
GLuint readPBO_peireneFountain, writePBO_peireneFountain;

#define NUM_ROOMS 6 // StudioRoom, MorningRoom, GreatDrawingRoom, PeireneFountain, DestroyedRoom, MetallicCave
GLuint roomFBOs[NUM_ROOMS];
GLuint roomTextures[NUM_ROOMS];
GLuint roomDepthTextures[NUM_ROOMS];

GLuint depthFBO, depthTexture, depthCubemap;
Drawable* quad, *miniquad;

#define NUM_PORTALS 6 //5 roooms, 1 return 
GLuint portalFBOs[NUM_PORTALS];
GLuint portalTextures[NUM_PORTALS];
GLuint portalDepthTextures[NUM_PORTALS];

// for the bump mapping
GLuint normalFBOs[NUM_ROOMS];
GLuint normalTextures[NUM_ROOMS];
GLuint normalDepthTextures[NUM_ROOMS];


// locations for defaultShader
GLuint defaultShaderWidthLocation, defaultShaderHeightLocation;

//locations for morningRoomShaderProgram
GLuint useTextureLocation, usePortalTextureLocation;
GLuint depthMapSampler;
GLuint lightVPLocation;

//locations for peireneFountainShaderProgram
//GLuint dotsCentersLocation;
//GLuint dotsRadiiLocation;
//GLuint numDotsLocation;

// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;
GLuint shadowPaintingLocation;

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;
GLuint quadWidthLocation, quadHeightLocation;

//locations for lightSphereProgram
GLuint lightsphereLightVPLocation;
GLuint lightsphereUseWhiteLocation;

// Create sample materials
const Material polishedSilver{
	vec4{0.23125, 0.23125, 0.23125, 1},
	vec4{0.2775, 0.2775, 0.2775, 1},
	vec4{0.773911, 0.773911, 0.773911, 1},
	vec4{0,0,0,0},
	89.6f
};

const Material turquoise{
	vec4{ 0.1, 0.18725, 0.1745, 0.8 },
	vec4{ 0.396, 0.74151, 0.69102, 0.8 },
	vec4{ 0.297254, 0.30829, 0.306678, 0.8 },
	vec4{0,0,0,0},
	12.8f
};

const Material obsidian = { 
	vec4(0.05375, 0.05, 0.06625, 0.82), 
	vec4(0.18275, 0.17, 0.22525, 0.82), 
	vec4(0.332741, 0.328634, 0.346435, 0.82),
	vec4(0,0,0,0),
	38.4f };

const Material Painting = {
	vec4(1.000000, 1.000000, 1.000000, 1),
	vec4(0.858824, 0.745098, 0.603922, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(0.0, 0.0, 0.0, 0.0),
	0.0f
		};

const Material gold = {
	vec4(0.24725, 0.1995, 0.0745, 1),
	vec4(0.75164, 0.60648, 0.22648, 1),
	vec4(0.628281, 0.555802, 0.366065, 1),
	vec4(0.0, 0.0, 0.0, 0.0),
	51.2
};

mat4 StudioRoomModelMatrix, TranslatingStudioRoom, ScalingStudioRoom; //FOR STUDIO ROOM
mat4 MorningRoomModelMatrix, TranslatingMorningRoom, ScalingMorningRoom, RotationMorningRoom; //FOR MORNING ROOM
mat4 PeireneFountainModelMatrix, TranslatingPeireneFountain, RotationPeireneFountain; //FOR PEIRENE FOUNTAIN
mat4 DestroyedRoomModelMatrix, TranslatingDestroyedRoom, ScalingDestroyedRoom; // FOR DESTROYED ROOM
mat4 GreatDrawingRoomModelMatrix, TranslatingGreatDrawingRoom; // FOR GREAT DRAWING ROOM
mat4 MetallicCaveModelMatrix, TranslatingMetallicCave; // FOR METALLIC CAVE
mat4 ScalingPainting; //FOR ALL PAINTINGS
mat4 RotationPainting3, RotationPainting4, RotationPainting5;
vec3 painting1TranslFactor, painting2TranslFactor, painting3TranslFactor, 
			painting4TranslFactor, painting5TranslFactor; //for keeping track of paintings distance
vec3 portalFrame1TranslFactor, portalFrame2TranslFactor, portalFrame3TranslFactor,
			portalFrame4TranslFactor, portalFrame5TranslFactor; //for keeping track of portal frames distance
vector<vec3> paintingsTranslFactorVector; 
vector<vec3> roomTeleportLocationsVector;
vector<vec3> roomTeleportDirectionsVector;
vector<Camera*> roomCameraVector;

//bool inStudioRoom = false;
//bool inMorningRoom = false;
//bool inPeireneFountain = false;
//bool inDestroyedRoom = false;
//bool inGreatDrawingRoom = false;
//bool inMetallicCave = false;
vector<bool> insideRoom;
int currentRoomIndex = 5; // inside Studio Room
bool useRealTimeRendering = false;
bool useOriginalTexture = false;
bool useKuwaharaFilter = false;
bool usePointilism = false;
bool useComicBookEffect = false; int halftoneCounter = 0;
bool useCRTEffect = false;
bool useNormalMaps = false;
bool useParallaxMapping = false;
bool raiseMesh = false;
bool stopPortalCameras = false;

bool showMinimap = false;
int minimapTextureIndex = 0;

//for when entering and exiting through portals
bool transitioning = false;
int transitioningToRoomIndex = 0;
float transitionDuration = 0.0f;
float globalTransitionDuration = 1.5f; // helper variable to keep track of the transition duration
float startingTransitionTime = -1.0f;
int transitionSelection = 0; // ranges from 0 (no transition) to 7 

void printInstructions() {
	cout << "------------------------------------" << endl;
	cout << "Instructions:" << endl;
	cout << "ENTER: Toggle active Dithering" << endl;
	cout << "SHIFT + ENTER: Toggle active colored Dithering or pre-dithered/original Texture" << endl;
	cout << "P: Toggle Pointilism" << endl;
	cout << "K: Toggle Kuwahara Filter" << endl;
	cout << "C: Toggle Comic Book Effect" << endl;
	cout << "T: Toggle CRT Effect" << endl;
	cout << "N: Toggle Normal Maps" << endl;
	cout << "M: Toggle Parallax Mapping" << endl;
	cout << "R: Raise Mesh" << endl;
	cout << "SHIFT + R: Stop Portal Cameras moving" << endl;
	cout << "0: Disable transitions and 1-7: Select transition effect" << endl;
	cout << "---" << endl;
	cout << "BACKSPACE: Print booleans status" << endl;
	cout << "SHIFT + BACKSPACE: Toggle minimap for debugging" << endl;
	cout << "LEFT/RIGHT ARROW: Change minimap texture" << endl;
	cout << "CTRL: Print Instructions Again" << endl;
	cout << "ESC: Quit" << endl;
	cout << "------------------------------------" << endl;

}

void initializeShaderObjects() {
	defaultShader = new ShaderProgram();
	depthShader = new ShaderProgram();
	miniMapShader = new ShaderProgram();
	lightSphereShader = new ShaderProgram();
	morningRoomShader = new ShaderProgram();
	greatDrawingRoomShader = new ShaderProgram();
	peireneFountainShader = new ShaderProgram();
	metallicCaveShader = new ShaderProgram();
	destroyedRoomShader = new ShaderProgram();
	portalShader = new ShaderProgram();
	transitionShader = new ShaderProgram();

	shaderObjectVector = { defaultShader, depthShader, miniMapShader, lightSphereShader, morningRoomShader,
		greatDrawingRoomShader, peireneFountainShader, metallicCaveShader, destroyedRoomShader, portalShader, transitionShader };
}

void loadShaderObjects() {
	//NOTE: check if the shaderProgram, depthProgram, etc... can be demoted from being global variables
	initializeShaderObjects();
	//ShadowMapping.vertexshader, ShadowMapping.fragmentshader
	shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");
	defaultShader->LoadedShaderProgram = shaderProgram;

	//Depth.vertexshader, Depth.fragmentshader
	depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader");
	depthShader->LoadedShaderProgram = depthProgram;

	// SimpleTexture.vertexshader, "SimpleTexture.fragmentshader"
	miniMapProgram = loadShaders("SimpleTexture.vertexshader", "SimpleTexture.fragmentshader");
	miniMapShader->LoadedShaderProgram = miniMapProgram;

	//Lightsphere.vertexshader, Lightsphere.fragmentshader
	lightSphereProgram = loadShaders("Lightsphere.vertexshader", "Lightsphere.fragmentshader");
	lightSphereShader->LoadedShaderProgram = lightSphereProgram;

	//for the Morning Room
	morningRoomShaderProgram = loadShaders("MorningRoom.vertexshader", "MorningRoom.fragmentshader");
	morningRoomShader->LoadedShaderProgram = morningRoomShaderProgram;

	//for the Great Drawing Room
	greatDrawingRoomShaderProgram = loadShaders("GreatDrawingRoom.vertexshader", "GreatDrawingRoom.fragmentshader");
	greatDrawingRoomShader->LoadedShaderProgram = greatDrawingRoomShaderProgram;

	//for the Peirene Fountain
	peireneFountainShaderProgram = loadShaders("PeireneFountain.vertexshader", "PeireneFountain.fragmentshader");
	peireneFountainShader->LoadedShaderProgram = peireneFountainShaderProgram;

	//for the Metallic Cave
	metallicCaveShaderProgram = loadShaders("MetallicCave.vertexshader", "MetallicCave.fragmentshader");
	metallicCaveShader->LoadedShaderProgram = metallicCaveShaderProgram;

	//for the Destroyed Room
	destroyedRoomShaderProgram = loadShaders("DestroyedRoom.vertexshader", "DestroyedRoom.fragmentshader");
	destroyedRoomShader->LoadedShaderProgram = destroyedRoomShaderProgram;

	portalShaderProgram = loadShaders("Portal.vertexshader", "Portal.fragmentshader");
	portalShader->LoadedShaderProgram = portalShaderProgram;

	transitionShaderProgram = loadShaders("Transition.vertexshader", "Transition.fragmentshader");
	transitionShader->LoadedShaderProgram = transitionShaderProgram;
	// NOTE: Don't forget to delete the shader programs on the free() function
}

void createPortalFrameComponents() {
	/*
	//portal-necessary parameters
	float width = 1.35f;
	float height = 1.7;
	vector<vec3> portalFrameVertices = {
	  vec3(0.0, 0.0, 0.0),
	  vec3(0.0, height, 0.0),
	  vec3(0.0, height, width),
	  vec3(0.0, height, width),
	  vec3(0.0, 0.0, width),
	  vec3(0.0, 0.0, 0.0)
	};

	// UV coordinates
	vector<vec2> portalFrameUVs = {
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 0.0),
		vec2(0.0, 0.0)
	};

	// Normal vectors
	vector<vec3> portalFrameNormals = {
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.0, 1.0)
	};
	*/
	/* */
	// start the timer
	auto start = std::chrono::high_resolution_clock::now();

	const int gridX = 500; // Number of subdivisions along the width
	const int gridY = 500; // Number of subdivisions along the height

	float width = 1.35f;
	float height = 1.7f;

	// Vectors to store the subdivided mesh data
	vector<vec3> portalFrameVertices;
	vector<vec2> portalFrameUVs;
	vector<vec3> portalFrameNormals;

	// Create the grid of vertices, UVs, and normals in the correct order
	for (int y = 0; y < gridY; ++y) {
		for (int x = 0; x < gridX; ++x) {
			// Calculate the position of the four vertices of each quad
			float u1 = static_cast<float>(x) / static_cast<float>(gridX);
			float v1 = static_cast<float>(y) / static_cast<float>(gridY);
			float u2 = static_cast<float>(x + 1) / static_cast<float>(gridX);
			float v2 = static_cast<float>(y + 1) / static_cast<float>(gridY);

			float py1 = v1 * height;
			float pz1 = u1 * width;
			float py2 = v2 * height;
			float pz2 = u2 * width;

			// First triangle (bottom-left to top-right of the quad)
			portalFrameVertices.push_back(vec3(0.0f, py1, pz1)); // Bottom-left
			portalFrameVertices.push_back(vec3(0.0f, py2, pz1)); // Bottom-right
			portalFrameVertices.push_back(vec3(0.0f, py2, pz2)); // Top-right

			// Second triangle (top-left to bottom-right of the quad)
			portalFrameVertices.push_back(vec3(0.0f, py2, pz2)); // Top-right
			portalFrameVertices.push_back(vec3(0.0f, py1, pz2)); // Top-left
			portalFrameVertices.push_back(vec3(0.0f, py1, pz1)); // Bottom-left

			// Corresponding UV coordinates
			portalFrameUVs.push_back(vec2(u1, v1)); // Bottom-left
			portalFrameUVs.push_back(vec2(u1, v2)); // Bottom-right
			portalFrameUVs.push_back(vec2(u2, v2)); // Top-right

			portalFrameUVs.push_back(vec2(u2, v2)); // Top-right
			portalFrameUVs.push_back(vec2(u2, v1)); // Top-left
			portalFrameUVs.push_back(vec2(u1, v1)); // Bottom-left

			// Normals (same for all vertices)
			for (int i = 0; i < 6; ++i) {
				portalFrameNormals.push_back(vec3(1.0f, 0.0f, 0.0f));
			}
		}
	}
	// */
	portalFrame = new Drawable(portalFrameVertices, portalFrameUVs, portalFrameNormals);

	// end the timer
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << ">>>>Elapsed time for portal frame creation: " << elapsed_seconds.count() << " seconds" << std::endl;
}

void createContext() {

	GLenum error = glGetError();
	if (error == GL_NO_ERROR) {
		cout << "NO ERROR!" << endl;
	}
	else {
		cout << "ERROR!" << endl;
		cout << error << endl;
	}

	// Start timing
	cout << ">>>>Starting counter...\n=============" << endl;
	auto start = std::chrono::high_resolution_clock::now();

	loadShaderObjects();

	defaultShader->GetUniformLocation();
	// Task 1.4
	useTextureLocation = glGetUniformLocation(defaultShader->LoadedShaderProgram, "useTexture"); //extra information for defaultShader
	// location for portal 
	usePortalTextureLocation = glGetUniformLocation(defaultShader->LoadedShaderProgram, "usePortalTexture");

	// --- defaultShader ---
	// locations for shadow rendering
	depthMapSampler = glGetUniformLocation(defaultShader->LoadedShaderProgram, "shadowMapSampler"); //extra information for defaultShader
	lightVPLocation = glGetUniformLocation(defaultShader->LoadedShaderProgram, "lightVP"); //extra information for defaultShader

	defaultShaderWidthLocation = glGetUniformLocation(defaultShader->LoadedShaderProgram, "width");
	defaultShaderHeightLocation = glGetUniformLocation(defaultShader->LoadedShaderProgram, "height");

	// --- depthProgram ---
	depthShader->GetUniformLocation();
	shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

	// --- miniMapProgram ---
	quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");
	quadWidthLocation = glGetUniformLocation(miniMapProgram, "width");
	quadHeightLocation = glGetUniformLocation(miniMapProgram, "height");

	//-- lightSphereProgram --
	lightSphereShader->GetUniformLocation();
	lightSphereShader->GetUniformLocationMultipleLights(lightSpheresLightVector);
	lightsphereLightVPLocation = glGetUniformLocation(lightSphereShader->LoadedShaderProgram, "lightVP");
	lightsphereUseWhiteLocation = glGetUniformLocation(lightSphereShader->LoadedShaderProgram, "useWhite");

	//-- morningRoomShaderProgram --
	morningRoomShader->GetUniformLocation();

	//-- greatDrawingRoomShaderProgram --
	greatDrawingRoomShader->GetUniformLocation();

	//-- peireneFountainShaderProgram --
	peireneFountainShader->GetUniformLocation();

	
	//-- metallicCaveShaderProgram --
	metallicCaveShader->GetUniformLocation();
	metallicCaveShader->GetUniformLocationMultipleLights(MCLightVector);

	//-- destroyedRoomShaderProgram --
	destroyedRoomShader->GetUniformLocation();

	//-- portalShaderProgram --
	portalShader->GetUniformLocation();

	//-- transitionShaderProgram --
	transitionShader->GetUniformLocation();

	//ROOM AND OBJECT MISCELLANEOUS PARAMETERS
	// Creating a 2D quad to visualize the depthmap
	// create geometry for screen-space quad
	vector<vec3> quadVertices_old = {
	  vec3(0.5, 0.5, -1.0),
	  vec3(1.0, 0.5, -1.0),
	  vec3(1.0, 1.0, -1.0),
	  vec3(1.0, 1.0, -1.0),
	  vec3(0.5, 1.0, -1.0),
	  vec3(0.5, 0.5, -1.0)
	};

	std::vector<glm::vec3> quadVertices = {
	glm::vec3(-1.0f, -1.0f, 0.0f),  // bottom left
	glm::vec3(1.0f, -1.0f, 0.0f),   // bottom right
	glm::vec3(1.0f, 1.0f, 0.0f),    // top right
	glm::vec3(1.0f, 1.0f, 0.0f),    // top right
	glm::vec3(-1.0f, 1.0f, 0.0f),   // top left
	glm::vec3(-1.0f, -1.0f, 0.0f)   // bottom left
	};

	vector<vec2> quadUVs = {
	  vec2(0.0, 0.0),
	  vec2(1.0, 0.0),
	  vec2(1.0, 1.0),
	  vec2(1.0, 1.0),
	  vec2(0.0, 1.0),
	  vec2(0.0, 0.0)
	};

	createPortalFrameComponents();
	
	//studioRoom parameters
	vec3 coords0 = vec3(0, 0, 0);
	TranslatingStudioRoom = translate(mat4(), coords0);
	float s0 = 10.0f;
	ScalingStudioRoom = glm::scale(mat4(), vec3(s0));
	StudioRoomModelMatrix = TranslatingStudioRoom * ScalingStudioRoom;
	//morningRoom parameters
	vec3 coords1 = vec3(-100.0, 0.0, 0.0);
	TranslatingMorningRoom = translate(mat4(), coords1);
	float s1 = 3.0f;
	ScalingMorningRoom = glm::scale(mat4(), vec3(s1));
	float angle = 235.0f;
	RotationMorningRoom = glm::rotate(mat4(), radians(angle), vec3(0.0, 1.0, 0.0));
	MorningRoomModelMatrix = TranslatingMorningRoom * ScalingMorningRoom * RotationMorningRoom;
	//greatDrawingRoom parameters
	vec3 coords2 = vec3(+100, 0.0, 0.0);
	TranslatingGreatDrawingRoom = translate(mat4(), coords2);
	GreatDrawingRoomModelMatrix = TranslatingGreatDrawingRoom * ScalingMorningRoom * RotationMorningRoom;
	//peireneFountain parameters
	vec3 coords3 = vec3(+120.0, 0.0, -120.0);
	TranslatingPeireneFountain = translate(mat4(), coords3);
	float angle2 = 180.0f;
	RotationPeireneFountain = glm::rotate(mat4(), radians(angle2), vec3(0.0, 1.0, 0.0));
	PeireneFountainModelMatrix = TranslatingPeireneFountain * ScalingMorningRoom * RotationPeireneFountain;
	//metallicCave Parameters
	vec3 coords4 = vec3(0.0, 0.0, -300.0);
	TranslatingMetallicCave = translate(mat4(), coords4);
	MetallicCaveModelMatrix = TranslatingMetallicCave;
	//destroyedRoom Parameters
	vec3 coords5 = vec3(-120.0, 0, -140.0);
	TranslatingDestroyedRoom = translate(mat4(), coords5);
	float s2 = 6.0f;
	ScalingDestroyedRoom = glm::scale(mat4(), vec3(s2));
	DestroyedRoomModelMatrix = TranslatingDestroyedRoom * ScalingDestroyedRoom;
	//paintings parameters
	RotationPainting3 = rotate(mat4(), radians(90.0f) / 1.0f, vec3(0, 1, 0));
	RotationPainting4 = rotate(mat4(), radians(90.0f) / 1.0f, vec3(0, 1, 0));
	RotationPainting5 = rotate(mat4(), -radians(90.0f) / 1.0f, vec3(0, 1, 0));
	//morningRoom Texture Path parameter
	const char* morningRoomTexturePath = "models/otherRooms/TheMorningRoom/tex_u1_v1.jpg";
	//for Floyd-Steinberg
	// texture size is 8192x8192
	int text_width = 8192; 
	int text_height = 8192;
	GLubyte* imageData = new GLubyte[text_width * text_height * 3];

	// TEXTURES
	// create a timer for the texture loading
	auto texture_start = std::chrono::high_resolution_clock::now();
	//StudioRoom textures
	studioRoomDiffuseTexture = loadDDS("models/studioRoom/textures/material_diffuse3.dds");
	studioRoomEmissionTexture = loadDDS("models/studioRoom/textures/material_emission.dds");
	studioRoomRoughnessTexture = loadDDS("models/studioRoom/textures/material_roughness.dds");
	//MorningRoom textures
	morningRoomTexture = loadSOIL(morningRoomTexturePath);
	preDitheredMorningRoomTexture = loadSOIL("models/otherRooms/TheMorningRoom/dithered_morning_room.bmp"); //loadSOIL(morningRoomTexturePath); if the pre-dithered texture is not available,
																											//we need to load the original texture and apply the dithering effect, then save it as a file
	//GreatDrawingRoom texture
	greatDrawingRoomTexture = loadDDS("models/otherRooms/TheGreatDrawingRoom/tex_u1_v1.dds");
	//peireFountain texture
	peireneFountainTexture = loadDDS("models/otherRooms/PeireneFountain/tex_u1_v1.dds");
	//destroyedRoom texture
	destroyedRoomTexture = loadDDS("models/otherRooms/DestroyedRoom/Material_Diffuse.dds");
	//destroyedRoomTexture = loadSOIL("models/otherRooms/DestroyedRoom/brushStroked.png");
	//metallicCave textures
	metallicCaveNormalTexture = loadSOIL("models/otherRooms/MetallicCave/Gravel006_4K_Normal.jpg");
	metallicCaveInternalGroundAOTexture = loadSOIL("models/otherRooms/MetallicCave/internal_ground_ao_texture.jpeg");
	metallicCaveRoughnessTexture = loadSOIL("models/otherRooms/MetallicCave/Gravel006_4K_Roughness.jpg");

	auto texture_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> texture_duration = texture_end - texture_start;
	std::cout << ">>>>Elapsed time for texture loading: " << texture_duration.count() << " seconds" << std::endl;

	// ROOMS
	std::cout << "Loading models...";
	// create a timer for the model loading
	auto model_start = std::chrono::high_resolution_clock::now();
	//StudioRoom model
	studioRoom = new Drawable("models/studioRoom/source/WhiteRoom/WhiteRoom.obj");
	//MorningRoom model
	morningRoom = new Drawable("models/otherRooms/TheMorningRoom/model.obj");
	if (morningRoom->indexedNormals.size() == 0) 
		calculateIndexedNormals(*morningRoom);
	//GreatDrawingRoom model
	greatDrawingRoom = new Drawable("models/otherRooms/TheGreatDrawingRoom/model.obj");
	if (greatDrawingRoom->indexedNormals.size() == 0)
		calculateIndexedNormals(*greatDrawingRoom);
	//PeireneFountain model
	peireneFountain = new Drawable("models/otherRooms/PeireneFountain/model.obj");
	if (peireneFountain->indexedNormals.size() == 0)
		calculateIndexedNormals(*peireneFountain);
	//MetallicCave model
	metallicCave = new Drawable("models/otherRooms/MetallicCave/metallic_cave.obj"); 
	if (metallicCave->indexedNormals.size() == 0) // almost certainly not needed
		calculateIndexedNormals(*metallicCave);
	calculateIndexedTangents(*metallicCave); //  for normal mapping
	//DestroyedRoom model
	destroyedRoom = new Drawable("models/otherRooms/DestroyedRoom/Room.obj");
	if (destroyedRoom->indexedNormals.size() == 0)
		calculateIndexedNormals(*destroyedRoom);
	//lightbulbs models
	lightbulb1 = new Drawable("earth.obj");
	lightsphere1 = new Drawable("earth.obj");
	//painting model
	painting = new Drawable("models/painting/pictureframe.obj");
	//Portals model
	
	//quad objects
	quad = new Drawable(quadVertices, quadUVs);
	miniquad = new Drawable(quadVertices_old, quadUVs);
	auto model_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> model_duration = model_end - model_start;
	std::cout << " ... Finished! Took " << model_duration.count() << " seconds" << std::endl;
	
	//*/

	// ---------------------------------------------------------------------------- //
	// -  Task 3.2 Create a depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //
	//*/
	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFBO);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);


	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Telling opengl the required information about the texture
	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	///

	// Task 4.5 Don't shadow area out of light's viewport
	///
	// Step 1 : (Don't forget to comment out the respective lines above
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set color to set out of border 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float borderColor[] = { 0.0 , 0.0, 0.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Next go to fragment shader and add an iff statement, so if the distance in the z-buffer is equal to 1, 
	// meaning that the fragment is out of the texture border (or further than the far clip plane) 
	// then the shadow value is 0.
	///

	/// Task 3.2 Continue
	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


	// Since the depth buffer is only for the generation of the depth texture, 
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//*/

	//----------------------------------------------------------------------------- //
	//---------------- Attaching a texture to screen framebuffer ----------------- //
	//----------------------------------------------------------------------------- //
	for (int i = 0; i < NUM_ROOMS; ++i) {
		// Generate and bind framebuffer
		glGenFramebuffers(1, &roomFBOs[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[i]);

		// Generate and bind texture
		glGenTextures(1, &roomTextures[i]);
		glBindTexture(GL_TEXTURE_2D, roomTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W_WIDTH, W_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Generate and bind depth texture
		glGenTextures(1, &roomDepthTextures[i]);
		glBindTexture(GL_TEXTURE_2D, roomDepthTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, W_WIDTH, W_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// Set color to set out of border
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// Attach textures to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, roomDepthTextures[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, roomTextures[i], 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::runtime_error("Frame buffer not initialized correctly for room " + std::to_string(i));
		}
	}


	//----------------------------------------------------------------------------- //
	//------ Creating a framebuffer and a texture to store the portal image ------- //
	//----------------------------------------------------------------------------- //
	for (int i = 0; i < NUM_PORTALS; ++i) {
		// Create framebuffer
		glGenFramebuffers(1, &portalFBOs[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, portalFBOs[i]);

		// Create texture
		glGenTextures(1, &portalTextures[i]);
		glBindTexture(GL_TEXTURE_2D, portalTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
			GL_RGB, GL_UNSIGNED_BYTE, NULL);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create and attach a texture for depth
		glGenTextures(1, &portalDepthTextures[i]);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //border?
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //border?
		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, portalDepthTextures[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, portalTextures[i], 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::runtime_error("Frame buffer not initialized correctly");
		}
	}
	//------------------------------------------------------------------------------------ //
	//------ Creating a framebuffer and a texture to create and store the bump map ------- //
	//------------------------------------------------------------------------------------ //

	for (int i = 0; i < NUM_ROOMS; ++i) {
		// Create framebuffer
		glGenFramebuffers(1, &normalFBOs[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, normalFBOs[i]);

		// Create texture
		glGenTextures(1, &normalTextures[i]);
		glBindTexture(GL_TEXTURE_2D, normalTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
			GL_RGB, GL_UNSIGNED_BYTE, NULL);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create and attach a texture for depth
		glGenTextures(1, &normalDepthTextures[i]);
		glBindTexture(GL_TEXTURE_2D, normalDepthTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, normalDepthTextures[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalTextures[i], 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::runtime_error("Frame buffer not initialized correctly");
		}
	}


	//----------------------------------------------------------------------------- //
	//----------- Creating a pixel buffer for morningRoomTexture  ------------ //
	//----------------------------------------------------------------------------- //
	glGenBuffers(1, &readPBO_morningRoom);
	glGenBuffers(1, &writePBO_morningRoom);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
	glBufferData(GL_PIXEL_PACK_BUFFER, W_WIDTH * W_HEIGHT * 4 * sizeof(GLubyte), NULL,  GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, writePBO_morningRoom); 
	glBufferData(GL_PIXEL_PACK_BUFFER, W_WIDTH * W_HEIGHT * 4 * sizeof(GLubyte), NULL, GL_STREAM_READ);


	// Binding the default framebuffer and PBOs
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------------------------------------- //
	//----------- Creating a pixel buffer for peireneFountainTexture  ------------ //
	//----------------------------------------------------------------------------- //

	glGenBuffers(1, &readPBO_peireneFountain);
	glGenBuffers(1, &writePBO_peireneFountain);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_peireneFountain);
	glBufferData(GL_PIXEL_PACK_BUFFER, W_WIDTH * W_HEIGHT * 4 * sizeof(GLubyte), NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, writePBO_peireneFountain);
	glBufferData(GL_PIXEL_PACK_BUFFER, W_WIDTH * W_HEIGHT * 4 * sizeof(GLubyte), NULL, GL_STREAM_READ);

	// Binding the default framebuffer and PBOs
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//initializing

	error = glGetError();
	if (error == GL_NO_ERROR) {
		cout << "NO ERROR!" << endl;
	}
	else {
		cout << "ERROR!" << endl;
		cout << error << endl;
	}
																	//size of the texture
	//applyTextureFloyd_SteinbergDithering(preDitheredMorningRoomTexture, 8192, 8192);
	//saveTextureAsBMP(preDitheredMorningRoomTexture, 8192, 8192, "dithered_morning_room.bmp"); // the texture ought to be saved to models/otherRooms/TheMorningRoom

	/*
	error = glGetError();
	if (error == GL_NO_ERROR) {
		cout << "NO ERROR!" << endl;
	}
	else {
		cout << "ERROR!" << endl;
		cout << error << endl;
	}

	//*/

	// End timing
	auto end = std::chrono::high_resolution_clock::now();
	// Calculate duration
	std::chrono::duration<double> duration = end - start;
	// Output duration
	std::cout << ">>>>Elapsed time: " << duration.count() << " seconds" << std::endl;
	
	// Print the main program instructions
	printInstructions();

}

void deleteShaderPrograms() {
	for (int i = 0; i < shaderObjectVector.size(); i++) {
		glDeleteProgram(shaderObjectVector[i]->LoadedShaderProgram);
	}
}

void free() {
	deleteShaderPrograms();	

	glDeleteFramebuffers(1, &depthFBO);
	glDeleteTextures(1, &depthTexture);
	for (int i = 0; i < NUM_PORTALS; ++i) {
		glDeleteFramebuffers(1, &portalFBOs[i]);
		glDeleteTextures(1, &portalTextures[i]);
	}
	for (int i = 0; i < NUM_ROOMS; ++i) {
		glDeleteFramebuffers(1, &roomFBOs[i]);
		glDeleteTextures(1, &roomTextures[i]);
		glDeleteTextures(1, &roomDepthTextures[i]);
		glDeleteFramebuffers(1, &normalFBOs[i]);
		glDeleteTextures(1, &normalTextures[i]);
		glDeleteTextures(1, &normalDepthTextures[i]);
	}

	glfwTerminate();
}

// Function to check if the camera is within the bounding box of a painting
bool isCameraNearPainting(const glm::vec3& paintingPosition, float thresholdDistance) {
	// Define a bounding box around the painting
	glm::vec3 boxMin = paintingPosition - glm::vec3(1.0f); // Adjust the size of the bounding box as needed
	glm::vec3 boxMax = paintingPosition + glm::vec3(1.0f);

	// Check if the camera is within the bounding box
	return (camera->position.x >= boxMin.x && camera->position.x <= boxMax.x &&
		camera->position.y >= boxMin.y && camera->position.y <= boxMax.y &&
		camera->position.z >= boxMin.z && camera->position.z <= boxMax.z);
}

// Function to transition the camera position when entering a portal in an interesting way
void transitionCamera(glm::vec3 targetPosition, float transitionDuration) {
	//
}

// Function to check if the user is close to a painting and trigger a transition
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//cout << "Camera Position: " << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << endl;
	//cout << "Direction now is: " << camera->direction.x << ", " << camera->direction.y << ", " << camera->direction.z << endl;
	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		if (length(camera->position) < 55.0f) {
			float thresholdDistance = 17.0f;

			// Iterate over paintings to find the one the camera is near
			for (int i = 0; i < paintingsTranslFactorVector.size(); i++) {
				cout << "Searching..." << i << endl;

				// Print camera position for debugging
				cout << "Camera Position: " << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << endl;

				// Print painting position for debugging
				cout << "Painting Position: " << paintingsTranslFactorVector[i].x << ", " << paintingsTranslFactorVector[i].y << ", " << paintingsTranslFactorVector[i].z << endl;

				// Check distance using glm::distance
				float currentDistanceFromRoom = glm::distance(camera->position, paintingsTranslFactorVector[i]);

				// Print distance for debugging
				cout << "Distance from Room: " << currentDistanceFromRoom << endl;

				if (currentDistanceFromRoom < thresholdDistance) {
					int roomNo = i; // Set the appropriate room number
					std::cout << "TRIGGER!!! " << "Room: " << roomNo << std::endl;

					// Print teleport location for debugging
					cout << "Teleporting to: " << roomTeleportLocationsVector[roomNo].x << ", " << roomTeleportLocationsVector[roomNo].y << ", " << roomTeleportLocationsVector[roomNo].z << endl;

					/*
					camera->position = roomTeleportLocationsVector[roomNo];

					insideRoom[i] = true;
					insideRoom[5] = false;
					currentRoomIndex = i;

					camera->verticalAngle = roomCameraVector[i]->verticalAngle;
					camera->horizontalAngle = roomCameraVector[i]->horizontalAngle;
					*/

					transitioning = true;
					transitioningToRoomIndex = i;

					break; // Exit the loop once a painting is found
				}
			}
		}
		else {
			// Teleport to the Studio Room
			//camera->position = vec3(0, 16, 0);
			//insideRoom[currentRoomIndex] = false;
			//insideRoom[5] = true;
			//currentRoomIndex = 5;

			//camera->setDirection(vec3(-29.3934, 16, 0.970362));

			transitioning = true;
			transitioningToRoomIndex = 5;
		}
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods != GLFW_MOD_SHIFT) {
		useRealTimeRendering = !useRealTimeRendering;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_SHIFT) {
		useOriginalTexture = !useOriginalTexture;
	}
	// Toggle the Kuwahara filter by pressing the 'K' key
	if (action == GLFW_PRESS && key == GLFW_KEY_K) {
		useKuwaharaFilter = !useKuwaharaFilter;
	}
	// Toggle the Pointilism effect by pressing the 'P' key
	if (action == GLFW_PRESS && key == GLFW_KEY_P) {
		usePointilism = !usePointilism;

	}
	// Toggle the Comic Book effect by pressing the 'C' key
	if (action == GLFW_PRESS && key == GLFW_KEY_C) {
		useComicBookEffect = !useComicBookEffect;
		//if (useComicBookEffect) { // There are unused, different halftone patterns that aren't used
		//	halftoneCounter = (halftoneCounter + 1) % 8;
		//	cout << "Halftone Counter: " << halftoneCounter << endl;
		//} 
	}
	// Toggle the CRT effect by pressing the 'T' key
	if (action == GLFW_PRESS && key == GLFW_KEY_T) {
		useCRTEffect = !useCRTEffect;
	}
	// Toggle the Normal Mapping
	if (action == GLFW_PRESS && key == GLFW_KEY_N) {
		useNormalMaps = !useNormalMaps;
	}
	// Toggle the Parallax Mapping
	if (action == GLFW_PRESS && key == GLFW_KEY_M) {
		useParallaxMapping = !useParallaxMapping;
	}
	// Toggle the Raised Mesh
	if (action == GLFW_PRESS && key == GLFW_KEY_R && mods != GLFW_MOD_SHIFT) {
		raiseMesh = !raiseMesh;
	}
	// Toggle the updating portal cameras
	if (action == GLFW_PRESS && key == GLFW_KEY_R && mods == GLFW_MOD_SHIFT) {
		stopPortalCameras = !stopPortalCameras;
	}
	// press 1, 2, 3, 4, 5, 6, 7, 0 to switch between transition modes
	if (action == GLFW_PRESS && key == GLFW_KEY_1) {
		transitionSelection = 1;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_2) {
		transitionSelection = 2;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_3) {
		transitionSelection = 3;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_4) {
		transitionSelection = 4;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_5) {
		transitionSelection = 5;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_6) {
		transitionSelection = 6;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_7) {
		transitionSelection = 7;
		transitionDuration = globalTransitionDuration;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_0) {
		transitionSelection = 0; // no transition
		transitionDuration = 0.0f;
	}

	// Arrow keys to switch texture in minimap
	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
		// goes up to 31
		minimapTextureIndex = (minimapTextureIndex + 1) % 32;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
		// goes down to 0, must round back to 31 
		minimapTextureIndex = (minimapTextureIndex - 1) % 32;
	}
	// Print var status
	if (action == GLFW_PRESS && key == GLFW_KEY_BACKSPACE && mods != GLFW_MOD_SHIFT) {
		cout << "------------------------------------" << endl;
		cout << "Camera Position: " << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << endl;
		cout << "useRealTimeRendering: " << useRealTimeRendering << endl;
		cout << "useOriginalTexture: " << useOriginalTexture << endl;
		cout << "usePointilism: " << usePointilism << endl;
		cout << "useKuwaharaFilter: " << useKuwaharaFilter << endl;
		cout << "useComicBookEffect: " << useComicBookEffect << endl;
		cout << "useCRTEffect: " << useCRTEffect << endl;
		cout << "useNormalMaps: " << useNormalMaps << endl;
		cout << "useParallaxMapping: " << useParallaxMapping << endl;
		cout << "raiseMesh: " << raiseMesh << endl;
		cout << "stopPortalCameras: " << stopPortalCameras << endl;
		cout << "transitionSelection: " << transitionSelection << endl;
		cout << "minimapTextureIndex: " << minimapTextureIndex << endl;
		cout << "------------------------------------" << endl;
	}
	// Toggle the minimap
	if (action == GLFW_PRESS && key == GLFW_KEY_BACKSPACE && mods == GLFW_MOD_SHIFT) {
		showMinimap = !showMinimap;
	}
	// Print the instructions
	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT_CONTROL) {
		printInstructions();
	}

}

void updateAllPortalCameras() {
	if (!stopPortalCameras) {
		morningRoomCamera->setDirection(roomTeleportDirectionsVector[4]);
		morningRoomCamera->verticalAngle = camera->verticalAngle - morningRoomCamera->verticalAngle;
		morningRoomCamera->horizontalAngle = camera->horizontalAngle - morningRoomCamera->horizontalAngle - 20.0f;
		morningRoomCamera->update();

		peireneFountainCamera->setDirection(roomTeleportDirectionsVector[3]);
		peireneFountainCamera->verticalAngle = camera->verticalAngle - peireneFountainCamera->verticalAngle;
		peireneFountainCamera->horizontalAngle = camera->horizontalAngle - peireneFountainCamera->horizontalAngle;
		peireneFountainCamera->update();

		destroyedRoomCamera->setDirection(-roomTeleportDirectionsVector[4]);
		destroyedRoomCamera->verticalAngle = camera->verticalAngle - destroyedRoomCamera->verticalAngle;
		destroyedRoomCamera->horizontalAngle = camera->horizontalAngle - destroyedRoomCamera->horizontalAngle - 20.0f;
		destroyedRoomCamera->update();

		greatDrawingRoomCamera->setDirection(roomTeleportDirectionsVector[0]);
		greatDrawingRoomCamera->verticalAngle = camera->verticalAngle - greatDrawingRoomCamera->verticalAngle;
		greatDrawingRoomCamera->horizontalAngle = camera->horizontalAngle - greatDrawingRoomCamera->horizontalAngle;
		greatDrawingRoomCamera->update();

		metallicCaveCamera->setDirection(roomTeleportDirectionsVector[4]);
		metallicCaveCamera->verticalAngle = camera->verticalAngle - metallicCaveCamera->verticalAngle;
		metallicCaveCamera->horizontalAngle = camera->horizontalAngle - metallicCaveCamera->horizontalAngle - 33.0f;
		metallicCaveCamera->update();

		/*for (Camera* room_cam : roomCameraVector) {
			room_cam->pollMovement();
		}*/
	}

}

//void depth_pass(mat4 viewMatrix, mat4 projectionMatrix) {
//
//	// Task 3.3
//	//*/
//
//
//	// Setting viewport to shadow map size
//	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//
//	// Binding the depth framebuffer
//	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
//
//	// Cleaning the framebuffer depth information (stored from the last render)
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	// Selecting the new shader program that will output the depth component
//	glUseProgram(depthShader->LoadedShaderProgram);
//
//	// sending the view and projection matrix to the shader
//	mat4 view_projection = projectionMatrix * viewMatrix;
//	glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);
//
//
//	// ---- rendering the scene ---- //
//
//	// ---- Rendering the scene from the portal's perspective ---- //
//	/* */
//	mat4 modelMatrix = MetallicCaveModelMatrix;
//	glUniformMatrix4fv(depthShader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//
//	glActiveTexture(GL_TEXTURE10);
//	glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
//	glUniform1i(metallicCaveShader->textureSamplerNormal, 10);
//
//	glActiveTexture(GL_TEXTURE11);
//	glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
//	glUniform1i(metallicCaveShader->textureSampler1, 11);
//
//	glActiveTexture(GL_TEXTURE12);
//	glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);
//	glUniform1i(metallicCaveShader->textureSamplerRoughness, 12);
//
//	metallicCave->bind();
//	metallicCave->draw();
//	// */
//
//	//mat4 modelMatrix = GreatDrawingRoomModelMatrix;//modelObjModelMatrix;
//	//glUniformMatrix4fv(depthShader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//
//	//greatDrawingRoom->bind();
//	//greatDrawingRoom->draw();
//	
//	/*/
//	mat4 modelMatrix = MorningRoomModelMatrix;//translate(mat4(), vec3(0.0, 0.0, 0.0));//translate(mat4(), vec3(0.0, 0.0, -5.0));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//
//	studioRoom->bind();
//	studioRoom->draw();
//
//	mat4 paintingModelMatrix = translate(mat4(), painting1TranslFactor) * scale(mat4(), vec3(0.3));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);
//
//	defaultShader->uploadMaterial(gold);
//	painting->bind();
//	painting->draw();
//
//
//	paintingModelMatrix = translate(mat4(), painting2TranslFactor) * scale(mat4(), vec3(0.3));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);
//
//	painting->bind();
//	painting->draw();
//
//	paintingModelMatrix = translate(mat4(), painting3TranslFactor) * scale(mat4(), vec3(0.3)) * rotate(mat4(), radians(90.0f) / 1.0f, vec3(0, 1, 0));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);
//
//	painting->bind();
//	painting->draw();
//
//	paintingModelMatrix = translate(mat4(), painting4TranslFactor) * scale(mat4(), vec3(0.3)) * rotate(mat4(), radians(90.0f) / 1.0f, vec3(0, 1, 0));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);
//
//	painting->bind();
//	painting->draw();
//
//	paintingModelMatrix = translate(mat4(), painting5TranslFactor) * scale(mat4(), vec3(0.3)) * rotate(mat4(), -radians(90.0f) / 1.0f, vec3(0, 1, 0));
//	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);
//
//	painting->bind();
//	painting->draw();
//	//*/
//	// binding the screen framebuffer again
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}

void portal_pass(int i, mat4 viewMatrix, mat4 projectionMatrix, ShaderProgram* shader, Drawable* modelObject, mat4 modelObjModelMatrix) {
	// Setting viewport to portal size
	glViewport(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT); // Replaced W_WIDTH and W_HEIGHT with the actual size of portal

	// Binding the portal framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, portalFBOs[i]);

	// Cleaning the portal framebuffer color and depth information
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Selecting the shader program for portal rendering
	glUseProgram(shader->LoadedShaderProgram);

	// Sending the view and projection matrix to the portal shader
	glUniformMatrix4fv(shader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	glUniformMatrix4fv(shader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	// ---- Rendering the scene from the portal's perspective ---- //

	mat4 modelMatrix = modelObjModelMatrix;
	glUniformMatrix4fv(shader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	vec4 sky_blue(0.6, 0.8, 1.0, 1.0);
	vec4 grey(0.5, 0.5, 0.5, 1.0);
	vec4 black(0.0, 0.0, 0.0, 1.0);
	vec4 white(1.0, 1.0, 1.0, 1.0); //for debugging
	vec4 color = sky_blue;
	switch (i) {
		case 0: // Morning Room
			color = grey;
			glClearColor(color.r, color.g, color.b, color.a);
			glActiveTexture(GL_TEXTURE4);
			// if we are using real-time rendering or the original texture, we bind the original texture
			if (useRealTimeRendering || useOriginalTexture)
				glBindTexture(GL_TEXTURE_2D, morningRoomTexture);
			// else, we bind the pre-dithered texture
			else
				glBindTexture(GL_TEXTURE_2D, preDitheredMorningRoomTexture);
			glUniform1i(shader->textureSampler1, 4);
			break;
		case 1: // Peirene Fountain
			color = sky_blue;
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, peireneFountainTexture);
			glUniform1i(shader->textureSampler1, 6);

			break;
		case 2: // Destroyed Room
			color = sky_blue;
			glActiveTexture(GL_TEXTURE13);
			glBindTexture(GL_TEXTURE_2D, destroyedRoomTexture);
			glUniform1i(shader->textureSampler1, 13);
			break;
		case 3: // Great Drawing Room
			color = sky_blue;
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, greatDrawingRoomTexture);
			glUniform1i(greatDrawingRoomShader->textureSampler1, 5);
			break;
		case 4: // Metallic Cave
			color = black;
			shader->uploadMaterial(obsidian);
			shader->uploadMultipleLights(MCLightVector);

			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
			glUniform1i(shader->textureSamplerNormal, 10);

			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
			glUniform1i(shader->textureSampler1, 11);

			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);
			glUniform1i(shader->textureSamplerRoughness, 12);
			break;
		default:
			color = sky_blue;
			break;
	}
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);

	modelObject->bind(); 
	modelObject->draw(); 


	// FLOYD-STEINBERG DITHERING FOR THE PORTAL TO MORNING ROOM
	if (useRealTimeRendering && i == 0) {
		glBindTexture(GL_TEXTURE_2D, portalTextures[i]);
		// Map the PBO for data access
		glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
		// Initiate pixel transfer to the PBO
		glReadPixels(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Get the image data from the PBO
		GLubyte* morningRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
		if (morningRoomImageData == nullptr) {
			std::cerr << "Failed to map buffer" << std::endl;
			return; // Early exit if mapping fails
		}
		bool useGrayscale = false;
		if (useOriginalTexture) {
			useGrayscale = true;
		}
		else {
			useGrayscale = false;
		}

		Floyd_SteinbergDitheringAlgorithm_RGBA(morningRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

		// Update the texture from the write PBO
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PORTAL_WIDTH, PORTAL_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// Unbind PBOs
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// POINTILISM EFFECT FOR THE PORTAL TO PEIRENE FOUNTAIN
	else if (usePointilism && i==1) {
		//setup: get the screen texture
		glBindTexture(GL_TEXTURE_2D, portalTextures[i]);
		// Map the PBO for data access
		glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_peireneFountain);
		// Initiate pixel transfer to the PBO
		glReadPixels(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Get the image data from the PBO
		GLubyte* peireneFountainImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
		if (peireneFountainImageData == nullptr) {
			std::cerr << "Failed to map buffer" << std::endl;
			// Check for OpenGL errors
			auto error = glGetError();
			if (error != GL_NO_ERROR) {
				std::cerr << "OpenGL error after mapping buffer: " << error << std::endl;
			}
			return; // Early exit if mapping fails
		}

		try {
			std::vector<Dot> dots = processImageForPointilism(peireneFountainImageData, PORTAL_WIDTH, PORTAL_HEIGHT, 6000, useRealTimeRendering);
			drawDots(peireneFountainImageData, PORTAL_WIDTH, PORTAL_HEIGHT, dots);
		}
		catch (...) {
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			throw; // Re-throw the exception after cleanup
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

		// Update the texture from the write PBO
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_peireneFountain);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PORTAL_WIDTH, PORTAL_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// Unbind PBOs
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//else if (useKuwaharaFilter && i == 2) {} // Handled in the fragment shader of defaultShader, banned from applying in the studioRoom
	//else if (useComicBookEffect && i == 3) {} // Handled in the fragment shader of defaultShader, banned from applying in the studioRoom

	// Binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void bumpMapping_pass(int i, mat4 viewMatrix, mat4 projectionMatrix, ShaderProgram* shader, Drawable* modelObject, mat4 modelObjModelMatrix) {
	// Setting viewport to portal size
	glViewport(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT);

	// Binding the bumpMapping framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, normalFBOs[i]);

	// Cleaning the bumpMapping framebuffer color and depth information
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);


	// Selecting the shader program for portal rendering
	glUseProgram(shader->LoadedShaderProgram);

	// sending the view and projection matrix to the shader
	//mat4 view_projection = projectionMatrix * viewMatrix;
	glUniformMatrix4fv(shader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(shader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	mat4 modelMatrix = modelObjModelMatrix;
	glUniformMatrix4fv(shader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); 

	switch (i) {
		case 0: // Morning Room
			glActiveTexture(GL_TEXTURE4);
			// if we are using real-time rendering or the original texture, we bind the original texture
			if (useRealTimeRendering || useOriginalTexture)
				glBindTexture(GL_TEXTURE_2D, morningRoomTexture);
			// else, we bind the pre-dithered texture
			else
				glBindTexture(GL_TEXTURE_2D, preDitheredMorningRoomTexture);
			glUniform1i(shader->textureSampler1, 4);
			break;
		case 1: // Peirene Fountain
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, peireneFountainTexture);
			glUniform1i(shader->textureSampler1, 6);

			break;
		case 2: // Destroyed Room
			glActiveTexture(GL_TEXTURE13);
			glBindTexture(GL_TEXTURE_2D, destroyedRoomTexture);
			glUniform1i(shader->textureSampler1, 13);
			break;
		case 3: // Great Drawing Room
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, greatDrawingRoomTexture);
			glUniform1i(greatDrawingRoomShader->textureSampler1, 5);
			break;
		case 4: // Metallic Cave
			//shader->uploadMaterial(obsidian);
			//shader->uploadMultipleLights(MCLightVector);

			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
			glUniform1i(shader->textureSamplerNormal, 10);

			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
			glUniform1i(shader->textureSampler1, 11);

			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);
			glUniform1i(shader->textureSamplerRoughness, 12);
			break;
		default:
			break;
	}

	modelObject->bind();
	modelObject->draw();

	// Binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Now, normalTexture[i] should contain the normal map of the scene

}

void depth_pass(int i, mat4 viewMatrix, mat4 projectionMatrix, ShaderProgram* shader, Drawable* modelObject, mat4 modelObjModelMatrix) {
	// Setting viewport to portal size
	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glViewport(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT);

	// Binding the bumpMapping framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	// Cleaning the 
	glClear(GL_DEPTH_BUFFER_BIT);

	// Selecting the shader program for creating a depth map
	glUseProgram(shader->LoadedShaderProgram);

	// sending the view and projection matrix to the shader
	//mat4 view_projection = projectionMatrix * viewMatrix;
	//glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);

	glUniformMatrix4fv(shader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	glUniformMatrix4fv(shader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	mat4 modelMatrix = modelObjModelMatrix;
	glUniformMatrix4fv(shader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	//switch (i) {
	//case 0: // Morning Room
	//	glActiveTexture(GL_TEXTURE4);
	//	// if we are using real-time rendering or the original texture, we bind the original texture
	//	if (useRealTimeRendering || useOriginalTexture)
	//		glBindTexture(GL_TEXTURE_2D, morningRoomTexture);
	//	// else, we bind the pre-dithered texture
	//	else
	//		glBindTexture(GL_TEXTURE_2D, preDitheredMorningRoomTexture);
	//	glUniform1i(shader->textureSampler1, 4);
	//	break;
	//case 1: // Peirene Fountain
	//	glActiveTexture(GL_TEXTURE6);
	//	glBindTexture(GL_TEXTURE_2D, peireneFountainTexture);
	//	glUniform1i(shader->textureSampler1, 6);

	//	break;
	//case 2: // Destroyed Room
	//	glActiveTexture(GL_TEXTURE13);
	//	glBindTexture(GL_TEXTURE_2D, destroyedRoomTexture);
	//	glUniform1i(shader->textureSampler1, 13);
	//	break;
	//case 3: // Great Drawing Room
	//	glActiveTexture(GL_TEXTURE5);
	//	glBindTexture(GL_TEXTURE_2D, greatDrawingRoomTexture);
	//	glUniform1i(greatDrawingRoomShader->textureSampler1, 5);
	//	break;
	//case 4: // Metallic Cave
	//	//shader->uploadMaterial(obsidian);
	//	//shader->uploadMultipleLights(MCLightVector);

	//	glActiveTexture(GL_TEXTURE10);
	//	glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
	//	glUniform1i(shader->textureSamplerNormal, 10);

	//	glActiveTexture(GL_TEXTURE11);
	//	glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
	//	glUniform1i(shader->textureSampler1, 11);

	//	glActiveTexture(GL_TEXTURE12);
	//	glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);
	//	glUniform1i(shader->textureSamplerRoughness, 12);
	//	break;
	//default:
	//	break;
	//}

	modelObject->bind();
	modelObject->draw();

	// Binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}
	
void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {

	// Step 1: Binding a frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[currentRoomIndex]);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Step 2: Clearing color and depth info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Step 3: Selecting shader program
	glUseProgram(defaultShader->LoadedShaderProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(defaultShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(defaultShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	//and the morningRoomShaderProgram!
	float t = glfwGetTime();
	float radius = 20.0f; // Adjust the radius of the circular motion
	float angularSpeed = 1.2f; // Adjust the speed of the circular motion

	//vec3{ -0.7, 64.037, -5.01013},
	
	/* */
	studiolight->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) - 0.7; // 
	studiolight->lightPosition_worldspace.y = 0.0 + 54.037;
	studiolight->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 5.01013;
	/**/
	studiolight->targetPosition = vec3(studiolight->lightPosition_worldspace.x, 0, studiolight->lightPosition_worldspace.z);

	// uploading the light parameters to the shader program
	defaultShader->uploadLight(studiolight);// AndMaterial(light, turquoise);
	// Task 1.4 Use different material for the plane
	// NOTE: when we make a variable uniform to the shader program the value of 
	//       that variable remains the same until we upload a new value. 
	//       This means that the same model matrix and material defined for
	//       model2 are used for the plane as well. 

	// upload the material


	// Task 4.1 Display shadows on the scene
	//*/
	// Sending the shadow texture to the shaderProgram
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);

	// Sending the light View-Projection matrix to the shader program
	mat4 lightVP = studiolight->lightVP();
	glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

	//*/
	drawSceneObjects(viewMatrix, projectionMatrix); 

}

void drawSceneObjects(mat4 viewMatrix, mat4 projectionMatrix){
	// ----------------------------------------------------------------- //
	// --------------------- Drawing scene objects --------------------- //	
	// ----------------------------------------------------------------- //


	//THE STUDIO ROOM
	
	if (insideRoom[5] == true) {
		// creating a model matrix
		mat4 modelMatrix = TranslatingStudioRoom * ScalingStudioRoom;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

		// Setting up texture to display on shader program          //  --- Texture Pipeline ---
		glActiveTexture(GL_TEXTURE1);								// Activate texture position
		glBindTexture(GL_TEXTURE_2D, studioRoomDiffuseTexture);			// Assign texture to position 
		//glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position
		glUniform1i(defaultShader->textureSamplerDiffuse, 1);

		/*
		glActiveTexture(GL_TEXTURE2);								//
		glBindTexture(GL_TEXTURE_2D, modelSpecularTexture);			// Same process for specular texture
		glUniform1i(specularColorSampler, 2);						//
		*/

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, studioRoomEmissionTexture);
		//glUniform1i(emissionColorSampler, 2);
		glUniform1i(defaultShader->textureSamplerEmissive, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, studioRoomRoughnessTexture);
		glUniform1i(defaultShader->textureSamplerRoughness, 3);

		// Inside the fragment shader, there is an if statement whether to use  
		// the material of an object or sample a texture
		glUniform1i(useTextureLocation, 1);


		// Draw studioRoom
		studioRoom->bind();
		studioRoom->draw();


		defaultShader->uploadMaterial(turquoise);
		glUniform1i(useTextureLocation, 0);

		//
		mat4 paintingModelMatrix = translate(mat4(), painting1TranslFactor) * ScalingPainting;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

		defaultShader->uploadMaterial(gold);
		painting->bind();
		painting->draw();


		paintingModelMatrix = translate(mat4(), painting2TranslFactor) * ScalingPainting;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

		painting->bind();
		painting->draw();

		paintingModelMatrix = translate(mat4(), painting3TranslFactor) * ScalingPainting * RotationPainting3;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

		painting->bind();
		painting->draw();

		paintingModelMatrix = translate(mat4(), painting4TranslFactor) * ScalingPainting * RotationPainting4;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

		painting->bind();
		painting->draw();

		paintingModelMatrix = translate(mat4(), painting5TranslFactor) * ScalingPainting * RotationPainting5;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

		painting->bind();
		painting->draw();

		// re-set the useTextureLocation to 0
		glUniform1i(useTextureLocation, 0);

		// PORTAL FRAME, PORTAL TEXTURE RENDERING

		mat4 ScalingPortalFrame = scale(mat4(), vec3(9));
		mat4 portalFrameModelMatrix0 = translate(mat4(), portalFrame1TranslFactor) * ScalingPortalFrame;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix0[0][0]);

		glUniform1i(usePortalTextureLocation, 1);

		glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
		glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
		glUniform1f(glGetUniformLocation(defaultShader->LoadedShaderProgram, "time"), glfwGetTime());
		if (useNormalMaps && insideRoom[5]) // if effect is enabled and in studio room
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useNormalMaps"), 1);
		else
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useNormalMaps"), 0);

		if (useParallaxMapping && insideRoom[5]) // if effect is enabled and in studio room
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useParallaxMapping"), 1);
		else
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useParallaxMapping"), 0);

		if (raiseMesh && insideRoom[5]) // if effect is enabled and in studio room
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "raiseMesh"), 1);
		else
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "raiseMesh"), 0);

		// PAINTING OF THE MORNING ROOM
		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_2D, portalTextures[0]);
		glUniform1i(defaultShader->textureSampler1, 14);

		if (useNormalMaps) {
			//include bump map of the portal to the morning room
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, normalTextures[0]);
			glUniform1i(defaultShader->textureSamplerNormal, 7);
		}

		if (useParallaxMapping || raiseMesh) {
			//include height map of the portal to the morning room
			glActiveTexture(GL_TEXTURE27);
			glBindTexture(GL_TEXTURE_2D, portalDepthTextures[0]);
			glUniform1i(defaultShader->textureSampler2, 27);
		}

		portalFrame->bind();
		portalFrame->draw();

		mat4 portalFrameModelMatrix = translate(mat4(), portalFrame2TranslFactor) * ScalingPortalFrame;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

		// PAINTING OF THE PEIRENE FOUNTAIN
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, portalTextures[1]);
		glUniform1i(defaultShader->textureSampler1, 15);
		if (usePointilism && currentRoomIndex == 5) {
			//
			glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
			glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 0);
		}

		if (useNormalMaps) {
			//include bump map of the portal to the peirene fountain
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, normalTextures[1]);
			glUniform1i(defaultShader->textureSamplerNormal, 8);
		}

		if (useParallaxMapping || raiseMesh) {
			//include height map of the portal to the peirene fountain
			glActiveTexture(GL_TEXTURE28);
			glBindTexture(GL_TEXTURE_2D, portalDepthTextures[1]);
			glUniform1i(defaultShader->textureSampler2, 28);
		}

		portalFrame->bind();
		portalFrame->draw();
		// re-assigning the value to 0
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 0);

		portalFrameModelMatrix = translate(mat4(), portalFrame3TranslFactor) * ScalingPortalFrame * RotationPainting3;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

		// PAINTING OF THE DESTROYED ROOM
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, portalTextures[2]);
		glUniform1i(defaultShader->textureSampler1, 16);
		if (useKuwaharaFilter && currentRoomIndex == 5) {
			//
			glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
			glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 0);
		}

		if (useNormalMaps) {
			//include bump map of the portal to the destroyed room
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, normalTextures[2]);
			glUniform1i(defaultShader->textureSamplerNormal, 9);
		}

		if (useParallaxMapping || raiseMesh) {
			//include height map of the portal to the destroyed room
			glActiveTexture(GL_TEXTURE29);
			glBindTexture(GL_TEXTURE_2D, portalDepthTextures[2]);
			glUniform1i(defaultShader->textureSampler2, 29);
		}

		portalFrame->bind();
		portalFrame->draw();
		// re-assigning the value to 0
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 0);


		portalFrameModelMatrix = translate(mat4(), portalFrame4TranslFactor) * ScalingPortalFrame * RotationPainting4;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

		// PAINTING OF THE GREAT DRAWING ROOM
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_2D, portalTextures[3]);
		glUniform1i(defaultShader->textureSampler1, 17);
		if (useComicBookEffect && currentRoomIndex == 5) {
			//
			glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
			glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 1);
			//glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "halftoneCounter"), halftoneCounter);
		}
		else {
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 0);
		}

		if (useNormalMaps) {
			//include bump map of the portal to the great drawing room
			glActiveTexture(GL_TEXTURE25);
			glBindTexture(GL_TEXTURE_2D, normalTextures[3]);
			glUniform1i(defaultShader->textureSamplerNormal, 25);
		}

		if (useParallaxMapping || raiseMesh) {
			//include height map of the portal to the great drawing room
			glActiveTexture(GL_TEXTURE30);
			glBindTexture(GL_TEXTURE_2D, portalDepthTextures[3]);
			glUniform1i(defaultShader->textureSampler2, 30);
		}

		portalFrame->bind();
		portalFrame->draw();
		// re-assigning the value to 0
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 0);

		portalFrameModelMatrix = translate(mat4(), portalFrame5TranslFactor) * ScalingPortalFrame * RotationPainting5;
		glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

		// PAINTING OF THE METALLIC CAVE
		glActiveTexture(GL_TEXTURE18);
		glBindTexture(GL_TEXTURE_2D, portalTextures[4]);
		glUniform1i(defaultShader->textureSampler1, 18);
		if (useCRTEffect && currentRoomIndex == 5) {
			//
			glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
			glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 0);
		}

		if (useNormalMaps) {
			//include bump map of the portal to the metallic cave
			glActiveTexture(GL_TEXTURE26);
			glBindTexture(GL_TEXTURE_2D, normalTextures[4]);
			glUniform1i(defaultShader->textureSamplerNormal, 26);
		}

		if (useParallaxMapping || raiseMesh) {
			//include height map of the portal to the metallic cave
			glActiveTexture(GL_TEXTURE31);
			glBindTexture(GL_TEXTURE_2D, portalDepthTextures[4]);
			glUniform1i(defaultShader->textureSampler2, 31);
		}

		portalFrame->bind();
		portalFrame->draw();
		// re-assigning the value to 0
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 0);

		glUniform1i(usePortalTextureLocation, 0);//
	}
	//THE MORNING ROOM
	if (insideRoom[0] == true) {

		glUseProgram(morningRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(morningRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(morningRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 morningRoomModelMatrix = MorningRoomModelMatrix;//TranslatingMorningRoom * ScalingMorningRoom * RotationMorningRoom;
		glUniformMatrix4fv(morningRoomShader->modelMatrixLocation, 1, GL_FALSE, &morningRoomModelMatrix[0][0]);

		
		glActiveTexture(GL_TEXTURE4);
		if (useRealTimeRendering || useOriginalTexture) {
			glBindTexture(GL_TEXTURE_2D, morningRoomTexture);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, preDitheredMorningRoomTexture);
		}
		glUniform1i(morningRoomShader->textureSampler1, 4);

		// Draw MorningRoom

		morningRoom->bind();
		morningRoom->draw();
	}


	//THE GREAT DRAWING ROOM
	if (insideRoom[3] == true) {

		glUseProgram(greatDrawingRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(greatDrawingRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(greatDrawingRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 greatDrawingRoomModelMatrix = GreatDrawingRoomModelMatrix;//TranslatingGreatDrawingRoom * ScalingMorningRoom * RotationMorningRoom;
		glUniformMatrix4fv(greatDrawingRoomShader->modelMatrixLocation, 1, GL_FALSE, &greatDrawingRoomModelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, greatDrawingRoomTexture);
		glUniform1i(greatDrawingRoomShader->textureSampler1, 5);

		greatDrawingRoom->bind();
		greatDrawingRoom->draw();
	}

	// PEIRENE FOUNTAIN
	if (insideRoom[1] == true) {

		glUseProgram(peireneFountainShader->LoadedShaderProgram);

		glUniformMatrix4fv(peireneFountainShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(peireneFountainShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 peireneFountainRoomModelMatrix = PeireneFountainModelMatrix;// TranslatingPeireneFountain* ScalingMorningRoom* RotationPeireneFountain;
		glUniformMatrix4fv(peireneFountainShader->modelMatrixLocation, 1, GL_FALSE, &peireneFountainRoomModelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, peireneFountainTexture);
		glUniform1i(peireneFountainShader->textureSampler1, 6);

		peireneFountain->bind();
		peireneFountain->draw();
	}

	// METALLIC CAVE
	if (insideRoom[4] == true) {

		glUseProgram(metallicCaveShader->LoadedShaderProgram);

		glUniformMatrix4fv(metallicCaveShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(metallicCaveShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 metallicCaveModelMatrix = MetallicCaveModelMatrix; //TranslatingMetallicCave;
		glUniformMatrix4fv(metallicCaveShader->modelMatrixLocation, 1, GL_FALSE, &metallicCaveModelMatrix[0][0]);		

		metallicCaveShader->uploadMaterial(obsidian);
		metallicCaveShader->uploadMultipleLights(MCLightVector);

		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
		glUniform1i(metallicCaveShader->textureSamplerNormal, 10);

		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
		glUniform1i(metallicCaveShader->textureSampler1, 11);

		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);
		glUniform1i(metallicCaveShader->textureSamplerRoughness, 12);

		metallicCave->bind();
		metallicCave->draw();
	}

	//DESTROYED ROOM
	if (insideRoom[2] == true) {
		glUseProgram(destroyedRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(destroyedRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(destroyedRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 destroyedRoomModelMatrix = DestroyedRoomModelMatrix; //TranslatingDestroyedRoom * ScalingDestroyedRoom;
		glUniformMatrix4fv(destroyedRoomShader->modelMatrixLocation, 1, GL_FALSE, &destroyedRoomModelMatrix[0][0]);

		//destroyedRoomShader->uploadMaterial();
		//destroyedRoomShader->uploadLight();

		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_2D, destroyedRoomTexture);
		glUniform1i(destroyedRoomShader->textureSampler1, 13);

		destroyedRoom->bind();
		destroyedRoom->draw();
	}

	// LIGHTSPHERES
	if (insideRoom[4] || insideRoom[5]) { // either inside Metallic Cave or Studio Room
		glUseProgram(lightSphereShader->LoadedShaderProgram);

		if (insideRoom[5]) { // Studio Room

			glUniform1i(lightsphereUseWhiteLocation, 0);

			glUniformMatrix4fv(lightSphereShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			glUniformMatrix4fv(lightSphereShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

			mat4 lightsphere1ModelMatrix = translate(mat4(), studiolight->lightPosition_worldspace) * scale(mat4(), vec3(0.2));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightsphere1ModelMatrix[0][0]);

			lightsphere1->bind();
			lightsphere1->draw();
		}

		if (insideRoom[4]) { // Metallic Cave
			std::cout << "Lights should be drawn..." << std::endl;

			glUniform1i(lightsphereUseWhiteLocation, 1);

			glUniformMatrix4fv(lightSphereShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			glUniformMatrix4fv(lightSphereShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

			float t = glfwGetTime();
			float radius = 0.0625f; // Adjust the radius of the circular motion
			float angularSpeed = 40.2f; // Adjust the speed of the circular motion

			metallicCaveLight1->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) + 0.0; // Keep the x-coordinate constant
			metallicCaveLight1->lightPosition_worldspace.y = 0.0 + 6;
			metallicCaveLight1->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			lightSphereShader->uploadLight(metallicCaveLight1);

			mat4 lightbulb1ModelMatrix = translate(mat4(), metallicCaveLight1->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb1ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();


			//radius = 5.0f; // Adjust the radius of the circular motion
			//angularSpeed = 1.2f; // Adjust the speed of the circular motion

			metallicCaveLight2->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) - 117.0f; // Keep the x-coordinate constant
			metallicCaveLight2->lightPosition_worldspace.y = 0.0 + 6.0f;
			metallicCaveLight2->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			mat4 lightbulb2ModelMatrix = translate(mat4(), metallicCaveLight2->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb2ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();


			//float radius = 5.0f; // Adjust the radius of the circular motion
			//float angularSpeed = 1.2f; // Adjust the speed of the circular motion

			metallicCaveLight3->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) + 126.0f; // Keep the x-coordinate constant
			metallicCaveLight3->lightPosition_worldspace.y = 0.0 + 6.0f;
			metallicCaveLight3->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			lightSphereShader->uploadMultipleLights(lightSpheresLightVector);

			mat4 lightbulb3ModelMatrix = translate(mat4(), metallicCaveLight3->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb3ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();
		}
	}

}

void renderScreenFBO() {

	// MORNING ROOM
	if (insideRoom[0]) {
		if (useRealTimeRendering) {
			// for Floyd-Steinberg
			//bind the texture
			glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
			// Map the write PBO for data access
			glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
			// Initiate pixel transfer to the read PBO
			glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			GLubyte* morningRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			if (morningRoomImageData == nullptr) {
				std::cerr << "Failed to map buffer" << std::endl;
				return; // Early exit if mapping fails
			}
			bool useGrayscale = false;
			if (!useOriginalTexture) {
				useGrayscale = true;
			}
			else{
				useGrayscale = false;
			}

			Floyd_SteinbergDitheringAlgorithm_RGBA(morningRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);

			// Unmap the write PBO
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			// Update the texture from the write PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// Unbind PBOs
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the shader program for the room

		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
		glUniform1i(quadTextureSamplerLocation, 20);

		quad->bind();
		quad->draw();
	}
	// PEIRENE FOUNTAIN
	else if (insideRoom[1]) {
		if (usePointilism) {
			// pointilism effect

			//setup: get the screen texture
			glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
			// Map the PBO for data access
			glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_peireneFountain);
			// Initiate pixel transfer to the PBO
			glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			// Get the image data from the PBO
			GLubyte* peireneFountainImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
			if (peireneFountainImageData == nullptr) {
				std::cerr << "Failed to map buffer" << std::endl;
				// Check for OpenGL errors
				auto error = glGetError();
				if (error != GL_NO_ERROR) {
					std::cerr << "OpenGL error after mapping buffer: " << error << std::endl;
				}
				return; // Early exit if mapping fails
			}

			try {
				std::vector<Dot> dots = processImageForPointilism(peireneFountainImageData, W_WIDTH, W_HEIGHT, 10000, useRealTimeRendering);
				drawDots(peireneFountainImageData, W_WIDTH, W_HEIGHT, dots);
			}
			catch (...) {
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
				throw; // Re-throw the exception after cleanup
			}
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

			// Update the texture from the write PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_peireneFountain);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// Unbind PBOs
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the shader program for the room

		glActiveTexture(GL_TEXTURE21);
		glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
		glUniform1i(quadTextureSamplerLocation, 21);

		quad->bind();
		quad->draw();
	}
	// DESTROYED ROOM
	else if (insideRoom[2]) {
		if (useRealTimeRendering) {
			// for Floyd-Steinberg
			//bind the texture
			glBindTexture(GL_TEXTURE_2D, roomTextures[2]);
			// Map the write PBO for data access
			glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
			// Initiate pixel transfer to the read PBO
			glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			GLubyte* destroyedRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			bool useGrayscale = false;
			if (useOriginalTexture) {
				useGrayscale = true;
			}
			else {
				useGrayscale = false;
			}

			Floyd_SteinbergDitheringAlgorithm_RGBA(destroyedRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);


			// Unmap the write PBO
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			// Update the texture from the write PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// Unbind PBOs
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the shader program for the room

		glActiveTexture(GL_TEXTURE22);
		glBindTexture(GL_TEXTURE_2D, roomTextures[2]);
		glUniform1i(quadTextureSamplerLocation, 22);

		quad->bind();
		quad->draw();
	}
	// GREAT DRAWING ROOM
	else if (insideRoom[3]) {
		if (useRealTimeRendering) {
			// for Floyd-Steinberg
			//bind the texture
			glBindTexture(GL_TEXTURE_2D, roomTextures[3]);
			// Map the write PBO for data access
			glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
			// Initiate pixel transfer to the read PBO
			glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			GLubyte* greatDrawingRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			bool useGrayscale = false;
			if (useOriginalTexture) {
				useGrayscale = true;
			}
			else {
				useGrayscale = false;
			}

			Floyd_SteinbergDitheringAlgorithm_RGBA(greatDrawingRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);

			// Unmap the write PBO
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			// Update the texture from the write PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// Unbind PBOs
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the shader program for the room

		glActiveTexture(GL_TEXTURE23);
		glBindTexture(GL_TEXTURE_2D, roomTextures[3]);
		glUniform1i(quadTextureSamplerLocation, 23);

		quad->bind();
		quad->draw();
	}
	// METALLIC CAVE
	else if (insideRoom[4]) {
		if (useRealTimeRendering) {
			// for Floyd-Steinberg
			//bind the texture
			glBindTexture(GL_TEXTURE_2D, roomTextures[4]);
			// Map the write PBO for data access
			glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
			// Initiate pixel transfer to the read PBO
			glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			GLubyte* metallicCaveImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			bool useGrayscale = false;
			if (useOriginalTexture) {
				useGrayscale = true;
			}
			else {
				useGrayscale = false;
			}

			Floyd_SteinbergDitheringAlgorithm_RGBA(metallicCaveImageData, W_WIDTH, W_HEIGHT, useGrayscale);

			// Unmap the write PBO
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			// Update the texture from the write PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// Unbind PBOs
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the shader program for the room

		glActiveTexture(GL_TEXTURE24);
		glBindTexture(GL_TEXTURE_2D, roomTextures[4]);
		glUniform1i(quadTextureSamplerLocation, 24);

		quad->bind();
		quad->draw();
	}
	// STUDIO ROOM (DEFAULT)
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(miniMapShader->LoadedShaderProgram); // Should use the default shader program

		glActiveTexture(GL_TEXTURE19); //
		glBindTexture(GL_TEXTURE_2D, roomTextures[5]);
		glUniform1i(quadTextureSamplerLocation, 19);

		quad->bind();
		quad->draw();
	}
}

void renderSubMap() {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(miniMapShader->LoadedShaderProgram);

	// Bind the texture of the normal map provided by bumpMapping_pass
	/*glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, normalTextures[0]);
	glUniform1i(quadTextureSamplerLocation, 7);*/

	// Bind the depth texture
	glUniform1i(glGetUniformLocation(miniMapShader->LoadedShaderProgram, "doDepth"), 0);

	/*
	const int BASE_TEXTURE_UNIT = 27;
	int textureUnit = BASE_TEXTURE_UNIT + minimapTextureIndex;

	
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, portalDepthTextures[minimapTextureIndex]);
	glUniform1i(quadTextureSamplerLocation, textureUnit);
	*/

	const int BASE_TEXTURE_UNIT = 0;
	int textureUnit = BASE_TEXTURE_UNIT + minimapTextureIndex;

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, roomTextures[minimapTextureIndex]);
	glUniform1i(quadTextureSamplerLocation, textureUnit);

	/*glActiveTexture(GL_TEXTURE27);
	glBindTexture(GL_TEXTURE_2D, portalDepthTextures[0]);
	glUniform1i(quadTextureSamplerLocation, 27);*/

	cout << "Texture unit: " << textureUnit << endl;


	miniquad->bind();
	miniquad->draw();

	glUniform1i(glGetUniformLocation(miniMapShader->LoadedShaderProgram, "doDepth"), 0);
}

void renderFromStudioRoom(mat4 viewMatrix, mat4 projectionMatrix) {
	// Render the scene from the studio room
	
	// bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[5]); // 5 is the index of the studio room
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(defaultShader->LoadedShaderProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(defaultShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(defaultShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	//and the morningRoomShaderProgram!
	float t = glfwGetTime();
	float radius = 20.0f; // Adjust the radius of the circular motion
	float angularSpeed = 1.2f; // Adjust the speed of the circular motion

	//vec3{ -0.7, 64.037, -5.01013},

	/* */
	studiolight->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) - 0.7; // 
	studiolight->lightPosition_worldspace.y = 0.0 + 54.037;
	studiolight->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 5.01013;
	/**/
	studiolight->targetPosition = vec3(studiolight->lightPosition_worldspace.x, 0, studiolight->lightPosition_worldspace.z);

	// uploading the light parameters to the shader program
	defaultShader->uploadLight(studiolight);// AndMaterial(light, turquoise);
	
	// Sending the shadow texture to the shaderProgram
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);

	// Sending the light View-Projection matrix to the shader program
	mat4 lightVP = studiolight->lightVP();
	glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

	// creating a model matrix
	mat4 modelMatrix = TranslatingStudioRoom * ScalingStudioRoom;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	// Setting up texture to display on shader program          //  --- Texture Pipeline ---
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, studioRoomDiffuseTexture);			// Assign texture to position 
	//glUniform1i(diffuseColorSampler, 1);						// Assign sampler to that position
	glUniform1i(defaultShader->textureSamplerDiffuse, 1);

	/*
	glActiveTexture(GL_TEXTURE2);								//
	glBindTexture(GL_TEXTURE_2D, modelSpecularTexture);			// Same process for specular texture
	glUniform1i(specularColorSampler, 2);						//
	*/

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, studioRoomEmissionTexture);
	//glUniform1i(emissionColorSampler, 2);
	glUniform1i(defaultShader->textureSamplerEmissive, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, studioRoomRoughnessTexture);
	glUniform1i(defaultShader->textureSamplerRoughness, 3);

	// Inside the fragment shader, there is an if statement whether to use  
	// the material of an object or sample a texture
	glUniform1i(useTextureLocation, 1);


	// Draw studioRoom
	studioRoom->bind();
	studioRoom->draw();


	defaultShader->uploadMaterial(turquoise);
	glUniform1i(useTextureLocation, 0);

	//
	mat4 paintingModelMatrix = translate(mat4(), painting1TranslFactor) * ScalingPainting;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

	defaultShader->uploadMaterial(gold);
	painting->bind();
	painting->draw();


	paintingModelMatrix = translate(mat4(), painting2TranslFactor) * ScalingPainting;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

	painting->bind();
	painting->draw();

	paintingModelMatrix = translate(mat4(), painting3TranslFactor) * ScalingPainting * RotationPainting3;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

	painting->bind();
	painting->draw();

	paintingModelMatrix = translate(mat4(), painting4TranslFactor) * ScalingPainting * RotationPainting4;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

	painting->bind();
	painting->draw();

	paintingModelMatrix = translate(mat4(), painting5TranslFactor) * ScalingPainting * RotationPainting5;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &paintingModelMatrix[0][0]);

	painting->bind();
	painting->draw();

	// re-set the useTextureLocation to 0
	glUniform1i(useTextureLocation, 0);

	// PORTAL FRAME, PORTAL TEXTURE RENDERING

	mat4 ScalingPortalFrame = scale(mat4(), vec3(9));
	mat4 portalFrameModelMatrix0 = translate(mat4(), portalFrame1TranslFactor) * ScalingPortalFrame;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix0[0][0]);

	glUniform1i(usePortalTextureLocation, 1);

	glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
	glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
	glUniform1f(glGetUniformLocation(defaultShader->LoadedShaderProgram, "time"), glfwGetTime());
	if (useNormalMaps && insideRoom[5]) // if effect is enabled and in studio room
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useNormalMaps"), 1);
	else
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useNormalMaps"), 0);

	if (useParallaxMapping && insideRoom[5]) // if effect is enabled and in studio room
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useParallaxMapping"), 1);
	else
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "useParallaxMapping"), 0);

	if (raiseMesh && insideRoom[5]) // if effect is enabled and in studio room
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "raiseMesh"), 1);
	else
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "raiseMesh"), 0);

	// PAINTING OF THE MORNING ROOM
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, portalTextures[0]);
	glUniform1i(defaultShader->textureSampler1, 14);

	if (useNormalMaps) {
		//include bump map of the portal to the morning room
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, normalTextures[0]);
		glUniform1i(defaultShader->textureSamplerNormal, 7);
	}

	if (useParallaxMapping || raiseMesh) {
		//include height map of the portal to the morning room
		glActiveTexture(GL_TEXTURE27);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[0]);
		glUniform1i(defaultShader->textureSampler2, 27);
	}

	portalFrame->bind();
	portalFrame->draw();

	mat4 portalFrameModelMatrix = translate(mat4(), portalFrame2TranslFactor) * ScalingPortalFrame;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

	// PAINTING OF THE PEIRENE FOUNTAIN
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, portalTextures[1]);
	glUniform1i(defaultShader->textureSampler1, 15);
	if (usePointilism && currentRoomIndex == 5) {
		//
		glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
		glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 0);
	}

	if (useNormalMaps) {
		//include bump map of the portal to the peirene fountain
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, normalTextures[1]);
		glUniform1i(defaultShader->textureSamplerNormal, 8);
	}

	if (useParallaxMapping || raiseMesh) {
		//include height map of the portal to the peirene fountain
		glActiveTexture(GL_TEXTURE28);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[1]);
		glUniform1i(defaultShader->textureSampler2, 28);
	}

	portalFrame->bind();
	portalFrame->draw();
	// re-assigning the value to 0
	glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "pointilism"), 0);

	portalFrameModelMatrix = translate(mat4(), portalFrame3TranslFactor) * ScalingPortalFrame * RotationPainting3;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

	// PAINTING OF THE DESTROYED ROOM
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, portalTextures[2]);
	glUniform1i(defaultShader->textureSampler1, 16);
	if (useKuwaharaFilter && currentRoomIndex == 5) {
		//
		glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
		glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 0);
	}

	if (useNormalMaps) {
		//include bump map of the portal to the destroyed room
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, normalTextures[2]);
		glUniform1i(defaultShader->textureSamplerNormal, 9);
	}

	if (useParallaxMapping || raiseMesh) {
		//include height map of the portal to the destroyed room
		glActiveTexture(GL_TEXTURE29);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[2]);
		glUniform1i(defaultShader->textureSampler2, 29);
	}

	portalFrame->bind();
	portalFrame->draw();
	// re-assigning the value to 0
	glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "kuwa"), 0);


	portalFrameModelMatrix = translate(mat4(), portalFrame4TranslFactor) * ScalingPortalFrame * RotationPainting4;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

	// PAINTING OF THE GREAT DRAWING ROOM
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, portalTextures[3]);
	glUniform1i(defaultShader->textureSampler1, 17);
	if (useComicBookEffect && currentRoomIndex == 5) {
		//
		glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
		glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 1);
		//glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "halftoneCounter"), halftoneCounter);
	}
	else {
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 0);
	}

	if (useNormalMaps) {
		//include bump map of the portal to the great drawing room
		glActiveTexture(GL_TEXTURE25);
		glBindTexture(GL_TEXTURE_2D, normalTextures[3]);
		glUniform1i(defaultShader->textureSamplerNormal, 25);
	}

	if (useParallaxMapping || raiseMesh) {
		//include height map of the portal to the great drawing room
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[3]);
		glUniform1i(defaultShader->textureSampler2, 30);
	}

	portalFrame->bind();
	portalFrame->draw();
	// re-assigning the value to 0
	glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "comic"), 0);

	portalFrameModelMatrix = translate(mat4(), portalFrame5TranslFactor) * ScalingPortalFrame * RotationPainting5;
	glUniformMatrix4fv(defaultShader->modelMatrixLocation, 1, GL_FALSE, &portalFrameModelMatrix[0][0]);

	// PAINTING OF THE METALLIC CAVE
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, portalTextures[4]);
	glUniform1i(defaultShader->textureSampler1, 18);
	if (useCRTEffect && currentRoomIndex == 5) {
		//
		glUniform1f(defaultShaderWidthLocation, PORTAL_WIDTH);
		glUniform1f(defaultShaderHeightLocation, PORTAL_HEIGHT);
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 0);
	}

	if (useNormalMaps) {
		//include bump map of the portal to the metallic cave
		glActiveTexture(GL_TEXTURE26);
		glBindTexture(GL_TEXTURE_2D, normalTextures[4]);
		glUniform1i(defaultShader->textureSamplerNormal, 26);
	}

	if (useParallaxMapping || raiseMesh) {
		//include height map of the portal to the metallic cave
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, portalDepthTextures[4]);
		glUniform1i(defaultShader->textureSampler2, 31);
	}

	portalFrame->bind();
	portalFrame->draw();
	// re-assigning the value to 0
	glUniform1i(glGetUniformLocation(defaultShader->LoadedShaderProgram, "crt"), 0);

	glUniform1i(usePortalTextureLocation, 0);//
}

void renderFromPaintingRoom(mat4 viewMatrix, mat4 projectionMatrix, int roomIndex) {
	// Render the scene from the painting room

	// bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[roomIndex]); // roomIndex is the index of the painting room we're transitioning to
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(defaultShader->LoadedShaderProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(defaultShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(defaultShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	vec4 sky_blue = vec4(0.6, 0.8, 1.0, 1.0);
	vec4 grey = vec4(0.5, 0.5, 0.5, 1.0);
	vec4 black = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 color = sky_blue;
	glClearColor(color.r, color.g, color.b, color.a);
	switch (roomIndex) {
	case 0: {// Morning Room
		color = grey;
		glClearColor(color.r, color.g, color.b, color.a);

		glUseProgram(morningRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(morningRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(morningRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 morningRoomModelMatrix = MorningRoomModelMatrix;//TranslatingMorningRoom * ScalingMorningRoom * RotationMorningRoom;
		glUniformMatrix4fv(morningRoomShader->modelMatrixLocation, 1, GL_FALSE, &morningRoomModelMatrix[0][0]);


		glActiveTexture(GL_TEXTURE4);
		if (useRealTimeRendering || useOriginalTexture) {
			glBindTexture(GL_TEXTURE_2D, morningRoomTexture);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, preDitheredMorningRoomTexture);
		}
		glUniform1i(morningRoomShader->textureSampler1, 4);

		// Draw MorningRoom

		morningRoom->bind();
		morningRoom->draw();
		break;

	}
	case 1: {// Peirene Fountain
		glUseProgram(peireneFountainShader->LoadedShaderProgram);

		glUniformMatrix4fv(peireneFountainShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(peireneFountainShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 peireneFountainRoomModelMatrix = PeireneFountainModelMatrix;// TranslatingPeireneFountain* ScalingMorningRoom* RotationPeireneFountain;
		glUniformMatrix4fv(peireneFountainShader->modelMatrixLocation, 1, GL_FALSE, &peireneFountainRoomModelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, peireneFountainTexture);
		glUniform1i(peireneFountainShader->textureSampler1, 6);

		peireneFountain->bind();
		peireneFountain->draw();
		break;
	}
	case 2: {// Destroyed Room
		glUseProgram(destroyedRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(destroyedRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(destroyedRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 destroyedRoomModelMatrix = DestroyedRoomModelMatrix; //TranslatingDestroyedRoom * ScalingDestroyedRoom;
		glUniformMatrix4fv(destroyedRoomShader->modelMatrixLocation, 1, GL_FALSE, &destroyedRoomModelMatrix[0][0]);

		//destroyedRoomShader->uploadMaterial();
		//destroyedRoomShader->uploadLight();

		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_2D, destroyedRoomTexture);
		glUniform1i(destroyedRoomShader->textureSampler1, 13);

		destroyedRoom->bind();
		destroyedRoom->draw();
		break;
	}
	case 3: {// Great Drawing Room
		glUseProgram(greatDrawingRoomShader->LoadedShaderProgram);

		glUniformMatrix4fv(greatDrawingRoomShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(greatDrawingRoomShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 greatDrawingRoomModelMatrix = GreatDrawingRoomModelMatrix;//TranslatingMorningRoom * ScalingMorningRoom * RotationMorningRoom;
		glUniformMatrix4fv(greatDrawingRoomShader->modelMatrixLocation, 1, GL_FALSE, &greatDrawingRoomModelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, greatDrawingRoomTexture);
		glUniform1i(greatDrawingRoomShader->textureSampler1, 5);

		greatDrawingRoom->bind();
		greatDrawingRoom->draw();
		break;
	}
	case 4: {// Metallic Cave
		color = black;
		glClearColor(color.r, color.g, color.b, color.a);

		glUseProgram(metallicCaveShader->LoadedShaderProgram);

		glUniformMatrix4fv(metallicCaveShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(metallicCaveShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

		mat4 metallicCaveModelMatrix = MetallicCaveModelMatrix; //TranslatingMetallicCave;
		glUniformMatrix4fv(metallicCaveShader->modelMatrixLocation, 1, GL_FALSE, &metallicCaveModelMatrix[0][0]);

		metallicCaveShader->uploadMaterial(obsidian);
		metallicCaveShader->uploadMultipleLights(MCLightVector);

		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, metallicCaveNormalTexture);
		glUniform1i(metallicCaveShader->textureSamplerNormal, 10);

		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, metallicCaveInternalGroundAOTexture);
		glUniform1i(metallicCaveShader->textureSampler1, 11);

		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, metallicCaveRoughnessTexture);

		glUniform1i(metallicCaveShader->textureSamplerRoughness, 12);

		metallicCave->bind();
		metallicCave->draw();
		break;
	}
	default:
		break;
	}

	// LIGHTSPHERES
	if (roomIndex == 4 || roomIndex == 5) { // either inside Metallic Cave or Studio Room
		glUseProgram(lightSphereShader->LoadedShaderProgram);

		if (roomIndex == 5) { // Studio Room

			glUniform1i(lightsphereUseWhiteLocation, 0);

			glUniformMatrix4fv(lightSphereShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			glUniformMatrix4fv(lightSphereShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

			mat4 lightsphere1ModelMatrix = translate(mat4(), studiolight->lightPosition_worldspace) * scale(mat4(), vec3(0.2));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightsphere1ModelMatrix[0][0]);

			lightsphere1->bind();
			lightsphere1->draw();
		}

		if (roomIndex == 4) { // Metallic Cave
			std::cout << "Lights should be drawn..." << std::endl;

			glUniform1i(lightsphereUseWhiteLocation, 1);

			glUniformMatrix4fv(lightSphereShader->viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
			glUniformMatrix4fv(lightSphereShader->projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

			float t = glfwGetTime();
			float radius = 0.0625f; // Adjust the radius of the circular motion
			float angularSpeed = 40.2f; // Adjust the speed of the circular motion

			metallicCaveLight1->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) + 0.0; // Keep the x-coordinate constant
			metallicCaveLight1->lightPosition_worldspace.y = 0.0 + 6;
			metallicCaveLight1->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			lightSphereShader->uploadLight(metallicCaveLight1);

			mat4 lightbulb1ModelMatrix = translate(mat4(), metallicCaveLight1->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb1ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();


			//radius = 5.0f; // Adjust the radius of the circular motion
			//angularSpeed = 1.2f; // Adjust the speed of the circular motion

			metallicCaveLight2->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) - 117.0f; // Keep the x-coordinate constant
			metallicCaveLight2->lightPosition_worldspace.y = 0.0 + 6.0f;
			metallicCaveLight2->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			mat4 lightbulb2ModelMatrix = translate(mat4(), metallicCaveLight2->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb2ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();


			//float radius = 5.0f; // Adjust the radius of the circular motion
			//float angularSpeed = 1.2f; // Adjust the speed of the circular motion

			metallicCaveLight3->lightPosition_worldspace.x = radius * std::cos(angularSpeed * t) + 126.0f; // Keep the x-coordinate constant
			metallicCaveLight3->lightPosition_worldspace.y = 0.0 + 6.0f;
			metallicCaveLight3->lightPosition_worldspace.z = radius * std::sin(angularSpeed * t) - 300.0;

			lightSphereShader->uploadMultipleLights(lightSpheresLightVector);

			mat4 lightbulb3ModelMatrix = translate(mat4(), metallicCaveLight3->lightPosition_worldspace) * scale(mat4(), vec3(0.1));
			glUniformMatrix4fv(lightSphereShader->modelMatrixLocation, 1, GL_FALSE, &lightbulb3ModelMatrix[0][0]);

			lightbulb1->bind();
			lightbulb1->draw();
		}
	}
	
}

void mainLoop() {


	studiolight->update();
	mat4 light_proj = studiolight->projectionMatrix;
	mat4 light_view = studiolight->viewMatrix;

	// Task 3.3
	// Create the depth buffer
	//depth_pass(light_view, light_proj);

	// PORTALS
	camera->update();
	camera->pollMovement();

	// For morningRoom
	morningRoomCamera->update();
	mat4 morningRoom_proj = morningRoomCamera->projectionMatrix;
	mat4 morningRoom_view = morningRoomCamera->viewMatrix;

	portal_pass(0, morningRoom_view, morningRoom_proj, morningRoomShader, morningRoom, MorningRoomModelMatrix); //pass for MorningRoom

	// For peireneFountain
	peireneFountainCamera->update();
	mat4 peireneFountain_proj = peireneFountainCamera->projectionMatrix;
	mat4 peireneFountain_view = peireneFountainCamera->viewMatrix;
	portal_pass(1, peireneFountain_view, peireneFountain_proj, peireneFountainShader, peireneFountain, PeireneFountainModelMatrix); //pass for PeireneFountain

	// For destroyedRoom
	destroyedRoomCamera->update();
	mat4 destroyedRoom_proj = destroyedRoomCamera->projectionMatrix;
	mat4 destroyedRoom_view = destroyedRoomCamera->viewMatrix;
	portal_pass(2, destroyedRoom_view, destroyedRoom_proj, destroyedRoomShader, destroyedRoom, DestroyedRoomModelMatrix); // pass for DestroyedRoom

	// For greatDrawingRoom
	greatDrawingRoomCamera->update();
	mat4 greatDrawingRoom_proj = greatDrawingRoomCamera->projectionMatrix;
	mat4 greatDrawingRoom_view = greatDrawingRoomCamera->viewMatrix;
	portal_pass(3, greatDrawingRoom_view, greatDrawingRoom_proj, greatDrawingRoomShader, greatDrawingRoom, GreatDrawingRoomModelMatrix); // pass for GreatDrawingRoom
	//depth_pass(greatDrawingRoom_view, greatDrawingRoom_proj);

	// For metallicCave
	metallicCaveCamera->update();
	mat4 metallicCave_proj = metallicCaveCamera->projectionMatrix;
	mat4 metallicCave_view = metallicCaveCamera->viewMatrix;
	portal_pass(4, metallicCave_view, metallicCave_proj, metallicCaveShader, metallicCave, MetallicCaveModelMatrix); // pass for MetallicCave

	bumpMapping_pass(0, morningRoom_view, morningRoom_proj, portalShader, morningRoom, MorningRoomModelMatrix);
	bumpMapping_pass(1, peireneFountain_view, peireneFountain_proj, portalShader, peireneFountain, PeireneFountainModelMatrix);
	bumpMapping_pass(2, destroyedRoom_view, destroyedRoom_proj, portalShader, destroyedRoom, DestroyedRoomModelMatrix);
	bumpMapping_pass(3, greatDrawingRoom_view, greatDrawingRoom_proj, portalShader, greatDrawingRoom, GreatDrawingRoomModelMatrix);
	bumpMapping_pass(4, metallicCave_view, metallicCave_proj, portalShader, metallicCave, MetallicCaveModelMatrix);

	cout << "Entering main loop..." << endl;
	do {
		// TODO: change location
		//send width and height to the shader

		if (useKuwaharaFilter && !insideRoom[5]) // if effect is enabled and not in studio room
			glUniform1i(glGetUniformLocation(miniMapProgram, "kuwa"), 1);
		else
			glUniform1i(glGetUniformLocation(miniMapProgram, "kuwa"), 0);
		if (usePointilism && insideRoom[1]) // if effect is enabled and in peireneFountain
			glUniform1i(glGetUniformLocation(miniMapProgram, "pointilism"), 1);
		else
			glUniform1i(glGetUniformLocation(miniMapProgram, "pointilism"), 0);
		if (useComicBookEffect && !insideRoom[5]) { // if effect is enabled and not in studio room
			glUniform1i(glGetUniformLocation(miniMapProgram, "comic"), 1);
			//glUniform1i(glGetUniformLocation(miniMapProgram, "halftoneOption"), halftoneCounter);
		}
		else
			glUniform1i(glGetUniformLocation(miniMapProgram, "comic"), 0);
		if (useCRTEffect && !insideRoom[5]) // if effect is enabled and not in studio room
			glUniform1i(glGetUniformLocation(miniMapProgram, "crt"), 1);
		else
			glUniform1i(glGetUniformLocation(miniMapProgram, "crt"), 0);
		
		glUniform1f(quadWidthLocation, W_WIDTH); // consider using textureSize in the shader instead
		glUniform1f(quadHeightLocation, W_HEIGHT);
		glUniform1f(glGetUniformLocation(miniMapProgram, "time"), glfwGetTime());


		/*glUniform1i(numDotsLocation, 0);
		glUniform2fv(dotsCentersLocation, 0, {});
		glUniform1fv(dotsRadiiLocation, 0, {});*/


		studiolight->update();
		mat4 light_proj = studiolight->projectionMatrix;
		mat4 light_view = studiolight->viewMatrix;


		// Task 3.5
		// Create the depth buffer
		//depth_pass(light_view, light_proj);

		// Getting camera information
		/*camera->update();
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;*/



		//lighting_pass(viewMatrix, projectionMatrix);

		// PORTAL
		camera->update();
		//if (!transitioning)
			camera->pollMovement();
		camera->setProjectionMatrix(16.0f/9.0f, 0.1f, 200.0f);
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;
		if (!transitioning) {
			// Normal rendering of the scene

			if (insideRoom[5]) {

				updateAllPortalCameras(); 

				// For morningRoom
				mat4 morningRoom_proj = morningRoomCamera->projectionMatrix;
				mat4 morningRoom_view = morningRoomCamera->viewMatrix;

				// For peireneFountain
				mat4 peireneFountain_proj = peireneFountainCamera->projectionMatrix;
				mat4 peireneFountain_view = peireneFountainCamera->viewMatrix;

				// For destroyedRoom
				mat4 destroyedRoom_proj = destroyedRoomCamera->projectionMatrix;
				mat4 destroyedRoom_view = destroyedRoomCamera->viewMatrix;

				// For greatDrawingRoom
				mat4 greatDrawingRoom_proj = greatDrawingRoomCamera->projectionMatrix;
				mat4 greatDrawingRoom_view = greatDrawingRoomCamera->viewMatrix;

				// For metallicCave
				mat4 metallicCave_proj = metallicCaveCamera->projectionMatrix;
				mat4 metallicCave_view = metallicCaveCamera->viewMatrix;

				//indexes similar to portalTeleportLocationVector and portalTeleportDirectionVector
				portal_pass(0, morningRoom_view, morningRoom_proj, morningRoomShader, morningRoom, MorningRoomModelMatrix); //pass for MorningRoom
				portal_pass(1, peireneFountain_view, peireneFountain_proj, peireneFountainShader, peireneFountain, PeireneFountainModelMatrix); //pass for PeireneFountain
				portal_pass(2, destroyedRoom_view, destroyedRoom_proj, destroyedRoomShader, destroyedRoom, DestroyedRoomModelMatrix); // pass for DestroyedRoom
				portal_pass(3, greatDrawingRoom_view, greatDrawingRoom_proj, greatDrawingRoomShader, greatDrawingRoom, GreatDrawingRoomModelMatrix); // pass for GreatDrawingRoom
				portal_pass(4, metallicCave_view, metallicCave_proj, metallicCaveShader, metallicCave, MetallicCaveModelMatrix); // pass for MetallicCave

				if (useNormalMaps) {
					// get the bump map for the portal images
					bumpMapping_pass(0, morningRoom_view, morningRoom_proj, portalShader, morningRoom, MorningRoomModelMatrix);
					bumpMapping_pass(1, peireneFountain_view, peireneFountain_proj, portalShader, peireneFountain, PeireneFountainModelMatrix);
					bumpMapping_pass(2, destroyedRoom_view, destroyedRoom_proj, portalShader, destroyedRoom, DestroyedRoomModelMatrix);
					bumpMapping_pass(3, greatDrawingRoom_view, greatDrawingRoom_proj, portalShader, greatDrawingRoom, GreatDrawingRoomModelMatrix);
					bumpMapping_pass(4, metallicCave_view, metallicCave_proj, portalShader, metallicCave, MetallicCaveModelMatrix);
				}

				glClearColor(0.6, 0.8, 1.0, 1.0);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			// END PORTAL 
			else {
				vec4 sky_blue = vec4(0.6, 0.8, 1.0, 1.0);
				vec4 grey = vec4(0.5, 0.5, 0.5, 1.0);
				vec4 black = vec4(0.0, 0.0, 0.0, 0.0);
				vec4 color = sky_blue;
				switch (currentRoomIndex) {
				case 0: // Morning Room
					color = grey;
					break;
				case 4: // Studio Room
					color = black;
					break;
				default:
					color = sky_blue;
					break;
				}
				glClearColor(color.r, color.g, color.b, color.a);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			//*/

			/*metallicCaveLight1->update();
			mat4 met_light_proj = metallicCaveLight1->projectionMatrix;
			mat4 met_light_view = metallicCaveLight1->viewMatrix;*/

			//depth_pass(met_light_view, met_light_proj);

			if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
				//lighting_pass(light_view, light_proj);
				lighting_pass(studioRoomCamera->viewMatrix, studioRoomCamera->projectionMatrix);
			}
			else {
				// Render the scene from camera's perspective
				lighting_pass(viewMatrix, projectionMatrix);
			}

			//*/

			// Task 2.2:
			renderScreenFBO();

			if (showMinimap)
				renderSubMap();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		// For when the user has entered a portal and a transition is happening
		else {
			// Determine if the transition is from Studio Room --> Portal
			// or from Portal --> Studio Room by checking insideRoom[currentRoomIndex] // insideRoom[0] is Morning Room, insideRoom[1] is Peirene Fountain,
			// insideRoom[2] is Destroyed Room, insideRoom[3] is Great Drawing Room, insideRoom[4] is Metallic Cave, insideRoom[5] is Studio Room.

			//-----------
			// Probably needs a studio room camera too -- imagine the scenario: You're in the middle of some other room, then press space. What's the effect supposed to be.
			// Probably place a studioRoom camera with the same exact initialization as the user camera, but leave it unchanged and it will only be used for transitions from portal to studio room.

			// Also needs a time limit -- where to define it?
			//-----------

			mat4 _projectionMatrix;
			mat4 _viewMatrix;

			if (startingTransitionTime < 0.0f) {
				startingTransitionTime = glfwGetTime();

			}

			if (insideRoom[5]) {
				// Transition from Studio Room to Portal

				// Render the scene from the camera's perspective as normal,
				// store it in a texture
				//camera->update();
				//projectionMatrix = camera->projectionMatrix;
				//viewMatrix = camera->viewMatrix;
				//renderFromStudioRoom(projectionMatrix, viewMatrix); // needs to also draw portals
				lighting_pass(viewMatrix, projectionMatrix); // this does the same thing as the above line, but in the default frame buffer

				// Render the scene from the portal camera's perspective,
				// store it in a texture
				//roomCameraVector[transitioningToRoomIndex]->setDirection(roomTeleportDirectionsVector[transitioningToRoomIndex]);
				updateAllPortalCameras();
				mat4 prev_proj_matrix = roomCameraVector[transitioningToRoomIndex]->projectionMatrix;
				float main_aspect = 16.0f / 9.0f;
				float near = 0.1f;
				float far = 200.0f;
				roomCameraVector[transitioningToRoomIndex]->setProjectionMatrix(main_aspect, near, far);
				roomCameraVector[transitioningToRoomIndex]->update();
				//roomCameraVector[transitioningToRoomIndex]->pollMovement();
				cout << "(" << roomCameraVector[transitioningToRoomIndex]->position.x << ", " << roomCameraVector[transitioningToRoomIndex]->position.y << ", " << roomCameraVector[transitioningToRoomIndex]->position.z << ")" << endl;
				mat4 room_view = roomCameraVector[transitioningToRoomIndex]->viewMatrix;
				mat4 room_proj = roomCameraVector[transitioningToRoomIndex]->projectionMatrix;
				renderFromPaintingRoom(room_view, room_proj, transitioningToRoomIndex);

				// reset the projection matrix
				roomCameraVector[transitioningToRoomIndex]->projectionMatrix = prev_proj_matrix;
				//roomCameraVector[transitioningToRoomIndex]->update();

				// send the 2 textures to the shader, send the time to the shader
				// and adjust a or other effects of the mixing of the two textures
				float currentTime = glfwGetTime();
				float deltaTime = currentTime - startingTransitionTime;

				cout << "Current Time: " << currentTime << endl;
				cout << "Starting Transition Time: " << startingTransitionTime << endl;
				cout << "Time spent transitioning: " << deltaTime << endl;

				glUseProgram(transitionShader->LoadedShaderProgram);

				glUniform1f(glGetUniformLocation(transitionShader->LoadedShaderProgram, "dt"), deltaTime);
				glUniform1f(glGetUniformLocation(transitionShader->LoadedShaderProgram, "transitionDuration"), transitionDuration);
				glUniform1i(glGetUniformLocation(transitionShader->LoadedShaderProgram, "transitionSelection"), transitionSelection);

				glActiveTexture(GL_TEXTURE19); // studio room texture
				glBindTexture(GL_TEXTURE_2D, roomTextures[5]);
				glUniform1i(transitionShader->textureSampler1, 19);

				switch (transitioningToRoomIndex) {
				case 0: {// Morning Room
					if (useRealTimeRendering) {
						glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[0]);
						// for Floyd-Steinberg
						//bind the texture
						glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
						// Map the write PBO for data access
						glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
						// Initiate pixel transfer to the read PBO
						glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // glReadPixels reads from the framebuffer
						

						GLubyte* morningRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
						if (morningRoomImageData == nullptr) {
							std::cerr << "Failed to map buffer" << std::endl;
							return; // Early exit if mapping fails
						}
						bool useGrayscale = false;
						if (!useOriginalTexture) {
							useGrayscale = true;
						}
						else {
							useGrayscale = false;
						}

						Floyd_SteinbergDitheringAlgorithm_RGBA(morningRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);

						// Unmap the write PBO
						glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
						// Update the texture from the write PBO
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
						// Unbind PBOs
						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
					}

					glActiveTexture(GL_TEXTURE20);
					glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
					glUniform1i(transitionShader->textureSampler2, 20);
					break;
				}
				case 1: {// Peirene Fountain
					if (usePointilism) {
						// pointilism effect
						glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[1]);
						//setup: get the screen texture
						glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
						// Map the PBO for data access
						glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_peireneFountain);
						// Initiate pixel transfer to the PBO
						glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

						// Get the image data from the PBO
						GLubyte* peireneFountainImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
						if (peireneFountainImageData == nullptr) {
							std::cerr << "Failed to map buffer" << std::endl;
							// Check for OpenGL errors
							auto error = glGetError();
							if (error != GL_NO_ERROR) {
								std::cerr << "OpenGL error after mapping buffer: " << error << std::endl;
							}
							return; // Early exit if mapping fails
						}

						try {
							std::vector<Dot> dots = processImageForPointilism(peireneFountainImageData, W_WIDTH, W_HEIGHT, 10000, useRealTimeRendering);
							drawDots(peireneFountainImageData, W_WIDTH, W_HEIGHT, dots);
						}
						catch (...) {
							glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
							throw; // Re-throw the exception after cleanup
						}
						glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

						// Update the texture from the write PBO
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_peireneFountain);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
						// Unbind PBOs
						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
					}
					glActiveTexture(GL_TEXTURE21);
					glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
					glUniform1i(transitionShader->textureSampler2, 21);
					break;
				}
				case 2: // Destroyed Room
					glActiveTexture(GL_TEXTURE22);
					glBindTexture(GL_TEXTURE_2D, roomTextures[2]);
					glUniform1i(transitionShader->textureSampler2, 22);
					break;
				case 3: // Great Drawing Room
					glActiveTexture(GL_TEXTURE23);
					glBindTexture(GL_TEXTURE_2D, roomTextures[3]);
					glUniform1i(transitionShader->textureSampler2, 23);
					break;
				case 4: // Metallic Cave
					glActiveTexture(GL_TEXTURE24);
					glBindTexture(GL_TEXTURE_2D, roomTextures[4]);
					glUniform1i(transitionShader->textureSampler2, 24);
					break;
				default:
					break;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glViewport(0, 0, W_WIDTH, W_HEIGHT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				quad->bind();
				quad->draw();

				// Determine if the transition is ended by checking the time
				if (deltaTime > transitionDuration) {
						transitioning = false;
						insideRoom[5] = false;
						insideRoom[transitioningToRoomIndex] = true;
						currentRoomIndex = transitioningToRoomIndex;
						// finally, also set the camera to the portal camera
				 		camera->position = roomCameraVector[currentRoomIndex]->position;
						camera->verticalAngle = roomCameraVector[currentRoomIndex]->verticalAngle;
						camera->horizontalAngle = roomCameraVector[currentRoomIndex]->horizontalAngle;

						startingTransitionTime = -1.0f; // reset the starting transition time
						transitioningToRoomIndex = 0; // reset the transitioning to room index
						
				 }
			}
			else {
				// Transition from Portal to Studio Room
				// 
				//currentRoomIndex = 5; // Don't change the currentRoomIndex yet, do it once the transition is over since we need it to determine where we are

				// Render the scene from the default camera's perspective,
				// which is in a room, store it in a texture
				/* */
				camera->update();
				projectionMatrix = camera->projectionMatrix;
				viewMatrix = camera->viewMatrix;
				//renderFromPaintingRoom(viewMatrix, projectionMatrix, currentRoomIndex);
				lighting_pass(viewMatrix, projectionMatrix); 
				// */
				

				// Render the scene from the studio room camera's perspective,
				// store it in a texture
				studioRoomCamera->update();
				mat4 prev_proj_matrix = studioRoomCamera->projectionMatrix;
				float main_aspect = 16.0f / 9.0f;
				float near = 0.1f;
				float far = 200.0f;
				studioRoomCamera->setProjectionMatrix(main_aspect, near, far);
				studioRoomCamera->update();
				//studioRoomCamera->pollMovement();
				mat4 studioRoom_proj = studioRoomCamera->projectionMatrix;
				mat4 studioRoom_view = studioRoomCamera->viewMatrix;
				//renderFromStudioRoom(studioRoom_proj, studioRoom_view); // needs to also draw portals
				
				vec4 sky_blue = vec4(0.6, 0.8, 1.0, 1.0);
				vec4 color = sky_blue;
				glClearColor(color.r, color.g, color.b, color.a);

				insideRoom[5] = true;
				int prev_room_index = currentRoomIndex;
				currentRoomIndex = 5;
				lighting_pass(studioRoom_view, studioRoom_proj); // this does the same thing as the above line, but in the default frame buffer
				currentRoomIndex = prev_room_index;
				insideRoom[5] = false;
				// reset the projection matrix
				studioRoomCamera->projectionMatrix = prev_proj_matrix;
				//studioRoomCamera->update();

				// send the 2 textures to the shader, send the time to the shader
				// and adjust a or other effects of the mixing of the two textures
				float currentTime = glfwGetTime();
				float deltaTime = currentTime - startingTransitionTime;

				glUseProgram(transitionShader->LoadedShaderProgram);

				glUniform1f(glGetUniformLocation(transitionShader->LoadedShaderProgram, "dt"), deltaTime);
				glUniform1f(glGetUniformLocation(transitionShader->LoadedShaderProgram, "transitionDuration"), transitionDuration);
				glUniform1i(glGetUniformLocation(transitionShader->LoadedShaderProgram, "transitionSelection"), transitionSelection);

				glActiveTexture(GL_TEXTURE19); // studio room texture
				glBindTexture(GL_TEXTURE_2D, roomTextures[5]);
				glUniform1i(transitionShader->textureSampler2, 19);

				vec4 grey = vec4(0.5, 0.5, 0.5, 1.0);
				vec4 black = vec4(0.0, 0.0, 0.0, 0.0);
				color = sky_blue;
				glClearColor(color.r, color.g, color.b, color.a);
				switch (currentRoomIndex) {
				case 0: {// Morning Room
					if (useRealTimeRendering) {
						glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[0]);
						// for Floyd-Steinberg
						//bind the texture
						glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
						// Map the write PBO for data access
						glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_morningRoom);
						// Initiate pixel transfer to the read PBO
						glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // glReadPixels reads from the framebuffer


						GLubyte* morningRoomImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
						if (morningRoomImageData == nullptr) {
							std::cerr << "Failed to map buffer" << std::endl;
							return; // Early exit if mapping fails
						}
						bool useGrayscale = false;
						if (!useOriginalTexture) {
							useGrayscale = true;
						}
						else {
							useGrayscale = false;
						}

						Floyd_SteinbergDitheringAlgorithm_RGBA(morningRoomImageData, W_WIDTH, W_HEIGHT, useGrayscale);

						// Unmap the write PBO
						glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
						// Update the texture from the write PBO
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_morningRoom);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
						// Unbind PBOs
						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
					}
					color = grey;
					glClearColor(color.r, color.g, color.b, color.a);

					glActiveTexture(GL_TEXTURE20);
					glBindTexture(GL_TEXTURE_2D, roomTextures[0]);
					glUniform1i(transitionShader->textureSampler1, 20);
					break;
				}
				case 1: {// Peirene Fountaie
					if (usePointilism) {
						// pointilism effect

						glBindFramebuffer(GL_FRAMEBUFFER, roomFBOs[1]);
						//setup: get the screen texture
						glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
						// Map the PBO for data access
						glBindBuffer(GL_PIXEL_PACK_BUFFER, readPBO_peireneFountain);
						// Initiate pixel transfer to the PBO
						glReadPixels(0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

						// Get the image data from the PBO
						GLubyte* peireneFountainImageData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
						if (peireneFountainImageData == nullptr) {
							std::cerr << "Failed to map buffer" << std::endl;
							// Check for OpenGL errors
							auto error = glGetError();
							if (error != GL_NO_ERROR) {
								std::cerr << "OpenGL error after mapping buffer: " << error << std::endl;
							}
							return; // Early exit if mapping fails
						}

						try {
							std::vector<Dot> dots = processImageForPointilism(peireneFountainImageData, W_WIDTH, W_HEIGHT, 10000, useRealTimeRendering);
							drawDots(peireneFountainImageData, W_WIDTH, W_HEIGHT, dots);
						}
						catch (...) {
							glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
							throw; // Re-throw the exception after cleanup
						}
						glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

						// Update the texture from the write PBO
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, readPBO_peireneFountain);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W_WIDTH, W_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
						// Unbind PBOs
						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
					}

					glActiveTexture(GL_TEXTURE21);
					glBindTexture(GL_TEXTURE_2D, roomTextures[1]);
					glUniform1i(transitionShader->textureSampler1, 21);
					break;
				}
				case 2: {// Destroyed Room
					glActiveTexture(GL_TEXTURE22);
					glBindTexture(GL_TEXTURE_2D, roomTextures[2]);
					glUniform1i(transitionShader->textureSampler1, 22);
					break;
				}
				case 3: {// Great Drawing Room
					glActiveTexture(GL_TEXTURE23);
					glBindTexture(GL_TEXTURE_2D, roomTextures[3]);
					glUniform1i(transitionShader->textureSampler1, 23);
					break;
				}
				case 4: {// Metallic Cave
					color = black;
					glClearColor(color.r, color.g, color.b, color.a);

					glActiveTexture(GL_TEXTURE24);
					glBindTexture(GL_TEXTURE_2D, roomTextures[4]);
					glUniform1i(transitionShader->textureSampler1, 24);
					break;
				}
				default:
					break;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glViewport(0, 0, W_WIDTH, W_HEIGHT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				quad->bind();
				quad->draw();

				// Determine if the transition is ended by checking the time
				if (deltaTime > transitionDuration) {
						transitioning = false;
						insideRoom[currentRoomIndex] = false;
						insideRoom[transitioningToRoomIndex] = true; // 5
						currentRoomIndex = transitioningToRoomIndex; // 5
						// finally, also set the camera to the studio room camera
				 		camera->position = studioRoomCamera->position;
						camera->verticalAngle = studioRoomCamera->verticalAngle;
						camera->horizontalAngle = studioRoomCamera->horizontalAngle;

						startingTransitionTime = -1.0f; // reset the starting transition time
						transitioningToRoomIndex = 0; // reset the transitioning to room index
				 }

			}

			glfwSwapBuffers(window);
			glfwPollEvents(); // Don't poll events while transitioning

		}

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}

void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::cout << "Loading time about 122 seconds... Hang tight... " << endl;

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Gray background color
	glClearColor(0.6, 0.8, 1, 1);//glClearColor(0.5, 0.7, 1, 1);//glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	//keyboard events
	glfwSetKeyCallback(window, pollKeyboard);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	// Log
	logGLParameters();

	// Create camera
	float main_aspect = 16.0f / 9.0f;
	float near = 0.1f;
	float far = 100.0f;
	camera = new Camera(window);
	camera->setProjectionMatrix(main_aspect, near, far);

	// Create the studio room camera
	studioRoomCamera = new Camera(window);
	studioRoomCamera->setProjectionMatrix(main_aspect, near, far); // same as the main camera for transition purposes

	// Task 1.1 Creating a light source
	// Creating a custom light
	studiolight = new Light(window,
		vec4{ 1, 1, 1, 1 }, // La
		vec4{ 1, 1, 1, 1 }, // Ld
		vec4{ 0.5, 0.5, 0.5, 1 },
		1, //kc
		0.09f, //kl
		0.032f, //kq
		vec3{ -0.7, 64.037, -5.01013 }, //position
		50.0f + 40.0f// power
	);
	//studiolight->orthoProj = true;

	Light* deadLight = new Light(window, //needs to be fixed, results in one more iteration in the fragment shader
		vec4{ 0, 0, 0, 0 },
		vec4{ 0, 0, 0, 0 },
		vec4{ 0, 0, 0, 0 },
		1,
		0.0f,
		0.0f,
		vec3{ 0.0, 6.0, -400.0 },
		50.0f
	);

	metallicCaveLight1 = new Light(window,
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		1,
		0.09f,
		0.032f,
		vec3{ 0.0, 6.0, -300.0 },
		25.0f
	);

	metallicCaveLight2 = new Light(window,
		//vec4{1, 0.8, 0.8, 1},
		//vec4{ 1, 0.8, 0.8, 1 },
		//vec4{ 1, 0.8, 0.8, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		1,
		0.09f,
		0.032f,
		vec3{ 126.0, 6.0, -300.0 },
		25.0f
	);

	metallicCaveLight3 = new Light(window,
		//vec4{ 1, 0.8, 0.8, 1 },
		//vec4{ 1, 0.8, 0.8, 1 },
		//vec4{ 1, 0.8, 0.8, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		1,
		0.09f,
		0.032f,
		vec3{ -115.0, 6.0, -300.0 },
		25.0f
	);
	//metallicCaveLight1->orthoProj = true;

	MCLightVector = { metallicCaveLight1, metallicCaveLight2, metallicCaveLight3 };
	lightSpheresLightVector = { studiolight, metallicCaveLight1, metallicCaveLight2, metallicCaveLight3 };

	painting1TranslFactor = vec3(-32.5, 7, -17);
	painting2TranslFactor = vec3(-32.5, 7, +16);
	painting3TranslFactor = vec3(-18, 7, 44.5);
	painting4TranslFactor = vec3(18, 7, 44.5);
	painting5TranslFactor = vec3(-3, 16, -42);

	vec3 offsetPortalFrameFactor_LeftWall = vec3(-2.2, 2.9, 1.4); //offset between the frame and painting model
	vec3 offsetPortalFrameFactor_BackWall = vec3(1.4, 2.9, 2.2);
	vec3 offsetPortalFrameFactor_ForwardWall = vec3(-1.4, 2.9, -2.2);
	portalFrame1TranslFactor = painting1TranslFactor + offsetPortalFrameFactor_LeftWall;
	portalFrame2TranslFactor = painting2TranslFactor + offsetPortalFrameFactor_LeftWall;
	portalFrame3TranslFactor = painting3TranslFactor + offsetPortalFrameFactor_BackWall;
	portalFrame4TranslFactor = painting4TranslFactor + offsetPortalFrameFactor_BackWall;
	portalFrame5TranslFactor = painting5TranslFactor + offsetPortalFrameFactor_ForwardWall;
	
	float s = 0.3;
	ScalingPainting = scale(mat4(), vec3(s));

	//coordinates to Teleport to:	1# Morning Room					2# Peirene Fountain
	roomTeleportLocationsVector = { vec3(-69.09812, 18.0, 2.5693), vec3(141.3231, 12.0, -105.989),
		//  3# Destroyed Room				4# Great Drawing Room		5# Metallic Cave
			vec3(-103.034, 9.6974, -75.4632),  vec3(84.15832, 13.0, 1.35958),  vec3(5.9789, 6, -345.134), //vec3(2.10957, 6.0, -245.103) };
		// 6# Studio Room
		vec3(0, 16, 0) };

	paintingsTranslFactorVector = { painting1TranslFactor, painting2TranslFactor, painting3TranslFactor, 
		painting4TranslFactor, painting5TranslFactor };

	//coordinates to Look At, normalized at setDirection
	//								     1# Morning Room               2# Peirene Fountain
	roomTeleportDirectionsVector = { vec3(0.940236, 6, -216.94), vec3(-108.097, 9.6974, -81.7389),
		//		    3# Destroyed Room			4# Great Drawing Room	  5# Metallic Cave
			vec3(-108.097, 9.6974, -81.7389), vec3(110.719, 13, 22.2646), vec3(0.940236, 6, -216.94), //vec3(-3.04979, 6, 324.299) };
		// 6# Studio Room
		vec3(-29.3934, 16, 0.970362) };

			   //{ inMorningRoom, inPeireneFountain, inDestroyedRoom, inGreatDrawingRoom, inMetallicCave, inStudioRoom };
	insideRoom = { false, false, false, false, false, true};

	// VIRTUAL CAMERAS
	studioRoomCamera->position = roomTeleportLocationsVector[5];
	studioRoomCamera->setDirection(roomTeleportDirectionsVector[5]);

	float room_aspect = 0.794;
	near = 1.0f;
	far = 100.0f;
	morningRoomCamera = new Camera(window);
	morningRoomCamera->position = roomTeleportLocationsVector[0];
	morningRoomCamera->setDirection(roomTeleportDirectionsVector[4]);
	//morningRoomCamera->FoV = 60.0f;
	morningRoomCamera->setProjectionMatrix(room_aspect, near, far);

	peireneFountainCamera = new Camera(window);
	peireneFountainCamera->position = roomTeleportLocationsVector[1];
	peireneFountainCamera->setDirection(roomTeleportDirectionsVector[3]);
	//peireneFountainCamera->FoV = 60.0f;
	peireneFountainCamera->setProjectionMatrix(room_aspect, near, far);

	// For destroyedRoom
	destroyedRoomCamera = new Camera(window);
	destroyedRoomCamera->position = roomTeleportLocationsVector[2];
	destroyedRoomCamera->setDirection(roomTeleportDirectionsVector[2]);
	//destroyedRoomCamera->FoV = 60.0f;
	destroyedRoomCamera->setProjectionMatrix(room_aspect, near, far*5);

	// For greatDrawingRoom
	greatDrawingRoomCamera = new Camera(window);
	greatDrawingRoomCamera->position = roomTeleportLocationsVector[3];
	greatDrawingRoomCamera->setDirection(roomTeleportDirectionsVector[3]);
	//greatDrawingRoomCamera->FoV = 60.0f;
	greatDrawingRoomCamera->setProjectionMatrix(room_aspect, near, far);

	// For metallicCave
	metallicCaveCamera = new Camera(window);
	metallicCaveCamera->position = roomTeleportLocationsVector[4];
	metallicCaveCamera->setDirection(roomTeleportDirectionsVector[4]);
	//metallicCaveCamera->FoV = 60.0f;
	metallicCaveCamera->setProjectionMatrix(room_aspect, near, far);

	roomCameraVector = { morningRoomCamera, peireneFountainCamera, destroyedRoomCamera, greatDrawingRoomCamera, metallicCaveCamera, studioRoomCamera };

}

int main(void) {
	try {
		initialize();
		createContext();
		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}