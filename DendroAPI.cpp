// DendroAPI.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "DendroAPI.h"

#include"DendroParticle.h"
#include"DendroMesh.h"
#include <openvdb/util/Util.h>
#include <openvdb/tools/Interpolation.h>
#include <vector>
#include <openvdb/openvdb.h>
#include <openvdb/Grid.h>
#include <functional>
#include <openvdb/tools/SignedFloodFill.h>

// grid class constructors
DENDRO_API DendroGrid* DendroCreate()
{
	DendroGrid *grid = new DendroGrid();
	return grid;
}

DENDRO_API void DendroDelete(DendroGrid * grid)
{
	if (grid != NULL) {
		delete grid;
		grid = NULL;
	}
}

DENDRO_API DendroGrid* DendroDuplicate(DendroGrid * grid)
{
	DendroGrid *dup = new DendroGrid(grid);
	return dup;
}

DENDRO_API bool DendroRead(DendroGrid * grid, const char * filename)
{
	return grid->Read(filename);
}

DENDRO_API bool DendroWrite(DendroGrid * grid, const char * filename)
{
	return grid->Write(filename);
}


// grid conversion methods
DENDRO_API bool DendroFromPoints(DendroGrid * grid, double *vPoints, int pCount, double *vRadius, int rCount, double voxelSize, double bandwidth)
{
	double inverseVoxelSize = 1.0 / voxelSize;

	std::vector<openvdb::Vec3R> particleList;

	int i = 0;
	while (i < pCount)
	{
		double x = vPoints[i]; // *inverseVoxelSize;
		double y = vPoints[i + 1]; // * inverseVoxelSize;
		double z = vPoints[i + 2]; // * inverseVoxelSize;

		particleList.push_back(openvdb::Vec3R(x, y, z));

		i += 3;
	}

	DendroParticle ps;
	ps.clear();

	if (particleList.size() == rCount)
	{

		int i = 0;
		for (auto it = particleList.begin(); it != particleList.end(); ++it)
		{
			ps.add((*it), openvdb::Real(vRadius[i]));
			i++;
		}
	}
	else
	{

		double average = 0.0;
		for (int i = 0; i < rCount; i++)
		{
			average += vRadius[i];
		}
		average /= rCount;
		openvdb::Real radius = openvdb::Real(average);

		for (auto it = particleList.begin(); it != particleList.end(); ++it)
		{
			ps.add((*it), radius);
		}
	}

	return grid->CreateFromPoints(ps, voxelSize, bandwidth);

}

DENDRO_API bool DendroFromMesh(DendroGrid * grid, float* vPoints, int vCount, int * vFaces, int fCount, double voxelSize, double bandwidth)
{
	double inverseVoxelSize = 1.0 / voxelSize;

	DendroMesh vMesh;
	vMesh.Clear();

	int i = 0;
	while (i < vCount) {

		openvdb::Vec3s vertex(vPoints[i], vPoints[i + 1], vPoints[i + 2]);

		vertex *= inverseVoxelSize;

		vMesh.AddVertice(vertex);

		i += 3;
	}

	i = 0;
	while (i<fCount) {
		openvdb::Vec4I face(vFaces[i], vFaces[i + 1], vFaces[i + 2], openvdb::util::INVALID_IDX);

		vMesh.AddFace(face);
		i += 3;
	}

	return grid->CreateFromMesh(vMesh, voxelSize, bandwidth);

}


// grid render methods
DENDRO_API void DendroToMesh(DendroGrid * grid)
{
	grid->UpdateDisplay();
}

DENDRO_API void DendroToMeshSettings(DendroGrid * grid, double isovalue, double adaptivity)
{
	grid->UpdateDisplay(isovalue, adaptivity);
}

DENDRO_API float* DendroVertexBuffer(DendroGrid * grid, int * size)
{
	float *verticeArray = grid->GetMeshVertices();

	*size = grid->GetVertexCount();

	return verticeArray;
}

DENDRO_API int * DendroFaceBuffer(DendroGrid * grid, int * size)
{
	int *faceArray = grid->GetMeshFaces();

	*size = grid->GetFaceCount();

	return faceArray;
}


// grid transformation methods
DENDRO_API bool DendroTransform(DendroGrid *grid, double *matrix, int mCount)
{
	if (mCount != 16) {
		return false;
	}

	openvdb::math::Mat4d xform = openvdb::math::Mat4d(matrix[0], matrix[1], matrix[2], matrix[3],
		matrix[4], matrix[5], matrix[6], matrix[7],
		matrix[8], matrix[9], matrix[10], matrix[11],
		matrix[12], matrix[13], matrix[14], matrix[15]);

	grid->Transform(xform);

	return true;
}


// grid csg methods
DENDRO_API void DendroUnion(DendroGrid * grid, DendroGrid * csgGrid)
{
	grid->BooleanUnion(*csgGrid);
}

DENDRO_API void DendroDifference(DendroGrid * grid, DendroGrid * csgGrid)
{
	grid->BooleanDifference(*csgGrid);
}

