// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include "stdafx.h"
#include "d3d9.h"
#include "MinHook.h"
#include <stdio.h>
#include <d3dx9.h>
typedef int (WINAPI *WINPROC)(HWND, UINT, WPARAM, LPARAM);
static char oldBGMdata[0x600];
static char customBGMdata[0x600];
void* bgmtbl;
static char newBGMPath[] = "/debug/bgm2/";
bool createDepth = false;
enum
{
	MAX_NUM_VP_WEIGHTS = 15,
	MAX_NUM_FP_WEIGHTS = 8
}
;

enum
{
	MAX_NUM_WEIGHTS = 12,
	NUM_WEIGHTS = 4
}
;
float m_weightBloomVp[3][MAX_NUM_VP_WEIGHTS];
float m_weightBloomFp[3][MAX_NUM_FP_WEIGHTS];
float m_weightStar[MAX_NUM_WEIGHTS];
float m_eyePos[4];
float m_bloomWeight[4];
float m_starWeight[4];
float m_scaleFactor;
float m_texHeight;
float m_texWidth;
float m_threshold;
float m_bloomCoeff[3];
float m_samplerOffset[3];
float m_starCoeff[3];
float m_ambColor[3];
LPD3DXEFFECT m_pEffect;
HHOOK hkb;
IDirect3DTexture9* HDRtex[9];
IDirect3DSurface9* HDRrt[9];
static void Matrix4x4MultiplyBy4x4(float src1[4][4], float src2[4][4], float dest[4][4]) {
	dest[0][0] = src1[0][0] * src2[0][0] + src1[0][1] * src2[1][0] + src1[0][2] * src2[2][0] + src1[0][3] * src2[3][0];
	dest[0][1] = src1[0][0] * src2[0][1] + src1[0][1] * src2[1][1] + src1[0][2] * src2[2][1] + src1[0][3] * src2[3][1];
	dest[0][2] = src1[0][0] * src2[0][2] + src1[0][1] * src2[1][2] + src1[0][2] * src2[2][2] + src1[0][3] * src2[3][2];
	dest[0][3] = src1[0][0] * src2[0][3] + src1[0][1] * src2[1][3] + src1[0][2] * src2[2][3] + src1[0][3] * src2[3][3];
	dest[1][0] = src1[1][0] * src2[0][0] + src1[1][1] * src2[1][0] + src1[1][2] * src2[2][0] + src1[1][3] * src2[3][0];
	dest[1][1] = src1[1][0] * src2[0][1] + src1[1][1] * src2[1][1] + src1[1][2] * src2[2][1] + src1[1][3] * src2[3][1];
	dest[1][2] = src1[1][0] * src2[0][2] + src1[1][1] * src2[1][2] + src1[1][2] * src2[2][2] + src1[1][3] * src2[3][2];
	dest[1][3] = src1[1][0] * src2[0][3] + src1[1][1] * src2[1][3] + src1[1][2] * src2[2][3] + src1[1][3] * src2[3][3];
	dest[2][0] = src1[2][0] * src2[0][0] + src1[2][1] * src2[1][0] + src1[2][2] * src2[2][0] + src1[2][3] * src2[3][0];
	dest[2][1] = src1[2][0] * src2[0][1] + src1[2][1] * src2[1][1] + src1[2][2] * src2[2][1] + src1[2][3] * src2[3][1];
	dest[2][2] = src1[2][0] * src2[0][2] + src1[2][1] * src2[1][2] + src1[2][2] * src2[2][2] + src1[2][3] * src2[3][2];
	dest[2][3] = src1[2][0] * src2[0][3] + src1[2][1] * src2[1][3] + src1[2][2] * src2[2][3] + src1[2][3] * src2[3][3];
	dest[3][0] = src1[3][0] * src2[0][0] + src1[3][1] * src2[1][0] + src1[3][2] * src2[2][0] + src1[3][3] * src2[3][0];
	dest[3][1] = src1[3][0] * src2[0][1] + src1[3][1] * src2[1][1] + src1[3][2] * src2[2][1] + src1[3][3] * src2[3][1];
	dest[3][2] = src1[3][0] * src2[0][2] + src1[3][1] * src2[1][2] + src1[3][2] * src2[2][2] + src1[3][3] * src2[3][2];
	dest[3][3] = src1[3][0] * src2[0][3] + src1[3][1] * src2[1][3] + src1[3][2] * src2[2][3] + src1[3][3] * src2[3][3];
}
;
int originalPtr[4096];
int original = NULL;
int original2 = NULL;
int original3 = NULL;
int original4 = NULL;
int original5 = NULL;
int original6 = NULL;
int original7 = NULL;
int original8 = NULL;
int original9 = NULL;
int original10 = NULL;
int original11 = NULL;
int original12 = NULL;
int original13 = NULL;
int original14 = NULL;
int original15 = NULL;
int rendererClassPtr = NULL;
int shaderCompileOriginal = NULL;
IDirect3DTexture9* shadowTex = NULL;
IDirect3DDevice9 *dev = NULL;
int shadowPass = true;

