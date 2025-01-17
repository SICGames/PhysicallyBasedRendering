#include "Game.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include<chrono>
#include<iostream>

#define max(a,b) (((a) > (b)) ? (a):(b))
#define min(a,b) (((a) < (b)) ? (a):(b))

Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;
	baseVertexShader = 0;
	basePixelShader = 0;
	camera = 0;
	

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

void Game::Release() {
	
	SAFE_DELETE(camera);
	SAFE_DELETE(basePixelShader);
	SAFE_DELETE(baseVertexShader);
	SAFE_DELETE(skyVertexShader);
	SAFE_DELETE(skyPixelShader);
	SAFE_DELETE(PBRVertexShader);
	SAFE_DELETE(PBRPixelShader);
	SAFE_DELETE(PBRMatPixelShader);
	SAFE_DELETE(ConvolutionPixelShader);
	SAFE_DELETE(PrefilterMapPixelShader);
	SAFE_DELETE(IntegrateBRDFPixelShader);
	SAFE_DELETE(QuadVertexShader);

	SAFE_DELETE(sphereMesh);
	SAFE_DELETE(cubeMesh);
	SAFE_DELETE(quadMesh);

	SAFE_DELETE(materialAluminiumInsulator);
	SAFE_DELETE(materialGold);
	SAFE_DELETE(materialGunMetal);
	SAFE_DELETE(materialLeather);
	SAFE_DELETE(materialSuperHeroFabric);
	SAFE_DELETE(materialCamoFabric);
	SAFE_DELETE(materialGlassVisor);
	SAFE_DELETE(materialIronOld);
	SAFE_DELETE(materialRubber);
	SAFE_DELETE(materialWood);

	SAFE_DELETE(materialSkyBox);
	SAFE_DELETE(quadEntity);
	SAFE_DELETE(skyBoxEntity);
	/*for(auto& se: sphereEntities) delete se;
	for (auto& fe : flatEntities) delete fe;*/
	SAFE_DELETE(pbrSphere);
	SAFE_DELETE(pbrSphere1);
	SAFE_DELETE(pbrSphere2);
	SAFE_DELETE(pbrSphere3);
	SAFE_DELETE(pbrSphere4);
	SAFE_DELETE(pbrSphere5);
	SAFE_DELETE(pbrSphere6);
	SAFE_DELETE(pbrSphere7);
	SAFE_DELETE(pbrSphere8);
	SAFE_DELETE(pbrSphere9);

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			delete pbrSpheres[i][j];
		}

	SAFE_RELEASE(skyIBLSRV);
	SAFE_RELEASE(envMapSRV);
	SAFE_RELEASE(envMaptex);
	SAFE_RELEASE(skyIBLtex);
	SAFE_RELEASE(brdfLUTSRV);
	SAFE_RELEASE(brdfLUTtex);
	SAFE_RELEASE(rasterizer);
	SAFE_RELEASE(skyDepthState);
	SAFE_RELEASE(skyRasterizerState);
	SAFE_RELEASE(sampler);

	SAFE_RELEASE(AluminiumInsulator_Albedo);
	SAFE_RELEASE(AluminiumInsulator_Normal);
	SAFE_RELEASE(AluminiumInsulator_Metallic);
	SAFE_RELEASE(AluminiumInsulator_Rough);

	SAFE_RELEASE(Gold_Albedo);
	SAFE_RELEASE(Gold_Normal);
	SAFE_RELEASE(Gold_Metallic);
	SAFE_RELEASE(Gold_Rough);

	SAFE_RELEASE(GunMetal_Albedo);
	SAFE_RELEASE(GunMetal_Normal);
	SAFE_RELEASE(GunMetal_Metallic);
	SAFE_RELEASE(GunMetal_Rough);

	SAFE_RELEASE(Leather_Albedo);
	SAFE_RELEASE(Leather_Normal);
	SAFE_RELEASE(Leather_Metallic);
	SAFE_RELEASE(Leather_Rough);

	SAFE_RELEASE(SuperHeroFabric_Albedo);
	SAFE_RELEASE(SuperHeroFabric_Normal);
	SAFE_RELEASE(SuperHeroFabric_Metallic);
	SAFE_RELEASE(SuperHeroFabric_Rough);

	SAFE_RELEASE(CamoFabric_Albedo);
	SAFE_RELEASE(CamoFabric_Normal);
	SAFE_RELEASE(CamoFabric_Metallic);
	SAFE_RELEASE(CamoFabric_Rough);

	SAFE_RELEASE(GlassVisor_Albedo);
	SAFE_RELEASE(GlassVisor_Normal);
	SAFE_RELEASE(GlassVisor_Metallic);
	SAFE_RELEASE(GlassVisor_Rough);

	SAFE_RELEASE(IronOld_Albedo);
	SAFE_RELEASE(IronOld_Normal);
	SAFE_RELEASE(IronOld_Metallic);
	SAFE_RELEASE(IronOld_Rough);

	SAFE_RELEASE(Rubber_Albedo);
	SAFE_RELEASE(Rubber_Normal);
	SAFE_RELEASE(Rubber_Metallic);
	SAFE_RELEASE(Rubber_Rough);

	SAFE_RELEASE(Wood_Albedo);
	SAFE_RELEASE(Wood_Normal);
	SAFE_RELEASE(Wood_Metallic);
	SAFE_RELEASE(Wood_Rough);

	SAFE_RELEASE(skySRV);
	SAFE_RELEASE(skyIR);

	//#ifdef _DEBUG
	//	ID3D11Debug* DebugDevice = nullptr;
	//	HRESULT Result = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice));
	//
	//	Result = DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//
	//	DebugDevice->Release();
	//#endif
}
Game::~Game()
{
	Release();
}


