// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#include <d3dx9.h>
#include <DirectXMath.h>
#include <map>
#include <string>
#include <vector>
#include <wrl.h>
#include "FVF.h"
#include "../../C++/Kernel/Templates.h"

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;

extern ComPtr<IDirect3DDevice9> pD3DDevice;

template<typename V>
struct MeshBuilder
{
	using VertexType = V;

	MeshBuilder() : m_pMesh(NULL)
	{
	}

	// Triangles
	DWORD FacesCount() const
	{
		return (DWORD)m_Indices.size() / 3;
	}

	void AddTriangle2(const VertexType& a, const VertexType& b, const VertexType& c)
	{
		V aCopy = a, bCopy = b, cCopy = c;

		AddVertex(aCopy);
		AddVertex(bCopy);
		AddVertex(cCopy);
	}

	void AddQuad2(const VertexType& a, const VertexType& b, const VertexType& c, const VertexType& d)
	{
		AddTriangle2(a, b, c);
		AddTriangle2(c, d, a);
	}

	void GetTriangle(int pTriangle, const VertexType& a, const VertexType& b, const VertexType& c) const
	{
		int Index = pTriangle * 3;
		a = m_Vertices[m_Indices[Index++]];
		b = m_Vertices[m_Indices[Index++]];
		c = m_Vertices[m_Indices[Index]];
	}

	void RemoveTriangle(int pTriangle)
	{
		int Index = pTriangle * 3 + 2;
		m_Indices.RemoveAt(Index--);
		m_Indices.RemoveAt(Index--);
		m_Indices.RemoveAt(Index);
	}

	virtual void OnParseTriangle(const VertexType& a, const VertexType& b, const VertexType& c)
	{
		AddTriangle2(a, b, c);
	}