IDirect3DTexture9 **texptr;
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // from the D3DFVF_XYZRHW flag
	FLOAT tu, tv; //from the D3DFVF_DIFFUSE flag
}
;
struct PPVERT
{
	float x, y, z, rhw;
	float tu, tv;  // Texcoord for post-process source
	float tu2, tv2; // Texcoord for the original scene
	const static D3DVERTEXELEMENT9 Decl[4];
}
;
void calcWeightBloom(float *weightVp, float *weightFp, float radius)
{
	int numWeightsVp, numWeightsFp;
	if (radius <= 1.0f)
	{
		numWeightsVp = 5;
		numWeightsFp = 3;
		radius = 1.0f;
	}
	else if (radius <= 2.0f)
	{
		numWeightsVp = 9;
		numWeightsFp = 5;
		radius = 2.0f;
	}
	else
	{
		numWeightsVp = 15;
		numWeightsFp = 8;
		radius = 4.0f;
	}
	float total = 0.0f;
	for (int i = 0; i < numWeightsVp; i++)
	{
		weightVp[i] = expf(-0.5f * i * i / (radius * radius));
		if (i == 0)
		{
			total += 0.5f * weightVp[i];
		}
		else
		{
			total += 2.0f * weightVp[i];
		}
	}
	for (int i = 0; i < numWeightsVp; i++)
	{
		weightVp[i] /= total;
		//weightVp[i] = 1.0f;
	}
	for (int i = 0; i < numWeightsFp; i++)
	{
		if (i == 0)
		{
			weightFp[i] = weightVp[i];
		}
		else
		{
			weightFp[i] = 0.5f * (weightVp[2 * i - 1] + weightVp[2 * i]);
		}
		//weightFp[0] = 0.0f;
	}
}
void calcWeightStar(float *weight, float scale)
{
	float *ptr;
	float s = scale / 4.0f;
	ptr = weight;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < NUM_WEIGHTS; j++)
		{
			*ptr = powf(0.8f, powf(4.0f, i) * j * s);
			ptr++;
		}
	}
}
;
float bias[4][4] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
}
;
PPVERT Quad[4] =
{
	{
		-0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
	}
	,
	{
		1 - 0.5f, -0.5, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
	}
	,
	{
		-0.5,  1 - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
	}
	,
	{
		1 - 0.5f, 1 - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
	}
}
;

int *ptr2 = 0;

bool isShadowPass() {
	 int*  ptr = (int*)0x86A688;
	
	 char str[32];




	if (*ptr != 0) {
		bool * isShadow = (bool*)(*ptr) + 0xFA2C;
		sprintf(str, "%i\n", *isShadow);
		OutputDebugStringA(str);

		return *isShadow;
	}
	return true;
 

}


float shadowEnable[] = {
	1,1,1,1
}
;
float shadowDisable[] = {
	0,0,0,0
}
;
float *aux;
float shadowMatrix[4][4];
float biasShadowMatrix[4][4];
int setVertex = 0x406FEB;
int* whatever = (int*)0x8832DC;


void
DefWindowProc15() {

	int ptr1 = *whatever;
	int setVertex = 0x406FEB;
	float *shadow = shadowEnable;
	shadowPass = false;
	_asm {

		pushad
			mov ECX, ptr1
			push  1
			push shadow;
	     	push 110
			CALL setVertex


			popad
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp


		



			jmp original15;
	}
	// return (((*)())original)();
}


void
DefWindowProc14() {
	int ptr1 = *whatever;
	int setVertex = 0x406FEB;
	float *shadow = shadowDisable;
	shadowPass = true;
	_asm {

		pushad
			mov ECX, ptr1
			push  1
			push shadow;
		push 110
			CALL setVertex


			popad
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			mov ebx,0

			jmp original14;
	}
	// return (((*)())original)();
}