void Game::Init()
{
	//Initialize helper methods
	
	CameraInitialize();
	ShadersInitialize();
	bool result = ModelsInitialize();
	if (result == false) 
	{
		MessageBox(0, "Failed to load models. Make sure Models folder is in the program's directory. Program now exiting.", "Models Load Failure.", 0);
		//-- should terminate.
		exit(0);
	}
	LoadTextures();
	SkyBoxInitialize();
	MaterialsInitialize();
	GameEntityInitialize();
	

	//Setup rasterizer state 
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = false;

	device->CreateRasterizerState(&rasterizerDesc, &rasterizer);

	//Setup blend state 
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//device->CreateBlendState(&blendDescDR, &blendDR);

	//TODO : DEPTH STENCIL STATE
	//TODO : Set the OMdepthstate also change depth target in Set Render target

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//device->CreateDepthStencilState(&depthStencilDescDR, &depthStateDR);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	IBLStuff();
}



void Game::CameraInitialize()
{
	camera = new Camera(0.0f, 1.0f, -14.0f);
	camera->UpdateProjectionMatrix((float)width / height);
}

void Game::ShadersInitialize()
{
	baseVertexShader = new SimpleVertexShader(device, context);
	if (!baseVertexShader->LoadShaderFile(L"Debug/BaseVertexShader.cso"))
		baseVertexShader->LoadShaderFile(L"BaseVertexShader.cso");

	basePixelShader = new SimplePixelShader(device, context);
	if (!basePixelShader->LoadShaderFile(L"Debug/BasePixelShader.cso"))
		basePixelShader->LoadShaderFile(L"BasePixelShader.cso");

	skyVertexShader = new SimpleVertexShader(device, context);
	if (!skyVertexShader->LoadShaderFile(L"Debug/SkyBoxVertexShader.cso"))
		skyVertexShader->LoadShaderFile(L"SkyBoxVertexShader.cso");

	skyPixelShader = new SimplePixelShader(device, context);
	if (!skyPixelShader->LoadShaderFile(L"Debug/SkyBoxPixelShader.cso"))
		skyPixelShader->LoadShaderFile(L"SkyBoxPixelShader.cso");

	PBRVertexShader = new SimpleVertexShader(device, context);
	if (!PBRVertexShader->LoadShaderFile(L"Debug/PBRVertexShader.cso"))
		PBRVertexShader->LoadShaderFile(L"PBRVertexShader.cso");

	PBRPixelShader = new SimplePixelShader(device, context);
	if (!PBRPixelShader->LoadShaderFile(L"Debug/PBRPixelShader.cso"))
		PBRPixelShader->LoadShaderFile(L"PBRPixelShader.cso");

	PBRMatPixelShader = new SimplePixelShader(device, context);
	if (!PBRMatPixelShader->LoadShaderFile(L"Debug/PBRMatPixelShader.cso"))
		PBRMatPixelShader->LoadShaderFile(L"PBRMatPixelShader.cso");

	ConvolutionPixelShader = new SimplePixelShader(device, context);
	if (!ConvolutionPixelShader->LoadShaderFile(L"Debug/ConvolutionPixelShader.cso"))
		ConvolutionPixelShader->LoadShaderFile(L"ConvolutionPixelShader.cso");

	PrefilterMapPixelShader = new SimplePixelShader(device, context);
	if (!PrefilterMapPixelShader->LoadShaderFile(L"Debug/PrefilterMapPixelShader.cso"))
		PrefilterMapPixelShader->LoadShaderFile(L"PrefilterMapPixelShader.cso");

	IntegrateBRDFPixelShader = new SimplePixelShader(device, context);
	if (!IntegrateBRDFPixelShader->LoadShaderFile(L"Debug/IntegrateBRDFPixelShader.cso"))
		IntegrateBRDFPixelShader->LoadShaderFile(L"IntegrateBRDFPixelShader.cso");

	QuadVertexShader = new SimpleVertexShader(device, context);
	if (!QuadVertexShader->LoadShaderFile(L"Debug/QuadVertexShader.cso"))
		QuadVertexShader->LoadShaderFile(L"QuadVertexShader.cso");
}