	// a helper
	void ValidateAndLockSourceMesh(
		LPD3DXMESH pSourceMesh,
		V** pVertices,
		DWORD** pIndices,
		DWORD& dwNumFaces,
		DWORD& dwNumVertices,
		DWORD& dwMeshEnum
	) const
	{
		// Lock the geometry buffers
		dwNumFaces = pSourceMesh->GetNumFaces();
		dwNumVertices = pSourceMesh->GetNumVertices();
		dwMeshEnum = pSourceMesh->GetOptions();

		// Allocate a temporary edge list
		if (FAILED(pSourceMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)pVertices)))
			throw std::exception("LockVertexBuffer");
		if (FAILED(pSourceMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)pIndices)))
			throw std::exception("LockIndexBuffer");
	}

	void ParseMesh(LPD3DXMESH pSourceMesh)
	{
		V* pVertices = nullptr;
		DWORD* pIndices = nullptr;
		WORD* pIndices2 = nullptr;

		// Lock the geometry buffers
		DWORD dwNumFaces = 0;
		DWORD dwNumVertices = 0;
		DWORD dwMeshEnum = 0;

		ValidateAndLockSourceMesh(pSourceMesh, &pVertices, &pIndices, dwNumFaces, dwNumVertices, dwMeshEnum);
		if (dwMeshEnum & D3DXMESH_32BIT)
			for (ULONG_PTR mInd = 0; mInd < dwNumFaces * 3; )
			{
				V a, b, c;

				a = pVertices[pIndices[mInd++]];
				b = pVertices[pIndices[mInd++]];
				c = pVertices[pIndices[mInd++]];

				OnParseTriangle(a, b, c);
			}
		else
		{
			pIndices2 = reinterpret_cast<WORD*>(pIndices);
			for (ULONG_PTR mInd = 0; mInd < dwNumFaces * 3; )
			{
				V a, b, c;

				a = pVertices[pIndices2[mInd++]];
				b = pVertices[pIndices2[mInd++]];
				c = pVertices[pIndices2[mInd++]];

				OnParseTriangle(a, b, c);
			}
		}

		if (FAILED(pSourceMesh->UnlockVertexBuffer()))
			throw std::exception("UnlockVertexBuffer");
		if (FAILED(pSourceMesh->UnlockIndexBuffer()))
			throw std::exception("UnlockIndexBuffer");
	}

	void ParseMesh(const std::wstring& sourceMesh)
	{
		LPD3DXMESH pMeshToParse(NULL);
		if (FAILED(D3DXLoadMeshFromX(
			GetPath(sourceMesh).c_str(),
			D3DXMESH_32BIT | D3DXMESH_IB_MANAGED | D3DXMESH_VB_MANAGED,
			pD3DDevice.Get(),
			NULL,
			NULL,
			NULL,
			NULL,
			&pMeshToParse
		)))
			throw std::exception("D3DXLoadMeshFromX");
		ParseMesh(pMeshToParse);
	}

	void CloneFromMesh(LPD3DXMESH pSourceMesh)
	{
		V* pVertices = nullptr;
		DWORD* pIndices = nullptr;

		// Lock the geometry buffers
		DWORD dwNumFaces = 0;
		DWORD dwNumVertices = 0;
		DWORD dwMeshEnum = 0;

		ValidateAndLockSourceMesh(pSourceMesh, &pVertices, &pIndices, dwNumFaces, dwNumVertices, dwMeshEnum);

		m_Indices.reserve(dwNumFaces * 3);
		m_Vertices.reserve(dwNumVertices);
		if (dwMeshEnum & D3DXMESH_32BIT)
			for (DWORD mInd = 0; mInd < dwNumFaces * 3; mInd++)
				m_Indices.push_back(pIndices[mInd]);
		else
		{
			WORD* pIndices2 = reinterpret_cast<WORD*>(pIndices);
			for (DWORD mInd = 0; mInd < dwNumFaces * 3; mInd++)
				m_Indices.push_back(pIndices2[mInd]);
		}
		for (DWORD mVert = 0; mVert < dwNumVertices; mVert++)
			m_Vertices.push_back(pVertices[mVert]);

		if (FAILED(pSourceMesh->UnlockVertexBuffer()))
			throw std::exception("UnlockVertexBuffer");
		if (FAILED(pSourceMesh->UnlockIndexBuffer()))
			throw std::exception("UnlockIndexBuffer");
	}

	void CommitMesh()
	{
		VOID* faces = nullptr;
		VOID* vertices = nullptr;

		if (FAILED(D3DXCreateMesh(
			FacesCount(),
			(DWORD)m_Vertices.size(),
			D3DXMESH_32BIT | D3DXMESH_IB_MANAGED | D3DXMESH_VB_MANAGED,
			V::Decl,
			pD3DDevice.Get(),
			&m_pMesh)))
			throw std::exception("D3DXCreateMesh");
		if (FAILED(m_pMesh->LockVertexBuffer(NULL, &vertices)))
			throw std::exception("LockVertexBuffer(NULL, &m_Vertices)");
		if (FAILED(m_pMesh->LockIndexBuffer(NULL, &faces)))
			throw std::exception("LockIndexBuffer(NULL, &m_Faces)");
		memcpy(vertices, &m_Vertices[0], sizeof(V) * m_Vertices.size());
		memcpy(faces, &m_Indices[0], sizeof(DWORD) * m_Indices.size());
		if (FAILED(m_pMesh->UnlockIndexBuffer()))
			throw std::exception("UnlockIndexBuffer");
		if (FAILED(m_pMesh->UnlockVertexBuffer()))
			throw std::exception("UnlockVertexBuffer");
	}

	void NormalizeMesh()
	{
		if (FAILED(D3DXComputeNormals(m_pMesh.Get(), NULL)))
			throw std::exception("D3DXComputeNormals");
	}

	void WeldMesh(const float EpsilonOfPosition, const float EpsilonOfNormal)
	{
		Microsoft::WRL::ComPtr<ID3DXBuffer>     AdjacencyBuffer; // adjacency data buffer
		//LPD3DXBUFFER     pD3DXMtrlBuffer  = NULL; // material buffer
		//LPD3DXMESH       mesh            = NULL; // mesh object
		//DWORD            m_dwNumMaterials;        // number of materials
		D3DXWELDEPSILONS epsilons;                // structure with epsilon values
		//DWORD            *pFaceRemap[65536];      // face remapping array
		//DWORD            i;                       // internal variable

		// Load the mesh from the specified file
		DWORD nF3 = m_pMesh->GetNumFaces() * 3;
		if (FAILED(D3DXCreateBuffer(nF3 * (DWORD)sizeof(DWORD), &AdjacencyBuffer)))
			throw std::exception("D3DXCreateBuffer");

		// Set epsilon values
		epsilons =
		{
			.Position = EpsilonOfPosition,
			.BlendWeights = 0,
			.Normal = EpsilonOfNormal,
			.PSize = 0,
			.Tangent = 0,
			.TessFactor = 0
		};

		// Weld the vertices
		/*for( i=0; i < 65536; i++ )
		{
		pFaceRemap[i] = 0;
		}*/

		if (FAILED(D3DXWeldVertices(m_pMesh.Get(),
			D3DXWELDEPSILONS_WELDALL,
			&epsilons,
			NULL,
			(DWORD*)AdjacencyBuffer->GetBufferPointer(),
			//(DWORD*)pFaceRemap,
			NULL,
			NULL)))
			throw std::exception("D3DXWeldVertices");

		if (FAILED(m_pMesh->OptimizeInplace(
			D3DXMESHOPT_DEVICEINDEPENDENT,
			(DWORD*)AdjacencyBuffer->GetBufferPointer(), 0, 0, 0)))
			throw std::exception("OptimizeInplace");
	}

	void CleanMesh()
	{
		LPD3DXMESH pNewMesh = NULL;
		DWORD nF3 = m_pMesh->GetNumFaces() * 3;

		//ComPtr<ID3DXBuffer> pBuffErr, pBuffAdjIn, pBuffAdjOut;
		LPD3DXBUFFER pBuffErr, pBuffAdjIn, pBuffAdjOut;

		if (FAILED(D3DXCreateBuffer(1024, &pBuffErr)))
			throw std::exception("D3DXCreateBuffer");
		if (FAILED(D3DXCreateBuffer(nF3 * sizeof(DWORD), &pBuffAdjIn)))
			throw std::exception("D3DXCreateBuffer");
		if (FAILED(D3DXCreateBuffer(nF3 * sizeof(DWORD), &pBuffAdjOut)))
			throw std::exception("D3DXCreateBuffer");

		if (FAILED(m_pMesh->GenerateAdjacency(0.0f, (DWORD*)(pBuffAdjIn->GetBufferPointer()))))
			throw std::exception("GenerateAdjacency");

		if (FAILED(D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION,
			m_pMesh.Get(),
			(DWORD*)(pBuffAdjIn->GetBufferPointer()),
			&pNewMesh,
			(DWORD*)(pBuffAdjOut->GetBufferPointer()),
			&pBuffErr)))
			throw std::exception("D3DXCleanMesh");

		if (m_pMesh.Get() != pNewMesh)
		{
			m_pMesh.Reset();
			m_pMesh = pNewMesh;
		}
	}

	std::wstring GetPath(const std::wstring& filename)
	{
		return L"D:\\Dokumenty\\Visual Studio 2022\\Projects\\monad_solution\\Apps\\EnchantedCrayon\\Assets\\Meshes\\" + filename;
	}

	void SaveMesh(const std::wstring& path)
	{
		if (FAILED(D3DXSaveMeshToX(
			GetPath(path).c_str(),
			m_pMesh.Get(),
			NULL,
			NULL,
			NULL,
			0,
			D3DXF_FILEFORMAT_TEXT | D3DXF_FILEFORMAT_COMPRESSED)))
			throw std::exception("D3DXSaveMeshToX");
	}

	void SimplifyMesh(const DWORD pMinValue)
	{
		ComPtr<ID3DXBuffer> AdjacencyBuffer;
		DWORD nF3 = m_pMesh->GetNumFaces() * 3;
		D3DXCreateBuffer(nF3 * sizeof(DWORD), &AdjacencyBuffer);
		LPD3DXMESH pNewMesh = NULL;

		if (
			FAILED(
				m_pMesh->GenerateAdjacency(
					0.0f, reinterpret_cast<DWORD*>(AdjacencyBuffer->GetBufferPointer()))))
			throw std::exception("GenerateAdjacency");

		if (
			FAILED(
				D3DXSimplifyMesh(
					m_pMesh.Get(),
					reinterpret_cast<DWORD*>(AdjacencyBuffer->GetBufferPointer()),
					NULL,
					NULL,
					pMinValue,
					D3DXMESHSIMP_VERTEX,
					&pNewMesh)))
			throw std::exception("D3DXSimplifyMesh");
		m_pMesh.Release();
		m_pMesh = pNewMesh;
	}

	void ValidMesh(const std::wstring& path)
	{
		ComPtr<ID3DXBuffer> ErrBuf, AdjacencyBuffer;
		DWORD nF3 = m_pMesh->GetNumFaces() * 3;
		if (FAILED(D3DXCreateBuffer(nF3 * sizeof(DWORD), &AdjacencyBuffer)))
			throw std::exception("D3DXCreateBuffer");
		if (FAILED(
			m_pMesh->GenerateAdjacency(0.0f, (DWORD*)AdjacencyBuffer->GetBufferPointer())))
			throw std::exception("GenerateAdjacency");

		D3DXValidMesh(m_pMesh.Get(), (DWORD*)AdjacencyBuffer->GetBufferPointer(), &ErrBuf);

		HANDLE FileToWrite = CreateFile2(
			FileToWrite,
			path.data(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			CREATE_ALWAYS
		);
		if (INVALID_HANDLE_VALUE == FileToWrite)
			throw std::exception("FileToWrite.Create");
		if (ErrBuf->GetBufferPointer() != NULL)
			if (FAILED(Write(FileToWrite, ErrBuf->GetBufferPointer(), ErrBuf->GetBufferSize())))
				throw std::exception("FileToWrite.Write");
		CloseHandle(FileToWrite);
	}

	void GetMinMax(XMVECTOR& maximum, XMVECTOR& minimum, XMVECTOR& delta)
	{
		maximum = XMLoadFloat3(&m_Vertices[0].m_position);
		minimum = maximum;
		XMVECTOR Source;

		for (size_t iVert = 0; m_Vertices.size() > iVert; iVert++)
		{
			Source = XMLoadFloat3(&m_Vertices[iVert].m_position);
			for (int ArrID = 0; ArrID < 3; ArrID++)
			{
				((float*)&maximum)[ArrID] = max(((float*)&maximum)[ArrID], ((float*)&Source)[ArrID]);
				((float*)&minimum)[ArrID] = min(((float*)&minimum)[ArrID], ((float*)&Source)[ArrID]);
			}
		}

		delta = XMVectorSubtract(maximum, minimum);
	}

	void CalculateCanonicalMesh()
	{
		XMVECTOR vMax;
		XMVECTOR vMin;
		XMVECTOR delta;

		GetMinMax(vMax, vMin, delta);

		// Scale
		float maxDelta = max(max(XMVectorGetX(delta), XMVectorGetY(delta)), XMVectorGetZ(delta));

		for (size_t iVert = 0; m_Vertices.size() > iVert; iVert++)
		{
			XMVECTOR tmp = XMLoadFloat3(&m_Vertices[iVert].m_position);
			tmp = XMVectorScale(tmp, 2 / maxDelta);
			XMStoreFloat3(&m_Vertices[iVert].m_position, tmp);
		}

		// Center
		GetMinMax(vMax, vMin, delta);

		for (size_t iVert = 0; m_Vertices.size() > iVert; iVert++)
		{
			XMVECTOR tmp = XMLoadFloat3(&m_Vertices[iVert].m_position);
			XMVECTOR tmpDelta = XMVectorScale(delta, 0.5f);
			XMVECTOR tmpSum = XMVectorAdd(vMin, tmpDelta);
			tmp = XMVectorSubtract(tmp, tmpSum);
			XMStoreFloat3(&m_Vertices[iVert].m_position, tmp);
		}
	}

	void CalculateCanonicalWallMesh()
	{
		XMVECTOR vMax;
		XMVECTOR vMin;
		XMVECTOR Delta;

		GetMinMax(vMax, vMin, Delta);

		// Scale
		float MaxDelta = max(max(XMVectorGetX(Delta), XMVectorGetY(Delta)), XMVectorGetZ(Delta));

		struct CPostion {
			CPostion(float& maxDelta)
				: mMaxDelta(maxDelta)
			{
			}
			void PosDelta(V& ver)
			{
				ver.m_position.x *= (2 / mMaxDelta);
				ver.m_position.y *= (2 / mMaxDelta);
				ver.m_position.z *= (2 / mMaxDelta);
				/*XMVECTOR tmp = XMLoadFloat3( & );
				tmp *= 2;
				tmp /= mMaxDelta;
				XMStoreFloat3( &ver.m_position, tmp );*/
			}

			const float& mMaxDelta;
		} iterate(MaxDelta);
		for (auto& vert : m_Vertices)
			iterate.PosDelta(vert);
		/*for( CVer iVert = 0; mVertices.GetSize() > iVert; iVert ++ )
		{
		mVertices[ iVert ].m_position *= 2;
		mVertices[ iVert ].m_position /= MaxDelta;
		}/*/

		// Center
		GetMinMax(vMax, vMin, Delta);

		struct CPosition3 {
			CPosition3(XMVECTOR& delta, XMVECTOR& pvMin)
				: mDelta(delta), mvMin(pvMin)
			{
			}
			void operator() (V& ver)
			{
				const XMVECTOR tmp = XMLoadFloat3(&ver.m_position);
				XMStoreFloat3(&ver.m_position, XMVectorSubtract(tmp, (XMVectorAdd(mvMin, XMVectorScale(mDelta, 0.5f)))));
			}
			XMVECTOR& mDelta, & mvMin;
		} mDelta(Delta, vMin);
		for (size_t i = 0; i < m_Vertices.size(); ++i)
			mDelta(m_Vertices[i].m_position);

		struct CPosition4 {
			CPosition4(XMVECTOR& delta)
				: mDelta(delta)
			{
			}
			void PosAbs(V& pVer)
			{
				pVer.m_position.z += abs(XMVectorGetZ(mDelta) / 2);
			}
			XMVECTOR& mDelta;
		} mFinish(Delta);
		for (auto& vert : m_Vertices)
			mFinish.PosAbs(vert);

		for (size_t i = 0; i < m_Vertices.size(); ++i)
			mFinish(m_Vertices[i]);
	}
	LPD3DXMESH GetMesh() const
	{
		return m_pMesh.Get();
	}

	// Variable members
	typedef std::vector<VertexType> CVerticesArray;
	typedef std::vector<DWORD> CInidicesArray;
	CVerticesArray m_Vertices;
	CInidicesArray m_Indices;
protected:
	struct structTriangle
	{
		structTriangle(VertexType a, VertexType b, VertexType c)
		{
			VertA = a.m_position;
			VertB = b.m_position;
			VertC = c.m_position;
		}
		XMFLOAT3 VertA, VertB, VertC;
	};

	Monad::Kernel::UnorderedMap<VertexType, DWORD> mVerticeMap;

	void AddVertex(VertexType& vertex)
	{
		DWORD index;

		if (mVerticeMap.end() == mVerticeMap.find(vertex))
		{
			m_Vertices.push_back(vertex);
			index = (DWORD)m_Vertices.size() - 1;
			mVerticeMap[vertex] = index;
		}
		else
			index = mVerticeMap[vertex];

		m_Indices.push_back(index);
	}

	Microsoft::WRL::ComPtr<ID3DXMesh> m_pMesh;
};

