#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrGLRendererBase.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "../cwin64/WOdriving.h"
#include "../cwin64/NetMsgOrca.h"

//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
#include <irrKlang.h>
#include <Windows.h>
#include <conio.h>

using namespace irrklang;
using namespace Aftr;

//WOWayPointAbstract* keyw;
//WOWayPointAbstract* cardw;
//WOWayPointAbstract* remotew;
//WOWayPointAbstract* carw;
//WOWayPointAbstract* usbw;
//WOWayPointAbstract* gasw;
//WOWayPointAbstract* dub;

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args )
{
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNewModule::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

	this->Engine = createIrrKlangDevice();

	//Playing 2D music for project 

	ISoundSource* music = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/imperial-march-ringtone.mp3");
	music->setDefaultVolume(1.0f);
	ISound* music_main = Engine->play2D(music, true);

	ISoundSource* noise = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/rain-01.mp3");
	noise->setDefaultVolume(0.9f);
	ISound* sound = Engine->play3D(noise, vec3df(50, 50, 50), true);

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
   this->orca = WOdriving::New();
   worldLst->push_back(this->orca->getorcaWO());

   if (ManagerEnvironmentConfiguration::getVariableValue("NetServerLisenPort") == "12683") {
	   this->net = NetMessengerClient::New("127.0.0.1", "12682");
   }
   else {
	   this->net = NetMessengerClient::New("127.0.0.1", "12683");
   }
}


GLViewNewModule::~GLViewNewModule()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
	//ISoundEngine* Engine = createIrrKlangDevice();
	//set listener position
	Engine->setListenerPosition(
		/*Position*/vec3df(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z),
		/*Look Direction*/vec3df(cam->getLookDirection().x, cam->getLookDirection().y, cam->getLookDirection().z),
		/*Velocity*/vec3df(0, 0, 0),
		/*Normal Direction\upVector*/ vec3df(cam->getNormalDirection().x, cam->getNormalDirection().y, cam->getNormalDirection().z)
	);

	/*if (escape_items == 6) {
		worldLst->eraseViaWOptr(end);
		escape_items = 0;
	}
	if (keyw->isTriggered() == 1) {
		worldLst->eraseViaWOptr(key);
		worldLst->eraseViaWOptr(keyw);
		escape_items++;
	}
	if (cardw->isTriggered() == 1) {
		worldLst->eraseViaWOptr(card);
		worldLst->eraseViaWOptr(cardw);
		escape_items++;
	}
	if (remotew->isTriggered() == 1) {
		worldLst->eraseViaWOptr(remote);
		worldLst->eraseViaWOptr(remotew);
		escape_items++;
	}
	if (carw->isTriggered() == 1) {
		worldLst->eraseViaWOptr(car);
		worldLst->eraseViaWOptr(carw);
		escape_items++;
	}
	if (usbw->isTriggered() == 1) {
		worldLst->eraseViaWOptr(usb);
		worldLst->eraseViaWOptr(usbw);
		escape_items++;
	}
	if (gasw->isTriggered() == 1) {
		worldLst->eraseViaWOptr(gas);
		worldLst->eraseViaWOptr(gasw);
		escape_items++;
	}*/

	Engine->setRolloffFactor(0.1f);
	if (this->isMoving()) {
		this->orca->update();
	}
	if (this->net->isTCPSocketOpen()) {
		if (this->isMoving()) {
			this->net->sendNetMsgSynchronousTCP(NetMsgOrca(this->orca->getPosition(), this->orca->getLookDirection()));
		}
	}
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{
	//if (!this->typing) {
		GLView::onKeyDown(key);
		if (key.keysym.sym == SDLK_0)
			this->setNumPhysicsStepsPerRender(1);

		if (this->isMoving()) { this->orca->onKeyDown(key); }

		SDL_Keycode pressed = key.keysym.sym;
		if (pressed == SDLK_w && this->cam != nullptr) {
			this->cam->moveInLookDirection(10);
		}
		if (pressed == SDLK_s && this->cam != nullptr) {
			this->cam->moveOppositeLookDirection(10);
		}
		if (pressed == SDLK_a && this->cam != nullptr) {
			this->cam->moveLeft();
			this->cam->moveLeft();
			this->cam->moveLeft();
			this->cam->moveLeft();
			this->cam->moveLeft();
			this->cam->moveLeft();
		}
		if (pressed == SDLK_d && this->cam != nullptr) {
			this->cam->moveRight();
			this->cam->moveRight();
			this->cam->moveRight();
			this->cam->moveRight();
			this->cam->moveRight();
			this->cam->moveRight();
		}
		//this->KeyPress(key.itemsym.sym);
	//}
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
   if (this->isMoving()) { this->orca->onKeyUp(key); }
}