bool Game::ModelsInitialize()
{
	sphereMesh = new Mesh("Models/sphere.obj", device);
	if (sphereMesh->GetIndexCount() < 0) 
	{
		return false;
	}
	
	cubeMesh = new Mesh("Models/cube.obj", device);
	
	if (cubeMesh->GetIndexCount() < 0) return false;

	quadMesh = new Mesh("Models/quad.obj", device);
	if (quadMesh->GetIndexCount() < 0) return false;

	return true;
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Albedo.png", 0, &AluminiumInsulator_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Normal.png", 0, &AluminiumInsulator_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Metallic.png", 0, &AluminiumInsulator_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Roughness.png", 0, &AluminiumInsulator_Rough);
	
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Albedo.png", 0, &Gold_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Normal.png", 0, &Gold_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Metallic.png", 0, &Gold_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Roughness.png", 0, &Gold_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Albedo.png", 0, &GunMetal_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Normal.png", 0, &GunMetal_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Metallic.png", 0, &GunMetal_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Roughness.png", 0, &GunMetal_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/Leather_Albedo.png", 0, &Leather_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Normal.png", 0, &Leather_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Metallic.png", 0, &Leather_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Roughness.png", 0, &Leather_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Albedo.png", 0, &SuperHeroFabric_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Normal.png", 0, &SuperHeroFabric_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Metallic.png", 0, &SuperHeroFabric_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Roughness.png", 0, &SuperHeroFabric_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/CamoFabric_Albedo.png", 0, &CamoFabric_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/CamoFabric_Normal.png", 0, &CamoFabric_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/CamoFabric_Metallic.png", 0, &CamoFabric_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/CamoFabric_Roughness.png", 0, &CamoFabric_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/GlassVisor_Albedo.png", 0, &GlassVisor_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/GlassVisor_Normal.png", 0, &GlassVisor_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/GlassVisor_Metallic.png", 0, &GlassVisor_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/GlassVisor_Roughness.png", 0, &GlassVisor_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/IronOld_Albedo.png", 0, &IronOld_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/IronOld_Normal.png", 0, &IronOld_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/IronOld_Metallic.png", 0, &IronOld_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/IronOld_Roughness.png", 0, &IronOld_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/Rubber_Albedo.png", 0, &Rubber_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Rubber_Normal.png", 0, &Rubber_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Rubber_Metallic.png", 0, &Rubber_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Rubber_Roughness.png", 0, &Rubber_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/Wood_Albedo.png", 0, &Wood_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Wood_Normal.png", 0, &Wood_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Wood_Metallic.png", 0, &Wood_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Wood_Roughness.png", 0, &Wood_Rough);
}

void Game::SkyBoxInitialize()
{
	CreateDDSTextureFromFile(device, L"Textures/skybox1.dds", 0, &skySRV);
	CreateDDSTextureFromFile(device, L"Textures/skybox1IR.dds", 0, &skyIR);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rasterizerDesc, &skyRasterizerState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDesc, &skyDepthState);


	//----
	



}

