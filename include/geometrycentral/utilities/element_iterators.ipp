namespace geometrycentral {

// ==========================================================
// =============       Range Iterator       =================
// ==========================================================

template <typename F>
inline RangeIteratorBase<F>::RangeIteratorBase(typename F::ParentMeshT* mesh_, size_t iStart_, size_t iEnd_,
                                               ReuseHeap* reuseHeap_, size_t* iterCurrPtr_)
    : mesh(mesh_), iCurr(iStart_), iEnd(iEnd_), reuseHeap(reuseHeap_), iterCurrPtr(iterCurrPtr_) {
  if (iCurr != iEnd && (!F::elementOkay(*mesh, iCurr) || skipIfReused())) {
    this->operator++();
  }
  if (iterCurrPtr) *iterCurrPtr = iCurr;
}

template <typename F>
inline const RangeIteratorBase<F>& RangeIteratorBase<F>::operator++() {
  iCurr++;
  while (iCurr != iEnd) {
    if (!F::elementOkay(*mesh, iCurr)) { iCurr++; continue; }
    if (skipIfReused()) { iCurr++; continue; }
    break;
  }
  if (iterCurrPtr) *iterCurrPtr = iCurr;
  return *this;
}

template <typename F>
inline RangeIteratorBase<F> RangeIteratorBase<F>::operator++(int) {
  RangeIteratorBase<F> tmp = *this;
  ++(*this);
  return tmp;
}

template <typename F>
inline bool RangeIteratorBase<F>::operator==(const RangeIteratorBase<F>& other) const {
  return iCurr == other.iCurr;
}

template <typename F>
inline bool RangeIteratorBase<F>::operator!=(const RangeIteratorBase<F>& other) const {
  return !(*this == other);
}

template <typename F>
inline typename F::Etype RangeIteratorBase<F>::operator*() const {
  return typename F::Etype(mesh, iCurr);
}

template <typename F>
inline bool RangeIteratorBase<F>::skipIfReused() {
  if (!reuseHeap || reuseHeap->empty()) return false;
  GC_SAFETY_ASSERT(reuseHeap->top() >= iCurr, "reuseHeap contains slot behind iterator position — logic error");
  if (reuseHeap->top() == iCurr) {
    reuseHeap->pop();
    return true;
  }
  return false;
}

template <typename F>
RangeSetBase<F>::RangeSetBase(typename F::ParentMeshT* mesh_, size_t iStart_, size_t iEnd_,
                               std::vector<ActiveRange>* rangeList)
    : mesh(mesh_), iStart(iStart_), iEnd(iEnd_), rangeList_(rangeList) {
  if (rangeList_) rangeList_->push_back({&reuseHeap, &iterCurr_, iEnd_});
}

template <typename F>
RangeSetBase<F>::~RangeSetBase() {
  if (rangeList_) {
    auto& v = *rangeList_;
    v.erase(std::remove_if(v.begin(), v.end(),
      [this](const ActiveRange& r) { return r.heap == &reuseHeap; }), v.end());
  }
}

template <typename F>
inline RangeIteratorBase<F> RangeSetBase<F>::begin() const {
  return RangeIteratorBase<F>(mesh, iStart, iEnd, &reuseHeap, &iterCurr_);
}

template <typename F>
inline RangeIteratorBase<F> RangeSetBase<F>::end() const {
  return RangeIteratorBase<F>(mesh, iEnd, iEnd);
}

// ==========================================================
// =============     Navigation Iterator     ================
// ==========================================================

template <typename N>
inline NavigationIteratorBase<N>::NavigationIteratorBase(typename N::Etype e, bool justStarted_)
    : state{e}, justStarted(justStarted_) {

  // checking startingE is a weird hack, but ensures the iterators don't infinite loop if there are no valid elemetnts
  // to return
  typename N::Etype startingE = e; // TODO nsharp changed this recently

  // Advance to first valid element
  while (!state.isValid()) {
    state.advance();
    if (state.currE == startingE) {
      // no valid elements! set this equal to the "end" iterator
      // so we iterate over nothing
      justStarted = false;
      break;
    }
  }
}

template <typename N>
inline const NavigationIteratorBase<N>& NavigationIteratorBase<N>::operator++() {
  state.advance();
  while (!state.isValid()) {
    state.advance();
  }
  justStarted = false;
  return *this;
}

template <typename N>
inline bool NavigationIteratorBase<N>::operator==(const NavigationIteratorBase<N>& other) const {
  return justStarted == other.justStarted && state.currE == other.state.currE;
}

template <typename N>
inline bool NavigationIteratorBase<N>::operator!=(const NavigationIteratorBase<N>& other) const {
  return !(*this == other);
}

template <typename N>
inline typename N::Rtype NavigationIteratorBase<N>::operator*() const {
  return state.getCurrent();
}

template <typename N>
NavigationSetBase<N>::NavigationSetBase(typename N::Etype firstE_)
    : firstE(firstE_), cachedEndIter{NavigationIteratorBase<N>(firstE, false)} {}

template <typename N>
inline NavigationIteratorBase<N> NavigationSetBase<N>::begin() const {
  return NavigationIteratorBase<N>(firstE, true);
}

template <typename N>
inline NavigationIteratorBase<N> NavigationSetBase<N>::end() const {
  return cachedEndIter;
}

}
