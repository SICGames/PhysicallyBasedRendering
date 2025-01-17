#include "Render.h"



Render::Render()
{
}


Render::~Render()
{
}


void Render::RenderProcess(GameEntity* &gameEntity, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer, SimpleVertexShader* &vertexShader, SimplePixelShader* &pixelShader, Camera* &camera, ID3D11DeviceContext* &context)
{
	SetLights();

	vertexBuffer = gameEntity->GetMesh()->GetVertexBuffer();
	indexBuffer = gameEntity->GetMesh()->GetIndexBuffer();

	vertexShader->SetMatrix4x4("world", *gameEntity->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetData("dirLight_1", &dirLight_1, sizeof(DirectionalLight));
	pixelShader->SetData("ambientLight", &ambientLight, sizeof(AmbientLight));
	pixelShader->SetData("pointLight1", &pointLight1, sizeof(PointLight));
	pixelShader->SetData("pointLight2", &pointLight2, sizeof(PointLight));
	pixelShader->SetData("pointLight3", &pointLight3, sizeof(PointLight));
	pixelShader->SetData("pointLight4", &pointLight4, sizeof(PointLight));
	pixelShader->SetData("spotLight", &spotLight, sizeof(SpotLight));
	pixelShader->SetFloat3("cameraPosition", camera->GetPosition());

	pixelShader->SetShaderResourceView("textureSRV", gameEntity->GetMaterial()->GetAlbedoSRV());
	pixelShader->SetShaderResourceView("normalMapSRV", gameEntity->GetMaterial()->GetNormalSRV());
	pixelShader->SetSamplerState("basicSampler", gameEntity->GetMaterial()->GetMaterialSampler());

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(gameEntity->GetMesh()->GetIndexCount(), 0, 0);
}

void Render::PBRRenderProcess(GameEntity *& gameEntity, ID3D11Buffer *& vertexBuffer, ID3D11Buffer *& indexBuffer, SimpleVertexShader *& vertexShader, SimplePixelShader *& pixelShader, Camera *& camera, ID3D11DeviceContext *& context, float m, float r, ID3D11ShaderResourceView*& skyIR, ID3D11ShaderResourceView*& skyPrefilter, ID3D11ShaderResourceView*& brdfLUT, ID3D11SamplerState*& sampler)
{
	vertexBuffer = gameEntity->GetMesh()->GetVertexBuffer();
	indexBuffer = gameEntity->GetMesh()->GetIndexBuffer();

	vertexShader->SetMatrix4x4("world", *gameEntity->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("skyIR", skyIR);
	pixelShader->SetShaderResourceView("skyPrefilter", skyPrefilter);
	pixelShader->SetShaderResourceView("brdfLUT", brdfLUT);

	pixelShader->SetSamplerState("basicSampler", sampler);

	pixelShader->SetFloat3("albedo", XMFLOAT3(1.0f, 1.0f, 1.0f));
	pixelShader->SetFloat("metallic", m);
	pixelShader->SetFloat("roughness", r);
	pixelShader->SetFloat("ao", 1.0f);

	pixelShader->SetFloat3("lightPos1", XMFLOAT3(10.0f, 10.0f, -5.0f));
	pixelShader->SetFloat3("lightPos2", XMFLOAT3(10.0f, -10.0f, -2.0f));
	pixelShader->SetFloat3("lightPos3", XMFLOAT3(-10.0f, -10.0f, 7.0f));
	pixelShader->SetFloat3("lightPos4", XMFLOAT3(-10.0f, 10.0f, -10.0f));
	//PBRPixelShader->SetData("lightPos", &lightPos, sizeof(lightPos));
	pixelShader->SetFloat3("lightCol", XMFLOAT3(300.0f, 300.0f, 300.0f));

	pixelShader->SetFloat3("camPos", camera->GetPosition());

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(gameEntity->GetMesh()->GetIndexCount(), 0, 0);
}

void Render::PBRMatRenderProcess(GameEntity *& gameEntity, ID3D11Buffer *& vertexBuffer, ID3D11Buffer *& indexBuffer, SimpleVertexShader *& vertexShader, SimplePixelShader *& pixelShader, Camera *& camera, ID3D11DeviceContext *& context, ID3D11ShaderResourceView *& skyIR, ID3D11ShaderResourceView*& skyPrefilter, ID3D11ShaderResourceView*& brdfLUT, ID3D11SamplerState *& sampler)
{
	vertexBuffer = gameEntity->GetMesh()->GetVertexBuffer();
	indexBuffer = gameEntity->GetMesh()->GetIndexBuffer();

	vertexShader->SetMatrix4x4("world", *gameEntity->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	
	pixelShader->SetShaderResourceView("skyIR", skyIR);
	pixelShader->SetShaderResourceView("skyPrefilter", skyPrefilter);
	pixelShader->SetShaderResourceView("brdfLUT", brdfLUT);

	//pixelShader->SetSamplerState("basicSampler", sampler);

	pixelShader->SetShaderResourceView("albedoSRV", gameEntity->GetMaterial()->GetAlbedoSRV());
	pixelShader->SetShaderResourceView("normalSRV", gameEntity->GetMaterial()->GetNormalSRV());
	pixelShader->SetShaderResourceView("metallicSRV", gameEntity->GetMaterial()->GetMetallicSRV());
	pixelShader->SetShaderResourceView("roughSRV", gameEntity->GetMaterial()->GetRoughSRV());

	pixelShader->SetSamplerState("basicSampler", sampler);

	pixelShader->SetFloat("ao", 1.0f);

	pixelShader->SetFloat3("lightPos1", XMFLOAT3(10.0f, 10.0f, -10.0f));
	pixelShader->SetFloat3("lightPos2", XMFLOAT3(10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightPos3", XMFLOAT3(-10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightPos4", XMFLOAT3(-10.0f, 10.0f, -10.0f));
	//PBRPixelShader->SetData("lightPos", &lightPos, sizeof(lightPos));
	pixelShader->SetFloat3("lightCol", XMFLOAT3(300.0f, 300.0f, 300.0f));

	pixelShader->SetFloat3("camPos", camera->GetPosition());

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(gameEntity->GetMesh()->GetIndexCount(), 0, 0);
}

void Render::RenderSkyBox(Mesh* &mesh, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer, SimpleVertexShader* &vertexShader, SimplePixelShader* &pixelShader, Camera* &camera, ID3D11DeviceContext* &context, ID3D11RasterizerState* &rasterizerState, ID3D11DepthStencilState* &depthState, ID3D11ShaderResourceView* &SRV)
{
	context->RSSetState(rasterizerState);
	context->OMSetDepthStencilState(depthState, 0);

	vertexBuffer = mesh->GetVertexBuffer();
	indexBuffer = mesh->GetIndexBuffer();
	
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Sky", SRV);

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	

	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

	// Reset the render states we've changed
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void Render::SetLights()
{
	dirLight_1.SetLightValues(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(10.0f, 0.0f, 0.0f), 0.0f);
	ambientLight.SetLightValues(XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f));
	pointLight1.SetLightValues(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(2.0f, 0.0f, 0.0f), 3.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 0.0f);
	pointLight2.SetLightValues(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(-2.0f, 0.0f, 0.0f), 3.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 0.0f);
	pointLight3.SetLightValues(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 2.0f), 3.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 0.0f);
	pointLight4.SetLightValues(XMFLOAT4(0.6f, 0.0f, 0.4f, 1.0f), XMFLOAT3(0.0f, 0.0f, -2.0f), 3.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 0.0f);
	spotLight.SetLightValues(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 10.0f, XMFLOAT3(0.0f, -1.0f, 0.0f), 5.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), 0.0f);
	
}