void Game::MaterialsInitialize()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	device->CreateSamplerState(&samplerDesc, &sampler);

	materialAluminiumInsulator = new Material(AluminiumInsulator_Albedo, AluminiumInsulator_Normal, AluminiumInsulator_Metallic, AluminiumInsulator_Rough, sampler);
	materialGold = new Material(Gold_Albedo, Gold_Normal, Gold_Metallic, Gold_Rough, sampler);
	materialGunMetal = new Material(GunMetal_Albedo, GunMetal_Normal, GunMetal_Metallic, GunMetal_Rough, sampler);
	materialLeather = new Material(Leather_Albedo, Leather_Normal, Leather_Metallic, Leather_Rough, sampler);
	materialSuperHeroFabric = new Material(SuperHeroFabric_Albedo, SuperHeroFabric_Normal, SuperHeroFabric_Metallic, SuperHeroFabric_Rough, sampler);
	materialCamoFabric = new Material(CamoFabric_Albedo, CamoFabric_Normal, CamoFabric_Metallic, CamoFabric_Rough, sampler);
	materialGlassVisor = new Material(GlassVisor_Albedo, GlassVisor_Normal, GlassVisor_Metallic, GlassVisor_Rough, sampler);
	materialIronOld = new Material(IronOld_Albedo, IronOld_Normal, IronOld_Metallic, IronOld_Rough, sampler);
	materialRubber = new Material(Rubber_Albedo, Rubber_Normal, Rubber_Metallic, Rubber_Rough, sampler);
	materialWood = new Material(Wood_Albedo, Wood_Normal, Wood_Metallic, Wood_Rough, sampler);

	materialSkyBox = new Material(skySRV, NULL, NULL, NULL, sampler);
	
}

void Game::GameEntityInitialize()
{
	skyBoxEntity = new GameEntity(cubeMesh, materialSkyBox);
	quadEntity = new GameEntity(cubeMesh);

	pbrSphere = new GameEntity(sphereMesh, materialAluminiumInsulator);
	pbrSphere->SetPosition(2.0f, 3.0f, 0.0f);

	pbrSphere1 = new GameEntity(sphereMesh, materialGold);
	pbrSphere1->SetPosition(2.0f, 2.0f, 0.0f);

	pbrSphere2 = new GameEntity(sphereMesh, materialGunMetal);
	pbrSphere2->SetPosition(2.0f, 1.0f, 0.0f);

	pbrSphere3 = new GameEntity(sphereMesh, materialLeather);
	pbrSphere3->SetPosition(2.0f, 0.0f, 0.0f);

	pbrSphere4 = new GameEntity(sphereMesh, materialSuperHeroFabric);
	pbrSphere4->SetPosition(2.0f, -1.0f, 0.0f);

	pbrSphere5 = new GameEntity(sphereMesh, materialCamoFabric);
	pbrSphere5->SetPosition(3.0f, 3.0f, 0.0f);

	pbrSphere6 = new GameEntity(sphereMesh, materialGlassVisor);
	pbrSphere6->SetPosition(3.0f, 2.0f, 0.0f);

	pbrSphere7 = new GameEntity(sphereMesh, materialIronOld);
	pbrSphere7->SetPosition(3.0f, 1.0f, 0.0f);

	pbrSphere8 = new GameEntity(sphereMesh, materialRubber);
	pbrSphere8->SetPosition(3.0f, 0.0f, 0.0f);

	pbrSphere9 = new GameEntity(sphereMesh, materialWood);
	pbrSphere9->SetPosition(3.0f, -1.0f, 0.0f);

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			pbrSpheres[i][j] = new GameEntity(sphereMesh);
		}

	float x = -6.0f; float y = 6.0f;
	for (size_t i = 0; i < 6; i++)
	{
		if (y == 0.0f)
			y = 6.0f;
		for (size_t j = 0; j < 6; j++)
		{
			if (x == 0.0f)
				x = -6.0f;

			pbrSpheres[i][j]->SetPosition(x, y, 0.0f);
			x += 1.0f;
		}
		y -= 1.0f;
	}
}