void
DefWindowProc1() {
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
	}
	if (createDepth) {
		createDepth = false;
		_asm {
			push eax
				mov  eax, dword ptr[ebx];
			mov  texptr, ebx;
			mov  shadowTex, ebx;
			pop eax;
		}
	}
	_asm {
		mov[esp + 14h], D3DFMT_A32B32G32R32F
			mov eax, dev
			jmp original2;
	}
	// return (((*)())original)();
}
bool ok = false;
void DefWindowProc2() {
	if (shadowTex != NULL) {
		_asm {
			push eax;
			mov  eax, [esi + 1F20h];
			mov  dev, eax;
			pop eax;
		}
		shadowTex = *texptr;
		IDirect3DSurface9 *depth;
		dev->SetTexture(4, shadowTex);
		dev->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		dev->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		dev->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		dev->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		dev->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			jmp original;
	}
	// return (((*)())original)();
}
void DefWindowProc4() {
	int ptr1 = *(int*)(0x86A688);
	_asm {
		pop edi
			pop esi
			mov eax, ptr1
			mov ebx, [eax + 0FA2Ch];
		pop ebx
			add esp, 0CCh
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			MOV ECX, DWORD PTR SS : [EBP - 8]
			jmp original4;
	}
	// return (((*)())original)();
}

