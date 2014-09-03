
#ifndef ZGRANNY_H
#define ZGRANNY_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "granny/granny.h"
#include "GlShaderProgram.h"
#include <glm/glm.hpp>

typedef struct {
	glm::mat4 *model;
	glm::mat4 *view;
	glm::mat4 *projection;
	
	glm::vec4 *viewInfo; //x = farPlane, y = nearPlane, z = screen width, w = screen height
	glm::vec4 *data; //x=current time, y=deltatime, z = wind direction X, w = wind direction Z;
	
	glm::vec4 *viewPos; // xyz = view pos, w = wind speed
	glm::mat4 *lightPropertyMatrix;
	glm::mat4x3 *fogPropertyMatrix;
	
	float opacityFade;
	float characterHeight;
	float characterHeightContrast;
	float backLightContrast;
	float backLightIntensity;
	glm::vec4 *color1;
	glm::vec4 *color2;
	glm::vec4 *color3;
	glm::vec4 *color4;
	glm::vec4 *color5;
	float fillLightContrast;
	float fillLightIntensity;
	float rimLightContrast;
	float rimLightIntensity;
	glm::vec4 *color1Specular;
	glm::vec4 *color2Specular;
	glm::vec4 *color3Specular;
	glm::vec4 *color4Specular;
	glm::vec4 *color5Specular;
} renderInfo_t;

void loadTexture(const GLubyte *pixelBuffer, int bufferSize, int width, int height, GLuint format);

struct VertexRGB {
	int r;
	int g;
	int b;
	int a;
};

struct ZGrannyTexture {
	char *name;
		// The name used for this textures when meshes reference it

	unsigned int textureHandle;
		// The handle GL gave me when I submitted the ZGrannyTexture image
};

struct ZGrannyMesh {
	granny_mesh *grannyMesh;
		// Granny's mesh information

	granny_mesh_binding *grannyBinding;
		// The binding between this mesh and the model its deformed with

	granny_mesh_deformer *grannyDeformer;
		// The deformer that I'm using to deform the ZGrannyMesh

	unsigned int displayList;
		// If it's a rigid ZGrannyMesh, the GL display list handle I generated
		// for it at startup

	int textureCount;
	ZGrannyTexture **textureReferences;
		// The textures used by this ZGrannyMesh
};

struct ZGrannyModel {
	float matrix[4][4];
		// The matrix that says where this ZGrannyModel should be placed in the world
	
	granny_skeleton *skeleton;
	
	granny_model_instance *grannyInstance;
		// Granny's instance of this model

	granny_world_pose *worldPose;
		// The current world-space state of the model
	
	granny_local_pose *localPose;

	int meshCount;	  
	ZGrannyMesh *meshes;
		// The meshes that comprise this model
};

struct ZGrannyScene {
	granny_camera camera;
		// The current camera

	granny_file *loadedFile;
		// The Granny file I've loaded

	int textureCount;
	ZGrannyTexture *textures;
		// All the textures I've loaded

	int modelCount;
	ZGrannyModel *models;
		// All the models I've loaded

	int maxBoneCount;
	granny_local_pose *sharedLocalPose;
		// An animation blending buffer that I use for all models, since
		// it's contents don't need to be preserved
};

struct MeshAttachmentPoint {
	std::string meshName;
	std::string boneName;
};

ZGrannyTexture *zGrannyFindTexture( ZGrannyScene *scene, granny_material *grannyMaterial );
void zGrannyCreateTexture( ZGrannyTexture *texture, granny_texture *grannyTexture );

void zGrannyCreateModel(ZGrannyModel *model, ZGrannyScene *inScene, granny_model *grannyModel , std::vector<GLuint > &textures);
void zGrannyCreateMesh(ZGrannyMesh *mesh, granny_mesh *grannyMesh, std::vector<GLuint > &textures, granny_model_instance *inModel, ZGrannyScene *inScene );

void zGrannyRenderModel(ZGrannyScene *inScene, ZGrannyModel *model, std::vector<GLuint > &textures, VertexRGB *vertexRgb , VertexRGB *vertexRgb2, GlShaderProgram *shaderProgram, renderInfo_t *renderInfo);
void zGrannyRenderMesh2( ZGrannyMesh *mesh, granny_pnt332_vertex *vertices, std::vector<GLuint > &textures, VertexRGB *rgbData, GlShaderProgram *shaderProgram);
void zGrannyRenderMesh3( ZGrannyMesh *mesh, granny_pngbt33332_vertex *vertices, std::vector<GLuint > &textures, VertexRGB *vertexRgb, VertexRGB *vertexRgb2, GlShaderProgram *shaderProgram);
void zGrannyRenderMesh(ZGrannyMesh *mesh, granny_pwngbt343332_vertex *vertices , std::vector<GLuint > &textures, VertexRGB *vertexRgb , VertexRGB *vertexRgb2, GlShaderProgram *shaderProgram, renderInfo_t *renderInfo);
void zGrannyRenderSkeleton( granny_skeleton *skeleton, granny_world_pose *worldPose );

ZGrannyScene *zGrannyCreateSceneFromMemory( const char *fileBytes, unsigned long fileSize, std::vector<GLuint > &textures );
ZGrannyScene *zGrannyCreateScene(const char *filename , std::vector<GLuint > &textures);
void zGrannyShutdownScene( ZGrannyScene *scene );

void zGrannyRenderScene(ZGrannyScene *scene , std::vector<GLuint > &textures, VertexRGB *vertexRgb, VertexRGB *vertexRgb2, GlShaderProgram *shaderProgram, GLfloat worldPos[3], renderInfo_t *renderInfo );

bool zGrannyGetObbCenter(std::string boneName, ZGrannyMesh *mesh, GLfloat obbCenter[3]);

bool zGrannyGetObbCenter2(std::string boneName, ZGrannyModel *attachTo, ZGrannyModel *toAttach, GLfloat obbCenter[3]);

#endif
