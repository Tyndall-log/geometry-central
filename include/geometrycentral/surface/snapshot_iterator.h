#pragma once

#include "geometrycentral/surface/surface_mesh.h"

#include <functional>
#include <memory>
#include <queue>
#include <vector>

namespace geometrycentral {
namespace surface {

// SnapshotIterator: iterates over elements that existed at construction time.
// Elements added to reused slots during iteration are automatically skipped
// via a lazily-initialized min-heap that the mesh allocator populates.
//
// Usage:
//   for (auto it = SnapshotVertexIterator(mesh); !it.isDone(); it.advance()) {
//     Vertex v = it.current();
//     // safe to do in-place topology modification here
//   }
//
// Thread safety:
//   Modifying the mesh from another thread during snapshot iteration is undefined behavior.
//   The lock system (07_lock_plan) prevents this.

class SnapshotVertexIterator {
public:
  explicit SnapshotVertexIterator(SurfaceMesh& mesh);
  ~SnapshotVertexIterator();

  SnapshotVertexIterator(const SnapshotVertexIterator&) = delete;
  SnapshotVertexIterator& operator=(const SnapshotVertexIterator&) = delete;

  void advance();
  bool isDone() const;
  Vertex current() const;
  size_t range() const { return snapshotFillCount_; }

  // Called by SurfaceMesh allocator when a free list slot is reused
  void markReused(size_t idx);

private:
  SurfaceMesh& mesh_;
  size_t snapshotFillCount_;
  size_t current_;

  // Lazily initialized on first markReused call
  using MinHeap = std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>>;
  std::unique_ptr<MinHeap> reusedHeap_;

  void skipInvalid();
};

class SnapshotEdgeIterator {
public:
  explicit SnapshotEdgeIterator(SurfaceMesh& mesh);
  ~SnapshotEdgeIterator();

  SnapshotEdgeIterator(const SnapshotEdgeIterator&) = delete;
  SnapshotEdgeIterator& operator=(const SnapshotEdgeIterator&) = delete;

  void advance();
  bool isDone() const;
  Edge current() const;
  size_t range() const { return snapshotFillCount_; }

  void markReused(size_t idx);

private:
  SurfaceMesh& mesh_;
  size_t snapshotFillCount_;
  size_t current_;
  using MinHeap = std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>>;
  std::unique_ptr<MinHeap> reusedHeap_;

  void skipInvalid();
};

class SnapshotFaceIterator {
public:
  explicit SnapshotFaceIterator(SurfaceMesh& mesh);
  ~SnapshotFaceIterator();

  SnapshotFaceIterator(const SnapshotFaceIterator&) = delete;
  SnapshotFaceIterator& operator=(const SnapshotFaceIterator&) = delete;

  void advance();
  bool isDone() const;
  Face current() const;
  size_t range() const { return snapshotFillCount_; }

  void markReused(size_t idx);

private:
  SurfaceMesh& mesh_;
  size_t snapshotFillCount_;
  size_t current_;
  using MinHeap = std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>>;
  std::unique_ptr<MinHeap> reusedHeap_;

  void skipInvalid();
};

} // namespace surface
} // namespace geometrycentral