template<typename VNormal>
struct MeshBuilderNormal : MeshBuilder<VNormal>
{
	using VertexNormalType = VNormal;

	XMFLOAT3 GetAvgNormal(const VertexNormalType& a, const VertexNormalType& b, const VertexNormalType& c) const
	{
		XMFLOAT3 res = a.m_normal + b.m_normal + c.m_normal;
		res.x /= 3.0f;
		res.y /= 3.0f;
		res.z /= 3.0f;

		return res;
	}

	XMFLOAT3 GetAvgNormal(const int pTriangle) const
	{
		VertexNormalType a, b, c;
		GetTriangle(pTriangle, a, b, c);

		return GetAvgNormal(a, b, c);
	}

	XMFLOAT3 CalculateTriangleNormal(const VertexNormalType& a, const VertexNormalType& b, const VertexNormalType& c) const
	{
		const XMVECTOR P = XMVectorSubtract(XMLoadFloat3(&b.m_position), XMLoadFloat3(&a.m_position));
		const XMVECTOR Q = XMVectorSubtract(XMLoadFloat3(&c.m_position), XMLoadFloat3(&a.m_position));
		XMVECTOR res = XMVector3Cross(P, Q);
		res = XMVector3Normalize(res);
		XMFLOAT3 ret;
		XMStoreFloat3(&ret, res);

		return ret;
	}