void Game::IBLStuff()
{
	/*__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
	double perfCounterSeconds = 1.0 / (double)perfFreq;

	__int64 start;
	QueryPerformanceCounter((LARGE_INTEGER*)&start);*/
	/*ID3D11Query * start;
	ID3D11Query * stop;
	ID3D11Query * disjoint;

	D3D11_QUERY_DESC timestamp_query_desc;
	timestamp_query_desc.Query = D3D11_QUERY_TIMESTAMP;
	timestamp_query_desc.MiscFlags = 0;

	D3D11_QUERY_DESC disjoint_query_desc;
	disjoint_query_desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	disjoint_query_desc.MiscFlags = 0;

	device->CreateQuery(&timestamp_query_desc, &start);
	device->CreateQuery(&timestamp_query_desc, &stop);
	device->CreateQuery(&disjoint_query_desc, &disjoint);

	context->Begin(disjoint);
	context->End(start);*/
	auto start = std::chrono::high_resolution_clock::now();

	XMFLOAT3 position = XMFLOAT3(0, 0, 0);
	XMFLOAT4X4 camViewMatrix;
	XMFLOAT4X4 camProjMatrix;
	XMVECTOR tar[] = { XMVectorSet(1, 0, 0, 0), XMVectorSet(-1, 0, 0, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, -1, 0) };
	XMVECTOR up[] = { XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 1, 0, 0) };
	//---
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.0f };
	//---
#pragma region Diffuse IBL
	// DIFFUSE IBL CONVOLUTION

	D3D11_TEXTURE2D_DESC skyIBLDesc;
	//ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	skyIBLDesc.Width = 64;
	skyIBLDesc.Height = 64;
	skyIBLDesc.MipLevels = 1;
	skyIBLDesc.ArraySize = 6;
	skyIBLDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	skyIBLDesc.Usage = D3D11_USAGE_DEFAULT;
	skyIBLDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	skyIBLDesc.CPUAccessFlags = 0;
	skyIBLDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	skyIBLDesc.SampleDesc.Count = 1;
	skyIBLDesc.SampleDesc.Quality = 0;
	//---
	ID3D11RenderTargetView* skyIBLRTV[6];
	//--
	D3D11_RENDER_TARGET_VIEW_DESC skyIBLRTVDesc;
	ZeroMemory(&skyIBLRTVDesc, sizeof(skyIBLRTVDesc));
	skyIBLRTVDesc.Format = skyIBLDesc.Format;
	skyIBLRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	skyIBLRTVDesc.Texture2DArray.ArraySize = 1;
	skyIBLRTVDesc.Texture2DArray.MipSlice = 0;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC skyIBLSRVDesc;
	ZeroMemory(&skyIBLSRVDesc, sizeof(skyIBLSRVDesc));
	skyIBLSRVDesc.Format = skyIBLDesc.Format;
	skyIBLSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	skyIBLSRVDesc.TextureCube.MostDetailedMip = 0;
	skyIBLSRVDesc.TextureCube.MipLevels = 1;
	//---
	D3D11_VIEWPORT skyIBLviewport;
	skyIBLviewport.Width = 64;
	skyIBLviewport.Height = 64;
	skyIBLviewport.MinDepth = 0.0f;
	skyIBLviewport.MaxDepth = 1.0f;
	skyIBLviewport.TopLeftX = 0.0f;
	skyIBLviewport.TopLeftY = 0.0f;
	//---
	
	device->CreateTexture2D(&skyIBLDesc, 0, &skyIBLtex);
	device->CreateShaderResourceView(skyIBLtex, &skyIBLSRVDesc, &skyIBLSRV);
	//context->GenerateMips(skyIBLSRV);
	for (int i = 0; i < 6; i++) {
		skyIBLRTVDesc.Texture2DArray.FirstArraySlice = i;
		device->CreateRenderTargetView(skyIBLtex, &skyIBLRTVDesc, &skyIBLRTV[i]);
		//-- Cam directions
		XMVECTOR dir = XMVector3Rotate(tar[i], XMQuaternionIdentity());
		XMMATRIX view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&position), dir, up[i]);
		XMStoreFloat4x4(&camViewMatrix, DirectX::XMMatrixTranspose(view));

		XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.0f, 0.1f, 100.0f);
		XMStoreFloat4x4(&camProjMatrix, DirectX::XMMatrixTranspose(P));

		context->OMSetRenderTargets(1, &skyIBLRTV[i], 0);
		context->RSSetViewports(1, &skyIBLviewport);
		context->ClearRenderTargetView(skyIBLRTV[i], color);
		//context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		vertexBuffer = cubeMesh->GetVertexBuffer();
		indexBuffer = cubeMesh->GetIndexBuffer();

		skyVertexShader->SetMatrix4x4("view", camViewMatrix);
		skyVertexShader->SetMatrix4x4("projection", camProjMatrix);

		skyVertexShader->CopyAllBufferData();
		skyVertexShader->SetShader();

		ConvolutionPixelShader->SetShaderResourceView("Sky", skySRV);
		ConvolutionPixelShader->SetSamplerState("basicSampler", sampler);

		ConvolutionPixelShader->CopyAllBufferData();
		ConvolutionPixelShader->SetShader();

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		context->RSSetState(skyRasterizerState);
		context->OMSetDepthStencilState(skyDepthState, 0);

		context->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);

		// Reset the render states we've changed
		//context->RSSetState(0);
		//context->OMSetDepthStencilState(0, 0);
		
	}
	
	for (int i = 0; i < 6; i++) {
		skyIBLRTV[i]->Release();
	}