D3DXMATRIX mymat;
void
DefWindowProc5() {
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0CCh
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			lea eax, [ebp - 18FCh]
			mov aux, eax;
	}
	int ptr1 = *whatever;
	float *shadow;
	if (shadowPass) {
	//	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		memcpy(shadowMatrix, aux, sizeof(float) * 16);
		memcpy(mymat, aux, sizeof(float) * 16);
	//	shadow = shadowDisable;
	}
	else {
	//	shadow = shadowEnable;
		//dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		_asm {
			lea eax, [ebp - 18FCh]
		}
	}
	static char str[64];

	sprintf(str, "Shadow:%f\n", shadow[0]);
	OutputDebugStringA(str);
	_asm {
		lea eax, mymat
			mov ECX, ptr1
			push 4
			push eax;
		push 100
			CALL setVertex
			mov ECX, ptr1
	//		push  1
		//	push shadow;
	//	push 110
		//	CALL setVertex
			jmp original5;
	}
	// return (((*)())original)();
}
void HDR() {
	IDirect3DSurface9* rt;
	LPD3DXSPRITE sprite = NULL;
	if (SUCCEEDED(D3DXCreateSprite(dev, &sprite))) {
		// created OK
	}
	dev->GetRenderTarget(0, &rt);
	IDirect3DTexture9* tex;
	dev->SetRenderTarget(0, HDRrt[0]);
	D3DRECT crec = {
		0, 0, 1280, 800
	}
	;
	// dev->SetRenderTarget(0, rt);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 4278190080, 1, 0);
	void *pContainer = NULL;
	IDirect3DTexture9 *pTexture = NULL;
	HRESULT hr = rt->GetContainer(IID_IDirect3DTexture9, &pContainer);
	//
	IDirect3DIndexBuffer9* pInd;
	dev->GetIndices(&pInd);
	IDirect3DVertexBuffer9* pVB;
	float left = 0.0f;
	float right = 2048;
	float top = 0.0f;
	float bottom = 1024;
	int indices[] = {
		0,1,2,3,4,5
	}
	;
	VOID* pIndices;
	(pInd->Lock(
		0, // Fill from start of the buffer
		sizeof(indices), // Size of the data to load
		&pIndices, // Returned index data
		0)); // Send default flags to the lock
	memcpy(pIndices, indices, sizeof(indices));
	pInd->Unlock();
	dev->SetIndices(pInd);
	//256 by 256 rectangle matching 256 by 256 texture
	CUSTOMVERTEX vertices[] =
	{
		{
			left, bottom, 0.5f, 1.0f, 0.0f, 1.0f
		}
		, // x, y, z, rhw, u, v
		{
			left, top, 0.5f, 1.0f, 0.0f, 0.0f
		}
		,
		{
			right, top, 0.5f, 1.0f, 1.0f, 0.0f
		}
		,
		{
			right, bottom, 0.5f, 1.0f, 1.0f, 1.0f
		}
		,
		{
			left, bottom, 0.5f, 1.0f, 0.0f, 1.0f
		}
		,
		{
			right, top, 0.5f, 1.0f, 1.0f, 0.0f
		}
		,
	}
	;
	for (int i = 0; i < 6; i++)
	{
		vertices[i].x -= 0.5f;
		vertices[i].y -= 0.5f;
	}
	VOID* pVoid; // a void pointer
				 //
				 // create a vertex buffer interface called v_buffer
	dev->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX),
		0,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		D3DPOOL_DEFAULT,
		&pVB,
		NULL);
	// lock v_buffer and load the vertices into it
	pVB->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	pVB->Unlock();
	if (SUCCEEDED(hr) && pContainer)
	{
		pTexture = (IDirect3DTexture9 *)pContainer;
		D3DXVECTOR3 pos;
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		unsigned int cpass, vpass;
		IDirect3DVertexDeclaration9* vertex;
		dev->GetVertexDeclaration(&vertex);
		IDirect3DVertexDeclaration9* vertex2;
		D3DVERTEXELEMENT9 ParticleElements[] =
		{
			{
				0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0
			}
			,
			{
				0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0
			}
			,
			D3DDECL_END()
		}
		;
		dev->CreateVertexDeclaration(ParticleElements, &vertex2);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		IDirect3DVertexDeclaration9 *old;
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		D3DXMatrixOrthoOffCenterLH(&mat, 0.5f, float(2048) + 0.5f, float(1024) + 0.5f, 0.5f, 0.0f, 1.0f);
		float lumThreshold;
		float starOffset[] = {
			1,1,1
		}
		;
		float downSample[] = {
			0,0
		}
		;
		dev->SetVertexDeclaration(vertex2);
		m_pEffect->Begin(&cpass, 0);
		m_pEffect->SetTechnique("HDR");
		m_pEffect->SetTexture("luminanceRT_Tex", HDRtex[0]);
		m_pEffect->SetTexture("Bloom1Tex_Tex", HDRtex[1]);
		m_pEffect->SetTexture("Bloom2Tex_Tex", HDRtex[2]);
		m_pEffect->SetTexture("Bloom4Tex_Tex", HDRtex[3]);
		m_pEffect->SetTexture("Star0Texture_Tex", HDRtex[4]);
		m_pEffect->SetTexture("Star1Texture_Tex", HDRtex[5]);
		m_pEffect->SetTexture("Star2Texture_Tex", HDRtex[6]);
		m_pEffect->SetTexture("Star3Texture_Tex", HDRtex[7]);
		m_pEffect->SetTexture("baseMap", pTexture);
		m_pEffect->SetTexture("__HDR_Merge_Pixel_Shader_baseMap", pTexture);
		m_pEffect->SetMatrix("matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Bloom1_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Bloom2_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Bloom4_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Merge_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star2_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star3_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star4_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star4_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("__HDR_Star4_Vertex_Shader_matViewProjection", &mat);
		m_pEffect->SetMatrix("matViewProjection", &mat);
		m_pEffect->SetFloat("gThreshold", m_threshold);
		m_pEffect->SetFloatArray("gWeight", &m_weightBloomVp[0][0], 5);
		m_pEffect->SetFloatArray("__HDR_Star_Vertex_Shader_gOffset", &starOffset[0], 3);
		m_pEffect->SetFloatArray("__HDR_Star2_Vertex_Shader_gOffset", &starOffset[0], 3);
		m_pEffect->SetFloatArray("__HDR_Star3_Vertex_Shader_gOffset", &starOffset[0], 3);
		m_pEffect->SetFloatArray("__HDR_Star3_Vertex_Shader_gOffset", &starOffset[0], 3);
		m_pEffect->SetFloatArray("gOffset", &downSample[0], 2);
		m_pEffect->SetFloatArray("__HDR_Bloom2_Vertex_Shader_gOffset", &downSample[0], 2);
		m_pEffect->SetFloatArray("__HDR_Bloom4_Vertex_Shader_gOffset", &downSample[0], 2);
		m_pEffect->SetFloatArray("__HDR_Bloom2_Vertex_Shader_gOffset", &downSample[0], 2);
		m_pEffect->SetFloatArray("__HDR_Star_Pixel_Shader_gWeight", &m_weightStar[0], 4);
		m_pEffect->SetFloatArray("__HDR_Star2_Pixel_Shader_gWeight", &m_weightStar[4], 4);
		m_pEffect->SetFloatArray("__HDR_Star3_Pixel_Shader_gWeight", &m_weightStar[8], 4);
		m_pEffect->SetFloatArray("__HDR_Star4_Pixel_Shader_gWeight", &m_weightStar[8], 4);
		m_pEffect->SetFloatArray("gWeightBloom", &m_bloomWeight[0], 4);
		m_pEffect->SetFloatArray("gWeightStar", &m_starWeight[0], 4);
		m_pEffect->SetFloatArray("__HDR_Bloom1_Pixel_Shader_gWeight", m_weightBloomVp[0], 5);
		m_pEffect->SetFloatArray("__HDR_Bloom1_Pixel_Shader_gWeight", m_weightBloomFp[0], 3);
		m_pEffect->SetFloatArray("__HDR_Bloom2_Vertex_Shader_gWeight", m_weightBloomVp[1], 9);
		m_pEffect->SetFloatArray("__HDR_Bloom2_Pixel_Shader_gWeight", m_weightBloomFp[1], 5);
		m_pEffect->SetFloatArray("__HDR_Bloom4_Vertex_Shader_gWeight", m_weightBloomVp[2], 15);
		m_pEffect->SetFloatArray("__HDR_Bloom4_Pixel_Shader_gWeight", m_weightBloomFp[2], 8);
		for (int i = 0;i < cpass;i++) {
			m_pEffect->SetTexture("baseMap_Tex", pTexture);
			m_pEffect->BeginPass(i);
			dev->SetRenderTarget(0, HDRrt[i]);
			dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 4278190080, 1, 0);
			dev->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));
			HRESULT hr = dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			m_pEffect->EndPass();
		}
		m_pEffect->End();
		dev->SetVertexDeclaration(vertex);
	}
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	dev->SetRenderTarget(0, rt);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 4278190080, 1, 0);
	D3DXVECTOR3 pos;
	pos.x = 0;
	pos.y = 0.0f;
	pos.z = 0.0f;
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(HDRtex[8], NULL, NULL, &pos, 0xFFFFFFFF);
	sprite->End();
	pVB->Release();
}
void
DefWindowProc10() {
	int doneRendering;
	_asm {
		pushad
			mov doneRendering, edx;
	}
	if (doneRendering == 1) {
		OutputDebugStringA("Done");
	}
	_asm {
		popad
			pop edi
			pop esi
			pop ebx
			add esp, 44h;
		cmp ebp, esp
			mov esp, ebp
			pop ebp
	}
	_asm {
		jmp original10;
	}
	// return (((*)())original)();
}
void
DefWindowProc6() {
	createDepth = true;
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
	}
	_asm {
		jmp original6;
	}
	// return (((*)())original)();
}
void compilePixelOriginal() {
	LPDIRECT3DPIXELSHADER9*  vertexShaderPointer = (LPDIRECT3DPIXELSHADER9*)rendererClassPtr;
	char buffer[1024];
	_asm {
		push eax;
		add vertexShaderPointer, 3388h;
		pop eax;
	}
	LPD3DXBUFFER shader = 0;
	LPD3DXBUFFER error = 0;
	for (int i = 0;i < 8;i++) {
		sprintf_s(buffer, 1024, "shaders/originals/ps%i.psh", i);
		HRESULT hr;
		bool ok = false;
		while (!ok) {
			hr = D3DXAssembleShaderFromFileA(buffer, 0, 0, 0, &shader, &error);
			if (FAILED(hr)) {
				OutputDebugString(L"Error compiling");
				char * myError = (char*)error->GetBufferPointer();
				OutputDebugStringA(myError);
				ok = false;
				MessageBoxA(NULL, myError, buffer, NULL);
			}
			else ok = true;
		}
		LPDIRECT3DPIXELSHADER9  oldVS = *vertexShaderPointer;
		if (oldVS != nullptr)
		{
			oldVS->Release();
		}
		hr = dev->CreatePixelShader((DWORD*)shader->GetBufferPointer(), vertexShaderPointer);
		if (shader != nullptr)
			shader->Release();
		if (error != nullptr)
			error->Release();
		if (FAILED(hr)) {
			OutputDebugString(L"Error compiling");
		}
		_asm
		{
			add vertexShaderPointer, 4h;
		}
		if (i == 14) {
			_asm
			{
				add vertexShaderPointer, 4h;
			}
		}
		if (i == 62) {
			_asm
			{
				add vertexShaderPointer, 28h;
			}
		}
	}
}
void compilePixel() {
	LPDIRECT3DPIXELSHADER9*  vertexShaderPointer = (LPDIRECT3DPIXELSHADER9*)rendererClassPtr;
	char buffer[1024];
	_asm {
		push eax;
		add vertexShaderPointer, 3388h;
		pop eax;
	}
	LPD3DXBUFFER shader = 0;
	LPD3DXBUFFER error = 0;
	for (int i = 0;i < 8;i++) {
		sprintf_s(buffer, 1024, "shaders/ps%i.psh", i);
		HRESULT hr;
		bool ok = false;
		while (!ok) {
			if (i != 1) {
				hr = D3DXAssembleShaderFromFileA(buffer, 0, 0, 0, &shader, &error);
			}
			else {
				sprintf_s(buffer, 1024, "shaders/hlsl/ps%i.psh", i);
				hr = D3DXCompileShaderFromFileA(buffer, 0, 0, "ps_main", "ps_3_0", D3DXSHADER_DEBUG, &shader, &error, 0);
			}
			if (FAILED(hr)) {
				OutputDebugString(L"Error compiling");
				char * myError = (char*)error->GetBufferPointer();
				OutputDebugStringA(myError);
				ok = false;
				MessageBoxA(NULL, myError, buffer, NULL);
			}
			else ok = true;
		}
		LPDIRECT3DPIXELSHADER9  oldVS = *vertexShaderPointer;
		if (oldVS != nullptr)
		{
			oldVS->Release();
		}
		hr = dev->CreatePixelShader((DWORD*)shader->GetBufferPointer(), vertexShaderPointer);
		if (shader != nullptr)
			shader->Release();
		if (error != nullptr)
			error->Release();
		if (FAILED(hr)) {
			OutputDebugString(L"Error compiling");
		}
		_asm
		{
			add vertexShaderPointer, 4h;
		}
		if (i == 14) {
			_asm
			{
				add vertexShaderPointer, 4h;
			}
		}
		if (i == 62) {
			_asm
			{
				add vertexShaderPointer, 28h;
			}
		}
	}
}
void compileVertexOriginal() {
	LPDIRECT3DVERTEXSHADER9*  vertexShaderPointer = (LPDIRECT3DVERTEXSHADER9*)rendererClassPtr;
	char buffer[1024];
	_asm {
		push eax;
		add vertexShaderPointer, 1F48h;
		pop eax;
	}
	LPD3DXBUFFER shader = 0;
	LPD3DXBUFFER error = 0;
	for (int i = 0;i < 66;i++) {
		sprintf_s(buffer, 1024, "shaders/originals/vs%i.vsh", i);
		bool ok = false;
		HRESULT hr;
		while (!ok) {
			hr = D3DXAssembleShaderFromFileA(buffer, 0, 0, 0, &shader, &error);
			if (FAILED(hr)) {
				OutputDebugString(L"Error compiling");
				char * myError = (char*)error->GetBufferPointer();
				OutputDebugStringA(myError);
				MessageBoxA(NULL, myError, buffer, NULL);
			}
			else  ok = true;
		}
		LPDIRECT3DVERTEXSHADER9  oldVS = *vertexShaderPointer;
		if (oldVS != nullptr)
			oldVS->Release();
		hr = dev->CreateVertexShader((DWORD*)shader->GetBufferPointer(), vertexShaderPointer);
		if (shader != nullptr)
			shader->Release();
		if (error != nullptr)
			error->Release();
		if (FAILED(hr)) {
			OutputDebugString(L"Error compiling");
		}
		_asm
		{
			add vertexShaderPointer, 4h;
		}
		if (i == 14) {
			_asm
			{
				add vertexShaderPointer, 4h;
			}
		}
		if (i == 62) {
			_asm
			{
				add vertexShaderPointer, 28h;
			}
		}
	}
}
void compileVertex() {
	LPDIRECT3DVERTEXSHADER9*  vertexShaderPointer = (LPDIRECT3DVERTEXSHADER9*)rendererClassPtr;
	char buffer[1024];
	_asm {
		push eax;
		add vertexShaderPointer, 1F48h;
		pop eax;
	}
	LPD3DXBUFFER shader = 0;
	LPD3DXBUFFER error = 0;
	for (int i = 0;i < 66;i++) {
		sprintf_s(buffer, 1024, "shaders/vs%i.vsh", i);
		bool ok = false;
		HRESULT hr;
		while (!ok) {
			if (i != 25 && i != 50) {
				hr = D3DXAssembleShaderFromFileA(buffer, 0, 0, 0, &shader, &error);
			}
			else {
				sprintf_s(buffer, 1024, "shaders/hlsl/vs%i.vsh", i);
				hr = D3DXCompileShaderFromFileA(buffer, 0, 0, "vs_main", "vs_3_0", D3DXSHADER_DEBUG, &shader, &error, 0);
			}
			if (FAILED(hr)) {
				OutputDebugString(L"Error compiling");
				char * myError = (char*)error->GetBufferPointer();
				OutputDebugStringA(myError);
				MessageBoxA(NULL, myError, buffer, NULL);
			}
			else  ok = true;
		}
		LPDIRECT3DVERTEXSHADER9  oldVS = *vertexShaderPointer;
		if (oldVS != nullptr)
			oldVS->Release();
		hr = dev->CreateVertexShader((DWORD*)shader->GetBufferPointer(), vertexShaderPointer);
		if (shader != nullptr)
			shader->Release();
		if (error != nullptr)
			error->Release();
		if (FAILED(hr)) {
			OutputDebugString(L"Error compiling");
		}
		_asm
		{
			add vertexShaderPointer, 4h;
		}
		if (i == 14) {
			_asm
			{
				add vertexShaderPointer, 4h;
			}
		}
		if (i == 62) {
			_asm
			{
				add vertexShaderPointer, 28h;
			}
		}
	}
}