	XMFLOAT3 CalculateTriangleNormal(const int pTriangle) const
	{
		VertexNormalType a, b, c;
		GetTriangle(pTriangle, a, b, c);

		return CalculateTriangleNormal(a, b, c);
	}

	void AddTriangle(const VertexNormalType& a, const VertexNormalType& b, const VertexNormalType& c)
	{
		VertexNormalType aCopy = a, bCopy = b, cCopy = c;

		/*if( CalcTriangleNornal )
		aCopy.m_normal = bCopy.m_normal = cCopy.m_normal = CalculateTriangleNormal( a, b, c );*/

		this->AddVertex(aCopy);
		this->AddVertex(bCopy);
		this->AddVertex(cCopy);
	}

	void AddAndParseTriangle(const VertexNormalType& a, const VertexNormalType b, const VertexNormalType& c, const bool CalcTriangleNornal = false)
	{
		VertexNormalType aCopy = a, bCopy = b, cCopy = c;

		if (CalcTriangleNornal)
			aCopy.m_normal = bCopy.m_normal = cCopy.m_normal = CalculateTriangleNormal(a, b, c);

		OnParseTriangle(aCopy, bCopy, cCopy);
	}

	void AddQuad(const VertexNormalType& a, const VertexNormalType& b, const VertexNormalType& c, const VertexNormalType& d)
	{
		AddTriangle(a, b, c);
		AddTriangle(c, d, a);
	}
};

typedef MeshBuilderNormal<VERTEXTEX> CMeshBuilderTex;
typedef MeshBuilderNormal<VERTEXCOL> CMeshBuilderCol;
typedef MeshBuilderNormal<VERTEXPLAIN> CMeshBuilderPlain;
typedef MeshBuilderNormal<VERTEXPANE> CMeshBuilderPane;