void Aftr::GLViewNewModule::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition(-500,325,15);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 100, 100, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 50, 50, 50 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );

   ////Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
   //wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
   //wo->setPosition( Vector(0,0,0) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   ////Create the infinite grass plane that uses NVIDIAPhysX(the floor)
   //wo = WONVStaticPlane::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0,0) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0,50.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0.5f,75.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue("sharedmultimediapath") + "/models/sphereRp5.wrl", Vector(1.0f, 1.0f, 1.0f), mstSMOOTH );
   //wo->setPosition( 0,0,100.0f );
   //wo->setLabel( "Sphere" );
   //this->worldLst->push_back( wo );

   //wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
   //((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
   //((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
   //wo->setPosition( Vector(20,20,20) );
   //wo->setLabel( "Paladin" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //netLst->push_back( wo );
   //this->setActor( wo );
   
   //wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
   //wo->setPosition( Vector(20,10,20) );
   //wo->isVisible = false; //hide the WOHuman's bounding box
   //((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
   //wo->setLabel( "Human Cyborg" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo ); //Push the WOHuman as an actor
   //netLst->push_back( wo );
   //this->setActor( wo ); //Start module where human is the actor

   ////Create and insert the WOWheeledVehicle
   //std::vector< std::string > wheels;
   //std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
   //wo->setPosition( Vector( 5, -15, 20 ) );
   //wo->setLabel( "Car 1970s Beater" );
   //((WOODE*)wo)->mass = 200;
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //this->setActor( wo );
   //netLst->push_back( wo );

   //These are my escape items 
   /*WO* usb = WO::New("../mm/models/usb.3ds", Vector(.7, .7, .7));
   usb->setPosition(Vector(-900, 950, 25));
   usb->rotateAboutGlobalX(-1.575);
   worldLst->push_back(usb);

   WO* key = WO::New("../mm/models/key.3DS", Vector(.3, .3, .3));
   key->setPosition(Vector(20, 20, 15));
   key->rotateAboutGlobalX(-1.575);
   worldLst->push_back(key);

   WO* car = WO::New("../mm/models/car_key.dae", Vector(1, 1, 1));
   car->setPosition(Vector(-1750, 1080, 30));
   car->rotateAboutGlobalX(-1.575);
   car->rotateAboutGlobalZ(1.575);
   worldLst->push_back(car);

   WO* card = WO::New("../mm/models/key_card.dae", Vector(400, 400, 400));
   card->setPosition(Vector(275, -130, 20));
   card->rotateAboutGlobalX(-1.575);
   card->rotateAboutGlobalY(-.575);
   card->rotateAboutGlobalZ(-1.575);
   worldLst->push_back(card);

   WO* gas = WO::New("../mm/models/gas.dae", Vector(60, 60, 60));
   gas->setPosition(Vector(-925, 125, 25));
   gas->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(gas);

   WO* remote = WO::New("../mm/models/remote.dae", Vector(.5, .5, .5));
   remote->setPosition(Vector(-1880, 185, 30));
   remote->rotateAboutGlobalY(1.575);
   remote->rotateAboutGlobalZ(2.275);
   worldLst->push_back(remote);*/




   //Maze items/maze


   /*WO* helo = WO::New("../mm/models/orca.dae", Vector(20,20,20), MESH_SHADING_TYPE::mstFLAT);
   helo->setPosition(Vector(-1450, 580, 80));
   helo->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(helo);*/

   //createNewModuleWayPoints();*/

  /*WO* maze2 = WO::New("../mm/models/3d-model.dae", Vector(30, 30, 20), MESH_SHADING_TYPE::mstFLAT);
  maze2->setPosition(Vector(-1450, 580, 20));
  maze2->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
  worldLst->push_back(maze2);

   WO* maze = WO::New("../mm/models/Labyrinth.3ds", Vector(50, 50, 100), MESH_SHADING_TYPE::mstFLAT);
   maze->setPosition(Vector(0, 0, 15));
   maze->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(maze);

   WO* door = WO::New("../mm/models/Massive Door with Animation.dae", Vector(18, 18, 18), MESH_SHADING_TYPE::mstFLAT);
   door->setPosition(Vector(-500, 474, 15));
   door->rotateAboutRelZ(-1.575);
   door->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(door);

   WO* wall = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(90, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall->setPosition(Vector(-1405, -216, 15));
   wall->rotateAboutRelZ(2.975);
   wall->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall);
   
   WO* wall2 = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(80, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall2->setPosition(Vector(-1649, -180, 15));
   wall2->rotateAboutRelZ(2.975);
   wall2->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall2);
   
   WO* wall3 = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(180, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall3->setPosition(Vector(-2243, 699, 15));
   wall3->rotateAboutRelZ(1.4);
   wall3->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall3);
   
   WO* wall4 = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(50, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall4->setPosition(Vector(-1147, 1315, 15));
   wall4->rotateAboutRelZ(-.175);
   wall4->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall4);
   
   WO* wall5 = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(50, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall5->setPosition(Vector(-1420, 1358, 15));
   wall5->rotateAboutRelZ(-.175);
   wall5->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall5);

   WO* wall6 = WO::New("../mm/models/uploads_files_968984_Spiegelleuchte.dae", Vector(40, 40, 40), MESH_SHADING_TYPE::mstFLAT);
   wall6->setPosition(Vector(-1810, -168, 15));
   wall6->rotateAboutRelZ(2.975);
   wall6->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(wall6);

     WO* gate = WO::New("../mm/models/CircleSquareTruss_150cm_Dae.DAE", Vector(470, 470, 200), MESH_SHADING_TYPE::mstFLAT);
   gate->setPosition(Vector(-1410, 580, 0));
   gate->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(gate);
   
   WO* glass = WO::New("../mm/models/glass.dae", Vector(20, 27, 17));
   glass->setPosition(Vector(50, 23, 16));
   glass->rotateAboutGlobalZ(-1.575);
   glass->renderOrderType = RENDER_ORDER_TYPE::roBLEND_FUNC_GL_ONE;
   worldLst->push_back(glass);   */

   createNewModuleWayPoints();
}

