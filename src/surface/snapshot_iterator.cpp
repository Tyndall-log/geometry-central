#include "geometrycentral/surface/snapshot_iterator.h"

#include <algorithm>

namespace geometrycentral {
namespace surface {

// == SnapshotVertexIterator ==

SnapshotVertexIterator::SnapshotVertexIterator(SurfaceMesh& mesh)
    : mesh_(mesh), snapshotFillCount_(mesh.vertexIndexSize()), current_(0) {
  mesh_.activeVertexSnapshotIterators_.push_back(this);
  skipInvalid();
}

SnapshotVertexIterator::~SnapshotVertexIterator() {
  auto& v = mesh_.activeVertexSnapshotIterators_;
  v.erase(std::remove(v.begin(), v.end(), this), v.end());
}

void SnapshotVertexIterator::advance() {
  current_++;
  skipInvalid();
}

bool SnapshotVertexIterator::isDone() const { return current_ >= snapshotFillCount_; }

Vertex SnapshotVertexIterator::current() const { return Vertex(&mesh_, current_); }

void SnapshotVertexIterator::markReused(size_t idx) {
  if (!reusedHeap_) {
    reusedHeap_ = std::make_unique<MinHeap>();
  }
  reusedHeap_->push(idx);
}

void SnapshotVertexIterator::skipInvalid() {
  while (current_ < snapshotFillCount_) {
    if (mesh_.vertexIsDead(current_)) { current_++; continue; }
    if (reusedHeap_ && !reusedHeap_->empty() && reusedHeap_->top() == current_) {
      reusedHeap_->pop();
      current_++;
      continue;
    }
    break;
  }
}

// == SnapshotEdgeIterator ==

SnapshotEdgeIterator::SnapshotEdgeIterator(SurfaceMesh& mesh)
    : mesh_(mesh), snapshotFillCount_(mesh.edgeIndexSize()), current_(0) {
  mesh_.activeEdgeSnapshotIterators_.push_back(this);
  skipInvalid();
}

SnapshotEdgeIterator::~SnapshotEdgeIterator() {
  auto& v = mesh_.activeEdgeSnapshotIterators_;
  v.erase(std::remove(v.begin(), v.end(), this), v.end());
}

void SnapshotEdgeIterator::advance() {
  current_++;
  skipInvalid();
}

bool SnapshotEdgeIterator::isDone() const { return current_ >= snapshotFillCount_; }

Edge SnapshotEdgeIterator::current() const { return Edge(&mesh_, current_); }

void SnapshotEdgeIterator::markReused(size_t idx) {
  if (!reusedHeap_) {
    reusedHeap_ = std::make_unique<MinHeap>();
  }
  reusedHeap_->push(idx);
}

void SnapshotEdgeIterator::skipInvalid() {
  while (current_ < snapshotFillCount_) {
    if (mesh_.edgeIsDead(current_)) { current_++; continue; }
    if (reusedHeap_ && !reusedHeap_->empty() && reusedHeap_->top() == current_) {
      reusedHeap_->pop();
      current_++;
      continue;
    }
    break;
  }
}

// == SnapshotFaceIterator ==

SnapshotFaceIterator::SnapshotFaceIterator(SurfaceMesh& mesh)
    : mesh_(mesh), snapshotFillCount_(mesh.faceIndexSize()), current_(0) {
  mesh_.activeFaceSnapshotIterators_.push_back(this);
  skipInvalid();
}

SnapshotFaceIterator::~SnapshotFaceIterator() {
  auto& v = mesh_.activeFaceSnapshotIterators_;
  v.erase(std::remove(v.begin(), v.end(), this), v.end());
}

void SnapshotFaceIterator::advance() {
  current_++;
  skipInvalid();
}

bool SnapshotFaceIterator::isDone() const { return current_ >= snapshotFillCount_; }

Face SnapshotFaceIterator::current() const { return Face(&mesh_, current_); }

void SnapshotFaceIterator::markReused(size_t idx) {
  if (!reusedHeap_) {
    reusedHeap_ = std::make_unique<MinHeap>();
  }
  reusedHeap_->push(idx);
}

void SnapshotFaceIterator::skipInvalid() {
  while (current_ < snapshotFillCount_) {
    if (mesh_.faceIsDead(current_)) { current_++; continue; }
    if (reusedHeap_ && !reusedHeap_->empty() && reusedHeap_->top() == current_) {
      reusedHeap_->pop();
      current_++;
      continue;
    }
    break;
  }
}

} // namespace surface
} // namespace geometrycentral