#pragma endregion

#pragma region Prefilter EnvMap
	// PREFILTER ENVIRONMENT MAP
	unsigned int maxMipLevels = 5;
	D3D11_TEXTURE2D_DESC envMapDesc;
	//ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	envMapDesc.Width = 256;
	envMapDesc.Height = 256;
	envMapDesc.MipLevels = maxMipLevels;
	envMapDesc.ArraySize = 6;
	envMapDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	envMapDesc.Usage = D3D11_USAGE_DEFAULT;
	envMapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	envMapDesc.CPUAccessFlags = 0;
	envMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	envMapDesc.SampleDesc.Count = 1;
	envMapDesc.SampleDesc.Quality = 0;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC envMapSRVDesc;
	ZeroMemory(&envMapSRVDesc, sizeof(envMapSRVDesc));
	envMapSRVDesc.Format = skyIBLDesc.Format;
	envMapSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	envMapSRVDesc.TextureCube.MostDetailedMip = 0;
	envMapSRVDesc.TextureCube.MipLevels = maxMipLevels;
	//--
	ID3D11RenderTargetView* envMapRTV[6];
	//---
	device->CreateTexture2D(&envMapDesc, 0, &envMaptex);
	device->CreateShaderResourceView(envMaptex, &envMapSRVDesc, &envMapSRV);
	for (int mip = 0; mip < maxMipLevels; mip++) {
	
		D3D11_RENDER_TARGET_VIEW_DESC envMapRTVDesc;
		ZeroMemory(&envMapRTVDesc, sizeof(envMapRTVDesc));
		envMapRTVDesc.Format = skyIBLDesc.Format;
		envMapRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		envMapRTVDesc.Texture2DArray.ArraySize = 1;
		envMapRTVDesc.Texture2DArray.MipSlice = mip;

		unsigned mipWidth = 256 * pow(0.5, mip);
		unsigned mipHeight = 256 * pow(0.5, mip);

		D3D11_VIEWPORT envMapviewport;
		envMapviewport.Width = mipWidth;
		envMapviewport.Height = mipHeight;
		envMapviewport.MinDepth = 0.0f;
		envMapviewport.MaxDepth = 1.0f;
		envMapviewport.TopLeftX = 0.0f;
		envMapviewport.TopLeftY = 0.0f;
		

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		//float roughness = 0.0;
		for (int i = 0; i < 6; i++) {
			envMapRTVDesc.Texture2DArray.FirstArraySlice = i;
			device->CreateRenderTargetView(envMaptex, &envMapRTVDesc, &envMapRTV[i]);

			//-- Cam directions
			XMVECTOR dir = XMVector3Rotate(tar[i], XMQuaternionIdentity());
			XMMATRIX view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&position), dir, up[i]);
			XMStoreFloat4x4(&camViewMatrix, DirectX::XMMatrixTranspose(view));

			XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.0f, 0.1f, 100.0f);
			XMStoreFloat4x4(&camProjMatrix, DirectX::XMMatrixTranspose(P));
			//---
			context->OMSetRenderTargets(1, &envMapRTV[i], 0);
			context->RSSetViewports(1, &envMapviewport);
			context->ClearRenderTargetView(envMapRTV[i], color);
			//---

			vertexBuffer = cubeMesh->GetVertexBuffer();
			indexBuffer = cubeMesh->GetIndexBuffer();

			skyVertexShader->SetMatrix4x4("view", camViewMatrix);
			skyVertexShader->SetMatrix4x4("projection", camProjMatrix);

			skyVertexShader->CopyAllBufferData();
			skyVertexShader->SetShader();

			PrefilterMapPixelShader->SetShaderResourceView("EnvMap", skySRV);
			PrefilterMapPixelShader->SetSamplerState("basicSampler", sampler);
			PrefilterMapPixelShader->SetFloat("roughness", roughness);

			PrefilterMapPixelShader->CopyAllBufferData();
			PrefilterMapPixelShader->SetShader();

			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			context->RSSetState(skyRasterizerState);
			context->OMSetDepthStencilState(skyDepthState, 0);

			context->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);

			// Reset the render states we've changed
			//context->RSSetState(0);
			//context->OMSetDepthStencilState(0, 0);

		}
		for (int i = 0; i < 6; i++) {
			envMapRTV[i]->Release();
		}
	
	}