void
DefWindowProc13() {
	float* timing1;
	float* timing2;
	float* increment;
	char *objName;
	_asm {
		pop edi
			pop esi
			pop ebx
			push eax
			lea eax, dword ptr[esi + 0BCh];
		mov increment, eax;
		lea eax, dword ptr[esi + 14h];
		mov objName, eax;
		pop eax;
		mov timing1, esi;
		add timing1, 15ch
			mov timing1,  esi;
		add timing1, 15ch
			mov timing2, esi;
		add timing2, 75ch;
	}
	*timing1 -= *increment / 2;
	*timing2 -= *increment / 2;
	if (strcmp(objName, "ChildS")) {
		*increment = 0.5;
	}
	_asm
	{
		add esp, 95Ch
		cmp ebp, esp
			mov esp, ebp
			pop ebp
			push eax;
		pop eax;
		jmp original13;
	}
}





void
DefWindowProc12() {
	void* bgmtbl;
	_asm {
		pop edi
			pop esi
			pop ebx
	}
	_asm
	{
		add esp, 95Ch
		cmp ebp, esp
			mov esp, ebp
			pop ebp
			push eax;
		lea eax, newBGMPath;
		mov[esp + 4], eax;
		pop eax;
		jmp original12;
	}
}
void
DefWindowProc11() {
	_asm {
		pop edi
			pop esi
			pop ebx
			mov bgmtbl, esi;
	}
	FILE *f = fopen("./bgmtbl.tbb", "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	memcpy(oldBGMdata, bgmtbl, 0x600);
	fread(bgmtbl, fsize, 1, f);
	memcpy(customBGMdata, bgmtbl, 0x600);
	fclose(f);
	_asm
	{
		add esp, 95Ch
		cmp ebp, esp
			mov esp, ebp
			pop ebp
			jmp original11;
	}
	// return (((*)())original)();
}
void
DefWindowProc8() {
	compileVertex();
	compilePixel();
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 95Ch
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			jmp original8;
	}
	// return (((*)())original)();
}
void
DefWindowProc9() {
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			push eax
			push ebx;
		mov eax, [ebp - 718h];
		mov rendererClassPtr, eax;
		add eax, 1F20h;
		mov ebx, [eax]
			mov dev, ebx;
		pop eax;
		pop ebx;
	}
	/*
	dev->CreateTexture(2048, 800,1, D3DUSAGE_RENDERTARGET, D3DFORMAT::D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT,&HDRtex[0], NULL);
	HDRtex[0]->GetSurfaceLevel(0, &HDRrt[0]);
	for (int i = 1;i < 8;i++) {
	dev->CreateTexture(1024, 400, 1, D3DUSAGE_RENDERTARGET, D3DFORMAT::D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &HDRtex[i], NULL);
	HDRtex[i]->GetSurfaceLevel(0, &HDRrt[i]);
	}
	dev->CreateTexture(2048, 800, 1, D3DUSAGE_RENDERTARGET, D3DFORMAT::D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &HDRtex[8], NULL);
	HDRtex[8]->GetSurfaceLevel(0, &HDRrt[8]);
	LPD3DXBUFFER
	pError
	= NULL;
	/*HRESULT hr=D3DXCreateEffectFromFileA(dev, "shaders/hlsl/HDR.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pError);
	if (pError) {
	OutputDebugStringA((char*)pError->GetBufferPointer());
	}
	*/
	_asm {
		jmp original9;
	}
	// return (((*)())original)();
}
void DefWindowProc7() {
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
	}
	_asm {
		jmp original7;
	}
	// return (((*)())original)();
}
void
__cdecl
DefWindowProc3() {
	float a = 0;
	float* b;
	_asm {
		lea ecx, [ebx + 148h];
		mov b, ecx;
	}
	*(b) += a;
	_asm {
		pop edi
			pop esi
			pop ebx
			add esp, 0C0h
			cmp ebp, esp
			mov esp, ebp
			pop ebp
			mov ecx, ebx
			jmp original3;
	}
	// return (((*)())original)();
}
__declspec(dllexport) LRESULT KeyboardProc(int nCode, WPARAM wParam,
	LPARAM lParam)
{
	char ch;
	if (((DWORD)lParam & 0x40000000) && (HC_ACTION == nCode))
	{
		if ((wParam == VK_F5))
		{
			compileVertex();
			compilePixel();
		}
		else if ((wParam == VK_F6)) {
			compileVertexOriginal();
			compilePixelOriginal();
		}
		else if ((wParam == VK_F7)) {
			strcpy(newBGMPath, "debug/bgm/");
			memcpy(bgmtbl, oldBGMdata, 0x600);
		}
		else if ((wParam == VK_F8)) {
			strcpy(newBGMPath, "debug/bgm2/");
			memcpy(bgmtbl, customBGMdata, 0x600);
		}
		LRESULT RetVal = CallNextHookEx(hkb, nCode, wParam, lParam);
		return RetVal;
	}
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
	)
{
	int r[256];
	int rt10 = 0x005FD382;
	int rt1 = (0x05EEA31);
	int rt = (0x005f5b1c);
	int rt2 = 0x0044E3D9;
	int rt3 = 0x045906E;
	int rt5 = 0x605C30;
	int rt6 = 0x5E9537;
	int rt7 = 0x04AEC9B;
	int rt8 = 0x05FC6CB;
	int rt9 = 0x05F9081;
	int rt11 = 0x0530660;
	r[12] = 0x052F89D;
	r[13] = 0x486BFF;
	r[14] = 0x04E379D;
	r[15] = 0x04E93D8;
	m_bloomWeight[0] = 0.8f;
	m_bloomWeight[1] = 0.3f;
	m_bloomWeight[2] = 0.2f;
	m_bloomWeight[3] = 1.0f;
	m_starWeight[0] = 0.305f;
	m_starWeight[1] = 1.0f;
	m_starWeight[2] = 0.9f;
	m_starWeight[3] = 0.4;
	m_threshold = 2.5f;
	m_scaleFactor = 2.0f;
	calcWeightBloom(m_weightBloomVp[0], m_weightBloomFp[0], 1.0f);
	calcWeightBloom(m_weightBloomVp[1], m_weightBloomFp[1], 2.0f);
	calcWeightBloom(m_weightBloomVp[2], m_weightBloomFp[2], 4.0f);
	calcWeightStar(m_weightStar, m_scaleFactor);
	m_texHeight = 1024;
	m_texWidth = 2048;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MH_Initialize();
		MH_STATUS status2;
		hkb = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, NULL, GetCurrentThreadId());


		if (status2 = MH_CreateHook((LPVOID)* &r[15], &DefWindowProc15, reinterpret_cast<LPVOID*>(&original15)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}


		if (status2 = MH_CreateHook((LPVOID)* &r[14], &DefWindowProc14, reinterpret_cast<LPVOID*>(&original14)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}


		if (status2 = MH_CreateHook((LPVOID)* &r[13], &DefWindowProc13, reinterpret_cast<LPVOID*>(&original13)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}

		if (status2 = MH_CreateHook((LPVOID)* &r[12], &DefWindowProc12, reinterpret_cast<LPVOID*>(&original12)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt11, &DefWindowProc11, reinterpret_cast<LPVOID*>(&original11)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt9, &DefWindowProc9, reinterpret_cast<LPVOID*>(&original9)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt10, &DefWindowProc10, reinterpret_cast<LPVOID*>(&original10)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt8, &DefWindowProc8, reinterpret_cast<LPVOID*>(&original8)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt7, &DefWindowProc6, reinterpret_cast<LPVOID*>(&original6)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt1, &DefWindowProc1, reinterpret_cast<LPVOID*>(&original2)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt5, &DefWindowProc5, reinterpret_cast<LPVOID*>(&original5)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt3, &DefWindowProc4, reinterpret_cast<LPVOID*>(&original4)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt2, &DefWindowProc3, reinterpret_cast<LPVOID*>(&original3)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (status2 = MH_CreateHook((LPVOID)* &rt, &DefWindowProc2, reinterpret_cast<LPVOID*>(&original)))
		{
			MessageBoxA(0, MH_StatusToString(status2), "fail", 0);
			return 1;
		}
		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			return 1;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}