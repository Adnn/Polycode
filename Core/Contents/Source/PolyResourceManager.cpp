/*
 *  PolyResourceManager.cpp
 *  Poly
 *
 *  Created by Ivan Safrin on 8/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "PolyResourceManager.h"

using namespace Polycode;

ResourceManager::ResourceManager() {
	PHYSFS_init(NULL);
}

ResourceManager::~ResourceManager() {
		printf("Shutting down resource manager...\n");
		PHYSFS_deinit();
		for(int i=0; i < resources.size(); i++)	{
			delete resources[i];
		}
		resources.clear();
}

void ResourceManager::parseShaders(String dirPath, bool recursive) {
	vector<OSFileEntry> resourceDir;
	resourceDir = OSBasics::parseFolder(dirPath, false);
	
	for(int i=0; i < resourceDir.size(); i++) {	
		if(resourceDir[i].type == OSFileEntry::TYPE_FILE) {
			if(resourceDir[i].extension == "mat") {
				Logger::log("Adding shaders from %s\n", resourceDir[i].nameWithoutExtension.c_str());
				TiXmlDocument doc(resourceDir[i].fullPath.c_str());
				doc.LoadFile();
				if(doc.Error()) {
					Logger::log("XML Error: %s\n", doc.ErrorDesc());
				} else {
					TiXmlElement *mElem = doc.RootElement()->FirstChildElement("shaders");
					
					if(mElem) {
						TiXmlNode* pChild;					
						for (pChild = mElem->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {						
							Shader *newShader = CoreServices::getInstance()->getMaterialManager()->createShaderFromXMLNode(pChild);
							if(newShader != NULL) {
								Logger::log("Adding shader %s\n", newShader->getName().c_str());
								newShader->setResourceName(newShader->getName());
								resources.push_back(newShader);
							}
						}
					}
				}
			}
		} else {
			if(recursive)
				parseShaders(dirPath+"/"+resourceDir[i].name, true);
		}
	}
}

void ResourceManager::addShaderModule(PolycodeShaderModule *module) {
	shaderModules.push_back(module);
}

void ResourceManager::parsePrograms(String dirPath, bool recursive) {
	vector<OSFileEntry> resourceDir;
	resourceDir = OSBasics::parseFolder(dirPath, false);
	for(int i=0; i < resourceDir.size(); i++) {	
		if(resourceDir[i].type == OSFileEntry::TYPE_FILE) {
			for(int m=0; m < shaderModules.size(); m++) {
				PolycodeShaderModule *shaderModule = shaderModules[m];
				if(shaderModule->acceptsExtension(resourceDir[i].extension)) {
					Resource *newProgram = shaderModule->createProgramFromFile(resourceDir[i].extension, resourceDir[i].fullPath);
					if(newProgram) {
						newProgram->setResourceName(resourceDir[i].name);
						newProgram->setResourcePath(resourceDir[i].fullPath);				
						resources.push_back(newProgram);					
					}
				}
			}
		} else {
			if(recursive)
				parsePrograms(dirPath+"/"+resourceDir[i].name, true);
		}
	}	
}

void ResourceManager::parseMaterials(String dirPath, bool recursive) {
	vector<OSFileEntry> resourceDir;
	resourceDir = OSBasics::parseFolder(dirPath, false);
	
	for(int i=0; i < resourceDir.size(); i++) {	
		if(resourceDir[i].type == OSFileEntry::TYPE_FILE) {
			if(resourceDir[i].extension == "mat") {
				Logger::log("Adding materials from %s\n", resourceDir[i].nameWithoutExtension.c_str());
				TiXmlDocument doc(resourceDir[i].fullPath.c_str());
				doc.LoadFile();
				if(doc.Error()) {
					Logger::log("XML Error: %s\n", doc.ErrorDesc());
				} else {
					TiXmlElement *mElem = doc.RootElement()->FirstChildElement("materials");
					if(mElem) {
						TiXmlNode* pChild;					
						for (pChild = mElem->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
							Material *newMat = CoreServices::getInstance()->getMaterialManager()->materialFromXMLNode(pChild);
							newMat->setResourceName(newMat->getName());
							resources.push_back(newMat);
						}
					}
				}
			}
		} else {
			if(recursive)
				parseMaterials(dirPath+"/"+resourceDir[i].name, true);
		}
	}
}

void ResourceManager::parseCubemaps(String dirPath, bool recursive) {
	vector<OSFileEntry> resourceDir;
	resourceDir = OSBasics::parseFolder(dirPath, false);
	
	for(int i=0; i < resourceDir.size(); i++) {	
		if(resourceDir[i].type == OSFileEntry::TYPE_FILE) {
			if(resourceDir[i].extension == "mat") {
				Logger::log("Adding cubemaps from %s\n", resourceDir[i].nameWithoutExtension.c_str());
				TiXmlDocument doc(resourceDir[i].fullPath.c_str());
				doc.LoadFile();
				if(doc.Error()) {
					Logger::log("XML Error: %s\n", doc.ErrorDesc());
				} else {
					TiXmlElement *mElem = doc.RootElement()->FirstChildElement("cubemaps");
					
					if(mElem) {
						TiXmlNode* pChild;					
						for (pChild = mElem->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
							Cubemap *newMat = CoreServices::getInstance()->getMaterialManager()->cubemapFromXMLNode(pChild);
							//						newMat->setResourceName(newMat->getName());
							if(newMat)
								resources.push_back(newMat);
						}
					}
				}
			}
		} else {
			if(recursive)
				parseCubemaps(dirPath+"/"+resourceDir[i].name, true);
		}
	}	
}

void ResourceManager::addResource(Resource *resource) {
	resources.push_back(resource);
}

void ResourceManager::parseTextures(String dirPath, bool recursive) {
	vector<OSFileEntry> resourceDir;
	resourceDir = OSBasics::parseFolder(dirPath, false);
	for(int i=0; i < resourceDir.size(); i++) {	
		if(resourceDir[i].type == OSFileEntry::TYPE_FILE) {
			if(resourceDir[i].extension == "png") {
				Logger::log("Adding texture %s\n", resourceDir[i].nameWithoutExtension.c_str());
				Texture *t = CoreServices::getInstance()->getMaterialManager()->createTextureFromFile(resourceDir[i].fullPath);
				t->setResourceName(resourceDir[i].name);
				resources.push_back(t);
			}
		} else {
			if(recursive)
				parseTextures(dirPath+"/"+resourceDir[i].name, true);
		}
	}
}


void ResourceManager::addArchive(String zipPath) {
	if(PHYSFS_addToSearchPath(zipPath.c_str(), 1, getThreadID()) == 0) {
		Logger::log("Error adding archive to resource manager...\n");
	} else {
		Logger::log("Added archive: %s\n", zipPath.c_str());
	}
}

void ResourceManager::addDirResource(String dirPath, bool recursive) {
	parseTextures(dirPath, recursive);
	parsePrograms(dirPath, recursive);
	parseShaders(dirPath, recursive);
	parseCubemaps(dirPath, recursive);	
	parseMaterials(dirPath, recursive);
}

Resource *ResourceManager::getResource(int resourceType, String resourceName) {
	Logger::log("requested %s\n", resourceName.c_str());
	for(int i =0; i < resources.size(); i++) {
//		Logger::log("is it %s?\n", resources[i]->getResourceName().c_str());		
		if(resources[i]->getResourceName() == resourceName && resources[i]->getResourceType() == resourceType) {
			return resources[i];
		}
	}
	Logger::log("return NULL\n");
	// need to add some sort of default resource for each type
	return NULL;
}