#pragma endregion

#pragma region Integrate BRDF LUT
	// INTEGRATE BRDF & CREATE LUT

	D3D11_TEXTURE2D_DESC brdfLUTDesc;
	//ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	brdfLUTDesc.Width = 512;
	brdfLUTDesc.Height = 512;
	brdfLUTDesc.MipLevels = 0;
	brdfLUTDesc.ArraySize = 1;
	brdfLUTDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	brdfLUTDesc.Usage = D3D11_USAGE_DEFAULT;
	brdfLUTDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	brdfLUTDesc.CPUAccessFlags = 0;
	brdfLUTDesc.MiscFlags = 0;
	brdfLUTDesc.SampleDesc.Count = 1;
	brdfLUTDesc.SampleDesc.Quality = 0;
	//---
	ID3D11RenderTargetView* brdfLUTRTV;
	//--
	D3D11_RENDER_TARGET_VIEW_DESC brdfLUTRTVDesc;
	ZeroMemory(&brdfLUTRTVDesc, sizeof(brdfLUTRTVDesc));
	brdfLUTRTVDesc.Format = brdfLUTDesc.Format;
	brdfLUTRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC brdfLUTSRVDesc;
	ZeroMemory(&brdfLUTSRVDesc, sizeof(brdfLUTSRVDesc));
	brdfLUTSRVDesc.Format = brdfLUTSRVDesc.Format;
	brdfLUTSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	brdfLUTSRVDesc.TextureCube.MostDetailedMip = 0;
	brdfLUTSRVDesc.TextureCube.MipLevels = 1;
	//---
	D3D11_VIEWPORT brdfLUTviewport;
	brdfLUTviewport.Width = 512;
	brdfLUTviewport.Height = 512;
	brdfLUTviewport.MinDepth = 0.0f;
	brdfLUTviewport.MaxDepth = 1.0f;
	brdfLUTviewport.TopLeftX = 0.0f;
	brdfLUTviewport.TopLeftY = 0.0f;
	//---
	device->CreateTexture2D(&brdfLUTDesc, 0, &brdfLUTtex);
	device->CreateRenderTargetView(brdfLUTtex, &brdfLUTRTVDesc, &brdfLUTRTV);
	device->CreateShaderResourceView(brdfLUTtex, &brdfLUTSRVDesc, &brdfLUTSRV);

	context->OMSetRenderTargets(1, &brdfLUTRTV, 0);
	context->RSSetViewports(1, &brdfLUTviewport);
	context->ClearRenderTargetView(brdfLUTRTV, color);

	vertexBuffer = quadEntity->GetMesh()->GetVertexBuffer();
	indexBuffer = quadEntity->GetMesh()->GetIndexBuffer();

	baseVertexShader->SetMatrix4x4("world", *quadEntity->GetWorldMatrix());
	baseVertexShader->SetMatrix4x4("view", camera->GetView());
	baseVertexShader->SetMatrix4x4("projection", camera->GetProjection());

	baseVertexShader->CopyAllBufferData();
	baseVertexShader->SetShader();

	IntegrateBRDFPixelShader->SetShader();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(quadEntity->GetMesh()->GetIndexCount(), 0, 0);

	/*QuadVertexShader->SetShader();
	IntegrateBRDFPixelShader->SetShader();


	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(3, 0);*/

	brdfLUTRTV->Release();

	//---JANKY CHEESE---
	//CreateWICTextureFromFile(device, context, L"Textures/ibl_brdf_lut.png", 0, &brdfLUTSRV);
	//---END CHEESE----
