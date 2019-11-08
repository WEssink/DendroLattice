#pragma once
#pragma once

#ifndef __DENDROAPI_H__
#define __DENDROAPI_H__

#include "DendroGrid.h"

#ifdef DENDROAPI_EXPORTS
#define DENDRO_API __declspec(dllexport)
#else
#define DENDRO_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	// ovdb volume class constructors
	extern DENDRO_API DendroGrid* DendroCreate();
	extern DENDRO_API void DendroDelete(DendroGrid* grid);
	extern DENDRO_API DendroGrid* DendroDuplicate(DendroGrid * grid);

	extern DENDRO_API bool DendroRead(DendroGrid * grid, const char * filename);
	extern DENDRO_API bool DendroWrite(DendroGrid * grid, const char * filename);

	// volume conversion methods
	extern DENDRO_API bool DendroFromPoints(DendroGrid * grid, double *vPoints, int pCount, double *vRadius, int rCount, double voxelSize, double bandwidth);
	extern DENDRO_API bool DendroFromMesh(DendroGrid * grid, float* vPoints, int vCount, int * vFaces, int fCount, double voxelSize, double bandwidth);

	// volume render methods
	extern DENDRO_API void DendroToMesh(DendroGrid * grid);
	extern DENDRO_API void DendroToMeshSettings(DendroGrid * grid, double isovalue, double adaptivity);

	extern DENDRO_API float* DendroVertexBuffer(DendroGrid * grid, int* size);
	extern DENDRO_API int* DendroFaceBuffer(DendroGrid * grid, int* size);

	// volume transformation methods
	extern DENDRO_API bool DendroTransform(DendroGrid * grid, double* matrix, int mCount);

	// volume csg methods
	extern DENDRO_API void DendroUnion(DendroGrid * grid, DendroGrid * csgGrid);
	extern DENDRO_API void DendroDifference(DendroGrid * grid, DendroGrid * csgGrid);
	extern DENDRO_API void DendroIntersection(DendroGrid * grid, DendroGrid * csgGrid);

	// volume filter methods
	extern DENDRO_API void DendroOffset(DendroGrid * grid, double amount);
	extern DENDRO_API void DendroOffsetMask(DendroGrid * grid, double amount, DendroGrid * mask, double min, double max, bool invert);
	extern DENDRO_API void DendroSmooth(DendroGrid * grid, int type, int iterations, int width);
	extern DENDRO_API void DendroSmoothMask(DendroGrid * grid, int type, int iterations, int width, DendroGrid * mask, double min, double max, bool invert);
	extern DENDRO_API void DendroBlend(DendroGrid * bGrid, DendroGrid * eGrid, double bPosition, double bEnd);
	extern DENDRO_API void DendroBlendMask(DendroGrid * bGrid, DendroGrid * eGrid, double bPosition, double bEnd, DendroGrid * mask, double min, double max, bool invert);

	// volume utilities
	extern DENDRO_API float* DendroClosestPoint(DendroGrid * grid, float* vPoints, int vCount, int* rSize);

	extern DENDRO_API void DendroGridSetValue(DendroGrid *grid, double x, double y, double z, double voxelSize, bool on);
	extern DENDRO_API bool DendroGridGetValue(DendroGrid *grid, double x, double y, double z, double voxelSize);
	extern DENDRO_API void DendroConvertToLattice(DendroGrid *grid, float* t, float* f, float* phi, float* mat, double voxelSize, int latticeType);
	extern DENDRO_API void DendroConvertToCurvedLattice(DendroGrid *grid, float* t, float* f, float* phi, float* mat, float* curveParams, double voxelSize, int latticeType);

	openvdb::Vec3d BendCoordinate(float* curveParams, openvdb::Vec3d point);
	openvdb::Vec3d MultiplyMat3x4(float* mat, openvdb::Vec3d v);
	float GetLatticeValue(openvdb::Vec3d vec, float* t, float* f, float* phi, int latticeType);
	float GetGyroidValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi);
	float GetSchwarzPValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi);
	float GetSchwarzDValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi);
	float GetNeoviusValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi);
	float GetIWPValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi);

	openvdb::Vec3f GetFrequencyValue(openvdb::Vec3d vec, float* f);
	float GetLevelValue(openvdb::Vec3d vec, float* t);

#ifdef __cplusplus
}
#endif

#endif // __DENDROAPI_H__