DENDRO_API void DendroIntersection(DendroGrid * grid, DendroGrid * csgGrid)
{
	grid->BooleanIntersection(*csgGrid);
}


// grid filter methods
DENDRO_API void DendroOffset(DendroGrid * grid, double amount)
{
	grid->Offset(amount);
}

DENDRO_API void DendroOffsetMask(DendroGrid * grid, double amount, DendroGrid * mask, double min, double max, bool invert)
{
	grid->Offset(amount, *mask, min, max, invert);
}

DENDRO_API void DendroSmooth(DendroGrid * grid, int type, int iterations, int width)
{
	grid->Smooth(type, iterations, width);
}

DENDRO_API void DendroSmoothMask(DendroGrid * grid, int type, int iterations, int width, DendroGrid * mask, double min, double max, bool invert)
{
	grid->Smooth(type, iterations, width, *mask, min, max, invert);
}

DENDRO_API void DendroBlend(DendroGrid * bGrid, DendroGrid * eGrid, double bPosition, double bEnd)
{
	bGrid->Blend(*eGrid, bPosition, bEnd);
}

DENDRO_API void DendroBlendMask(DendroGrid * bGrid, DendroGrid * eGrid, double bPosition, double bEnd, DendroGrid * mask, double min, double max, bool invert)
{
	bGrid->Blend(*eGrid, bPosition, bEnd, *mask, min, max, invert);
}

// volume utilities
DENDRO_API float * DendroClosestPoint(DendroGrid * grid, float * vPoints, int vCount, int * rSize)
{
	std::vector<openvdb::Vec3R> points;
	std::vector<float> distances;

	int i = 0;
	while (i < vCount) {

		openvdb::Vec3R vertex(vPoints[i], vPoints[i + 1], vPoints[i + 2]);

		points.push_back(vertex);

		i += 3;
	}

	grid->ClosestPoint(points, distances);

	*rSize = points.size() * 3;

	float *pArray = reinterpret_cast<float*>(malloc(*rSize * sizeof(float)));

	i = 0;
	for (auto it = points.begin(); it != points.end(); ++it) {
		pArray[i] = it->x();
		pArray[i + 1] = it->y();
		pArray[i + 2] = it->z();
		i += 3;
	}

	return pArray;
}

DENDRO_API void DendroGridSetValue(DendroGrid *grid, double x, double y, double z, double voxelSize, bool on)
{
	openvdb::Vec3d worldSpacePoint(x, y, z);
	openvdb::math::Transform::Ptr linearTransform =
		openvdb::math::Transform::createLinearTransform(voxelSize);
	openvdb::Vec3d indexSpacePoint = linearTransform->worldToIndex(worldSpacePoint);
	openvdb::math::Coord coord(indexSpacePoint.x(), indexSpacePoint.y(), indexSpacePoint.z());
	if (on) {
		grid->mGrid->getAccessor().setValue(coord, voxelSize);
	}
	else {
		grid->mGrid->getAccessor().setValue(coord, 0);
	}

}

//DENDRO_API void DendroConvertToGyroid(DendroGrid *grid, float t, float f, double voxelSize, int latticeType) 
//{
//	openvdb::math::Transform::Ptr linearTransform =
//		openvdb::math::Transform::createLinearTransform(voxelSize);
//	grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ();
//	for (openvdb::math::CoordBBox::XYZIterator iter = grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ(); iter; ++iter) {
//
//		openvdb::Vec3d worldSpacePoint = linearTransform->indexToWorld(*iter);
//		float value = GetLatticeValue(worldSpacePoint, t, f, latticeType);
//		grid->mGrid->getAccessor().setValue(*iter, value);
//	}
//}

DENDRO_API void DendroConvertToLattice(DendroGrid *grid, float* t, float* f, float* phi, float* mat, double voxelSize, int latticeType)
{
	openvdb::math::Transform::Ptr linearTransform =
		openvdb::math::Transform::createLinearTransform(voxelSize);
	grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ();
	for (openvdb::math::CoordBBox::XYZIterator iter = grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ(); iter; ++iter) {

		openvdb::Vec3d worldSpacePoint = linearTransform->indexToWorld(*iter);
		worldSpacePoint = MultiplyMat3x4(mat, worldSpacePoint);
		float value = GetLatticeValue(worldSpacePoint, t, f, phi, latticeType);
		grid->mGrid->getAccessor().setValue(*iter, value);
	}
}

DENDRO_API void DendroConvertToCurvedLattice(DendroGrid *grid, float* t, float* f, float* phi, float* mat, float* curveParams, double voxelSize, int latticeType)
{
	openvdb::math::Transform::Ptr linearTransform =
		openvdb::math::Transform::createLinearTransform(voxelSize);
	grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ();
	for (openvdb::math::CoordBBox::XYZIterator iter = grid->mGrid->evalActiveVoxelBoundingBox().beginXYZ(); iter; ++iter) {

		openvdb::Vec3d worldSpacePoint = linearTransform->indexToWorld(*iter);
		worldSpacePoint = MultiplyMat3x4(mat, worldSpacePoint);
		worldSpacePoint = BendCoordinate(curveParams, worldSpacePoint);
		float value = GetLatticeValue(worldSpacePoint, t, f, phi, latticeType);
		grid->mGrid->getAccessor().setValue(*iter, value);
	}
}

