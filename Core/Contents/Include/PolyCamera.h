/*
 *  PolyCamera.h
 *  Polycode
 *
 *  Created by Ivan Safrin on 3/26/08.
 *  Copyright 2008 Ivan Safrin. All rights reserved.
 *
 */

// @package Scene

#pragma once
#include "PolyLogger.h"
#include "PolyString.h"
#include "PolyGlobals.h"
#include "PolyCoreServices.h"
#include "PolySceneEntity.h"
#include "PolySceneRenderTexture.h"

namespace Polycode {

	class _PolyExport Camera : public SceneEntity {
		public:
			
			Camera(Scene *parentScene);
			~Camera();
			
			void buildFrustrumPlanes();
			bool isSphereInFrustrum(Vector3 pos, float fRadius);
		
			bool canSee(SceneEntity *entity);
			
			void setOrthoMode(bool mode);
			bool getOrthoMode();
			void setFOV(float fov);
			float getFOV();
			void doCameraTransform();

			void setLightDepthTexture(Texture *texture);
			
			void setParentScene(Scene *parentScene);
			bool hasFilterShader();
			void drawFilter();
			
			
			void setExposureLevel(float level);
			float getExposureLevel();
			
			void createPostFilter(Material *shaderMaterial);
			void setPostFilter(String shaderName);
			
		private:
		
			float exposureLevel;
			bool orthoMode;
			float fov;
			float frustumPlanes[6][4];
			Scene *parentScene;
		
			bool fovSet;

			Material *filterShaderMaterial;			
			Texture *originalSceneTexture;			
			Texture *zBufferSceneTexture;						
			vector<ShaderBinding*> localShaderOptions;
			bool _hasFilterShader;
	};	
}