bool GLViewNewModule::isMoving() {
	return this->orca != nullptr;
}

//this->cam->setPosition(-500, 325, 15);
void GLViewNewModule::createNewModuleWayPoints()
{
	//WayPointParametersBase params(this);
	//params.frequency = 5000;
	//params.useCamera = true;
	//params.visible = true;
	//WOWayPointSpherical* wayPt = WOWP1::New(params, 6);
	////x is red, y is green, z is blue
	//wayPt->setPosition(Vector(100, 100, 100));
	//worldLst->push_back(wayPt);
	//worldLst->eraseViaWOptr(wayPt);
	//
	//WayPointParametersBase win(this);
	//win.frequency = 5000;
	//win.useCamera = true;
	//win.visible = false;

	//dub = WOWP1::New(win, 30);
	//dub->setPosition(Vector(410, -250, 0));
	//worldLst->push_back(dub);

	//WayPointParametersBase items(this);
	//items.frequency = 5000;
	//items.useCamera = true;
	//items.visible = false;

	/*
	I don't want to do anymore and give way the rest of the spots :)
	*/
	/*keyw = WOWP1::New(items, 30);
	keyw->setPosition(Vector(21, 20, 0));
	worldLst->push_back(keyw);
	cardw = WOWP1::New(items, 30);
	cardw->setPosition(Vector(276, -130, 20));
	worldLst->push_back(cardw);
	remotew = WOWP1::New(items, 30);
	remotew->setPosition(Vector(-1879, 185, 30));
	worldLst->push_back(remotew);
	carw = WOWP1::New(items, 30);
	carw->setPosition(Vector(-1749, 1085, 0));
	worldLst->push_back(carw);
	usbw = WOWP1::New(items, 30);
	usbw->setPosition(Vector(-900, 950, 0));
	worldLst->push_back(usbw);
	gasw = WOWP1::New(items, 30);
	gasw->setPosition(Vector(-926, 125, 25));
	worldLst->push_back(gasw);*/

	
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 50, 50 ) );
   worldLst->push_back( wayPt );
}

/*void GLViewNewModule::KeyPress(const SDL_Keycode& key) {
	if (this->typing) {
		switch (key) {
		case SDLK_RETURN:
			this->typing = false;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			if (!this->shift) {
				this->shift = true;
			}
			break;
		case SDLK_TAB:
			net->sendNetMsgSynchronousTCP(NetMsgTxt(this->worldTxt->getText()));
			break;
		case SDLK_ESCAPE:
			this->typing = false;
			break;
		default:
			this->type(key);
		}
		return;
	}
	switch (key) {
	case SDLK_RETURN:
		this->typing = true;
		if (this->worldTxt->getText() == this->initTxt) {
			this->worldTxt->setText("");
		}
		return;
	case SDLK_b:
		this->Bananer();
		break;
	default: break;
	}
}*/