openvdb::Vec3d BendCoordinate(float* curveParams, openvdb::Vec3d point) {
	return openvdb::Vec3d(point.x() + curveParams[2] * point.y() * point.y() + curveParams[4] * point.z() * point.z(),
						  curveParams[0] * point.x() * point.x() + point.y() + curveParams[5] * point.y() * point.y(),
						  curveParams[1] * point.x() * point.x() + curveParams[3] * point.y() * point.y() + point.z());
}

openvdb::Vec3d MultiplyMat3x4(float* mat, openvdb::Vec3d v) {
	return openvdb::Vec3d((mat[0] *  v.x() + mat[1] * v.y() + mat[2] * v.z()) + mat[3], 
						  (mat[4] *  v.x() + mat[5] * v.y() + mat[6] * v.z()) + mat[7], 
						  (mat[8] *  v.x() + mat[9] * v.y() + mat[10] * v.z()) + mat[11]);
}

float GetLatticeValue(openvdb::Vec3d vec, float* t, float* f, float* phi,  int latticeType) {
	if (latticeType == 0) {
		return GetGyroidValue(vec, GetLevelValue(vec, t), GetFrequencyValue(vec, f), phi);
	}
	else if (latticeType == 1) {
		return GetSchwarzPValue(vec, GetLevelValue(vec, t), GetFrequencyValue(vec, f), phi);
	}
	else if (latticeType == 2) {
		return GetSchwarzDValue(vec, GetLevelValue(vec, t), GetFrequencyValue(vec, f), phi);
	}
	else if (latticeType == 3) {
		return GetNeoviusValue(vec, GetLevelValue(vec, t), GetFrequencyValue(vec, f), phi);
	}
	else if (latticeType == 4) {
		return GetIWPValue(vec, GetLevelValue(vec, t), GetFrequencyValue(vec, f), phi);
	}
	return 0;
}

//Lattice equations with coordinate location
float GetGyroidValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi) {

	return (sin(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) + sin(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2]) + sin(f.z() * vec.z() - phi[2]) * cos(f.x() * vec.x() - phi[0]) - t);
}

float GetSchwarzPValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi) {
	return (cos(f.x() * vec.x() - phi[0]) + cos(f.y() * vec.y() - phi[1]) + cos(f.z() * vec.z() - phi[2]) - t);
}

float GetSchwarzDValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi) {
	return (sin(f.x() * vec.x() - phi[0]) * sin(f.y() * vec.y() - phi[1]) * sin(f.z() * vec.z() - phi[2]) + sin(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2]) + cos(f.x() * vec.x() - phi[0]) * sin(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2]) + cos(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) * sin(f.z() * vec.z() - phi[2]) - t);
}

float GetNeoviusValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi) {
	return (3 * (cos(f.x() * vec.x() - phi[0]) + cos(f.y() * vec.y() - phi[1]) + cos(f.z() * vec.z() - phi[2])) + 4 * (cos(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2])) - t);
}

float GetIWPValue(openvdb::Vec3d vec, float t, openvdb::Vec3d f, float* phi) {
	return (cos(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) + cos(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2]) + cos(f.z() * vec.z() - phi[2]) * cos(f.x() * vec.x() - phi[0]) - cos(f.x() * vec.x() - phi[0]) * cos(f.y() * vec.y() - phi[1]) * cos(f.z() * vec.z() - phi[2]) - t);
}

openvdb::Vec3f GetFrequencyValue(openvdb::Vec3d vec, float* f) {
	return openvdb::Vec3f(f[0] * vec.x() + f[1] * vec.x() * vec.x() + f[6], f[2] * vec.y() + f[3] * vec.y() * vec.y() + f[6], f[4] * vec.z() + f[5] * vec.z() * vec.z() + f[6]);
}

float GetLevelValue(openvdb::Vec3d vec, float* t) {
	return t[0] * vec.x() + t[1] * vec.x() * vec.x() + t[2] * vec.y() + t[3] * vec.y() * vec.y() + t[4] * vec.z() + t[5] * vec.z() * vec.z() + t[6];
}

DENDRO_API bool DendroGridGetValue(DendroGrid *grid, double x, double y, double z, double voxelSize)
{
	openvdb::FloatGrid::Accessor accessor = grid->mGrid->getAccessor();
	openvdb::Vec3d worldSpacePoint(x, y, z);
	openvdb::math::Transform::Ptr linearTransform =
		openvdb::math::Transform::createLinearTransform(voxelSize);
	openvdb::Vec3d indexSpacePoint = linearTransform->worldToIndex(worldSpacePoint);
	openvdb::math::Coord coord(indexSpacePoint.x(), indexSpacePoint.y(), indexSpacePoint.z());
	return accessor.isValueOn(coord);
}