#pragma endregion

	auto stop = std::chrono::high_resolution_clock::now();

	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//printf("\n %f", (float)diff.count());
	std::cout << "\n" << diff.count();
	/*__int64 stop;
	QueryPerformanceCounter((LARGE_INTEGER*)&stop);

	float totalTime = (float)((stop - start) * perfCounterSeconds);
	printf("%f", totalTime);*/

	//context->End(stop);
	//context->End(disjoint);

	//while (context->GetData(disjoint, NULL, 0, 0) == S_FALSE)
	//{
	//	Sleep(1);       // Wait a bit, but give other threads a chance to run
	//}
	//D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	//context->GetData(disjoint, &tsDisjoint, sizeof(tsDisjoint), 0);
	//if (tsDisjoint.Disjoint)
	//{
	//	return;
	//}

	//UINT64 tsStart, tsStop;
	//context->GetData(start, &tsStart, sizeof(UINT64), 0);
	//context->GetData(stop, &tsStop, sizeof(UINT64), 0);

	//float timestamp = float(tsStart - tsStop) /
	//	float(tsDisjoint.Frequency) * 1000.0f;
	//
	//start->Release();
	//stop->Release();
	//disjoint->Release();
	//printf("\n %f", timestamp);
}

void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	
	// Update the projection matrix assuming the
	// camera exists
	if (camera)
		camera->UpdateProjectionMatrix((float)width / height);
}

void Game::Update(float deltaTime, float totalTime)
{
	camera->Update(deltaTime);

	
	pbrSphere->UpdateWorldMatrix();
	pbrSphere1->UpdateWorldMatrix();
	pbrSphere2->UpdateWorldMatrix();
	pbrSphere3->UpdateWorldMatrix();
	pbrSphere4->UpdateWorldMatrix();
	pbrSphere5->UpdateWorldMatrix();
	pbrSphere6->UpdateWorldMatrix();
	pbrSphere7->UpdateWorldMatrix();
	pbrSphere8->UpdateWorldMatrix();
	pbrSphere9->UpdateWorldMatrix();

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			pbrSpheres[i][j]->UpdateWorldMatrix();
		}
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

void Game::Draw(float deltaTime, float totalTime)
{

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.0f };


	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->RSSetViewports(1, &viewport);
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMFLOAT3 lightPos[4] = { XMFLOAT3(10.0f, 10.0f, -10.0f), XMFLOAT3(10.0f, -10.0f, -10.0f), XMFLOAT3(-10.0f, 10.0f, -10.0f), XMFLOAT3(-10.0f, -10.0f, -10.0f) };
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	float r = 0.0f;
	for (size_t i = 0; i < 6; i++)
	{
		float m = 0.0f;
		for (size_t j = 0; j < 6; j++)
		{

			render.PBRRenderProcess(pbrSpheres[i][j], vertexBuffer, indexBuffer, PBRVertexShader, PBRPixelShader, camera, context, m, r, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);

			m += 0.2f;
		}
		r += 0.2f;
	}
	
	render.PBRMatRenderProcess(pbrSphere, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere1, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere2, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere3, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere4, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere5, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere6, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere7, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere8, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);
	render.PBRMatRenderProcess(pbrSphere9, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context, skyIBLSRV, envMapSRV, brdfLUTSRV, sampler);

	

	render.RenderSkyBox(cubeMesh, vertexBuffer, indexBuffer, skyVertexShader, skyPixelShader, camera, context, skyRasterizerState, skyDepthState, skySRV);

	swapChain->Present(0, 0);
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Check left mouse button
	if (buttonState & 0x0001) {
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(yDiff, xDiff);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}

#pragma endregion
