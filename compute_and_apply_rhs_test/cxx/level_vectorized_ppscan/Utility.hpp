#ifndef HOMMEXX_UTILITY_HPP
#define HOMMEXX_UTILITY_HPP

#include "Types.hpp"
#include "ExecSpaceDefs.hpp"

#ifndef NDEBUG
#define DEBUG_PRINT(...)                                                       \
  { printf(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...)                                                       \
  {}
#endif

namespace Homme {

// ================ Subviews of 2d views ======================= //
// Note: we still template on ScalarType (should always be Homme::Real here)
//       to allow const/non-const version
template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM1], MemSpace>
subview(ViewType<ScalarType * [DIM1], MemSpace, MemManagement> v_in, int ie) {
  return ViewUnmanaged<ScalarType[DIM1], MemSpace>(&v_in(ie, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM1][DIM2], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2], MemSpace, MemManagement> v_in,
        int ie) {
  return ViewUnmanaged<ScalarType[DIM1][DIM2], MemSpace>(&v_in(ie, 0, 0));
}

// Here, usually, DIM1=DIM2=2 (D and DInv)
template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4>
KOKKOS_INLINE_FUNCTION
ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3][DIM4], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4], MemSpace, MemManagement>
            v_in, int ie) {
  return ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3][DIM4], MemSpace>(
      &v_in(ie, 0, 0, 0, 0));
}

// ================ Subviews of 3d views ======================= //
// Note: we still template on ScalarType (should always be Homme::Scalar here)
//       to allow const/non-const version

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3], MemSpace, MemManagement> v_in,
        int ie) {
  return ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3], MemSpace>(
      &v_in(ie, 0, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM3], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3], MemSpace, MemManagement> v_in,
        int ie, int igp, int jgp) {
  return ViewUnmanaged<ScalarType[DIM3], MemSpace>(&v_in(ie, igp, jgp, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM3][DIM4], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4], MemSpace, MemManagement>
            v_in, int ie, int igp, int jgp) {
  return ViewUnmanaged<ScalarType[DIM3][DIM4], MemSpace>(
      &v_in(ie, igp, jgp, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4], MemSpace>
subview(
    ViewType<ScalarType[DIM1][DIM2][DIM3][DIM4], MemSpace, MemManagement> v_in,
    int var) {
  return ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4], MemSpace>(
      &v_in(var, 0, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4], MemSpace, MemManagement>
            v_in, int ie, int idim) {
  return ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4], MemSpace>(
      &v_in(ie, idim, 0, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4, int DIM5>
KOKKOS_INLINE_FUNCTION
ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3][DIM4][DIM5], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4][DIM5], MemSpace,
                 MemManagement> v_in,
        int ie) {
  return ViewUnmanaged<ScalarType[DIM1][DIM2][DIM3][DIM4][DIM5], MemSpace>(
      &v_in(ie, 0, 0, 0, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4, int DIM5>
KOKKOS_INLINE_FUNCTION
ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4][DIM5], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4][DIM5], MemSpace,
                 MemManagement> v_in,
        int ie, int idim1) {
  return ViewUnmanaged<ScalarType[DIM2][DIM3][DIM4][DIM5], MemSpace>(
      &v_in(ie, idim1, 0, 0, 0, 0));
}

template <typename MemSpace, typename MemManagement, typename ScalarType,
          int DIM1, int DIM2, int DIM3, int DIM4, int DIM5>
KOKKOS_INLINE_FUNCTION ViewUnmanaged<ScalarType[DIM3][DIM4][DIM5], MemSpace>
subview(ViewType<ScalarType * [DIM1][DIM2][DIM3][DIM4][DIM5], MemSpace,
                 MemManagement> v_in,
        int ie, int idim1, int idim2) {
  return ViewUnmanaged<ScalarType[DIM3][DIM4][DIM5], MemSpace>(
      &v_in(ie, idim1, idim2, 0, 0, 0));
}

// Templates to verify at compile time that a view has the specified array type
template <typename ViewT, typename ArrayT> struct exec_view_mappable {
  using exec_view = ExecViewUnmanaged<ArrayT>;
  static constexpr bool value = Kokkos::Impl::ViewMapping<
      typename ViewT::traits, typename exec_view::traits, void>::is_assignable;
};

template <typename ViewT, typename ArrayT> struct host_view_mappable {
  using host_view = HostViewUnmanaged<ArrayT>;
  static constexpr bool value = Kokkos::Impl::ViewMapping<
      typename ViewT::traits, typename host_view::traits, void>::is_assignable;
};

// Kokkos views cannot be used to determine which overloaded function to call,
// so implement this check ourselves with enable_if.
// Despite the ugly templates, this provides much better error messages
// These functions synchronize views from the Fortran layout to the Kernel
// layout
template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar * [NUM_TIME_LEVELS][NP][NP][NUM_LEV]>::
        value &&host_view_mappable<
            Dest_T, Real * [NUM_TIME_LEVELS][NUM_PHYSICAL_LEV][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source, Dest_T dest) {
  ExecViewUnmanaged<Scalar * [NUM_TIME_LEVELS][NP][NP][NUM_LEV]>::HostMirror
  source_mirror(Kokkos::create_mirror_view(source));
  Kokkos::deep_copy(source_mirror, source);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int time = 0; time < NUM_TIME_LEVELS; ++time) {
      for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
           ++vector_level) {
        for (int vector = 0; vector < VECTOR_SIZE && level < NUM_PHYSICAL_LEV;
             ++vector, ++level) {
          for (int igp = 0; igp < NP; ++igp) {
            for (int jgp = 0; jgp < NP; ++jgp) {
              dest(ie, time, level, igp, jgp) =
                  source_mirror(ie, time, igp, jgp, vector_level)[vector];
            }
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar * [NP][NP][NUM_LEV]>::value &&
        host_view_mappable<Dest_T, Real * [NUM_PHYSICAL_LEV][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source, Dest_T dest) {
  ExecViewUnmanaged<Scalar * [NP][NP][NUM_LEV]>::HostMirror source_mirror(
      Kokkos::create_mirror_view(source));
  Kokkos::deep_copy(source_mirror, source);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int igp = 0; igp < NP; ++igp) {
          for (int jgp = 0; jgp < NP; ++jgp) {
            dest(ie, level, igp, jgp) =
                source_mirror(ie, igp, jgp, vector_level)[vector];
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar[NP][NP][NUM_LEV]>::value &&
        host_view_mappable<Dest_T, Real[NUM_PHYSICAL_LEV][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source, Dest_T dest) {
  ExecViewUnmanaged<Scalar[NP][NP][NUM_LEV]>::HostMirror source_mirror(
      Kokkos::create_mirror_view(source));
  Kokkos::deep_copy(source_mirror, source);
  for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
       ++vector_level) {
    for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
      for (int igp = 0; igp < NP; ++igp) {
        for (int jgp = 0; jgp < NP; ++jgp) {
          dest(level, igp, jgp) = source_mirror(igp, jgp, vector_level)[vector];
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar * [2][NP][NP][NUM_LEV]>::value &&
        host_view_mappable<Dest_T, Real * [NUM_PHYSICAL_LEV][2][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source, Dest_T dest) {
  ExecViewUnmanaged<Scalar * [2][NP][NP][NUM_LEV]>::HostMirror source_mirror(
      Kokkos::create_mirror_view(source));
  Kokkos::deep_copy(source_mirror, source);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int dim = 0; dim < 2; ++dim) {
          for (int igp = 0; igp < NP; ++igp) {
            for (int jgp = 0; jgp < NP; ++jgp) {
              dest(ie, level, dim, igp, jgp) =
                  source_mirror(ie, dim, igp, jgp, vector_level)[vector];
            }
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T,
                       Scalar * [Q_NUM_TIME_LEVELS][QSIZE_D][NP][NP][NUM_LEV]>::
        value &&host_view_mappable<
            Dest_T, Real * [Q_NUM_TIME_LEVELS][QSIZE_D][NUM_PHYSICAL_LEV][NP]
                                              [NP]>::value,
    void>::type
sync_to_host(Source_T source, Dest_T dest) {
  typename Source_T::HostMirror source_mirror(
      Kokkos::create_mirror_view(source));
  Kokkos::deep_copy(source_mirror, source);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int time = 0; time < Q_NUM_TIME_LEVELS; ++time) {
      for (int tracer = 0; tracer < QSIZE_D; ++tracer) {
        for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
             ++vector_level) {
          for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
            for (int igp = 0; igp < NP; ++igp) {
              for (int jgp = 0; jgp < NP; ++jgp) {
                dest(ie, time, tracer, level, igp, jgp) = source_mirror(
                    ie, time, tracer, igp, jgp, vector_level)[vector];
              }
            }
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar * [NUM_TIME_LEVELS][NP][NP][NUM_LEV]>::
        value &&host_view_mappable<
            Dest_T, Real * [NUM_TIME_LEVELS][NUM_PHYSICAL_LEV][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source_1, Source_T source_2, Dest_T dest) {
  ExecViewUnmanaged<Scalar * [NUM_TIME_LEVELS][NP][NP][NUM_LEV]>::HostMirror
  source_1_mirror(Kokkos::create_mirror_view(source_1)),
      source_2_mirror(Kokkos::create_mirror_view(source_2));
  Kokkos::deep_copy(source_1_mirror, source_1);
  Kokkos::deep_copy(source_2_mirror, source_2);
  for (int ie = 0; ie < source_1.extent_int(0); ++ie) {
    for (int time = 0; time < NUM_TIME_LEVELS; ++time) {
      for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
           ++vector_level) {
        for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
          for (int igp = 0; igp < NP; ++igp) {
            for (int jgp = 0; jgp < NP; ++jgp) {
              dest(ie, time, level, 0, igp, jgp) =
                  source_1_mirror(ie, time, igp, jgp, vector_level)[vector];
              dest(ie, time, level, 1, igp, jgp) =
                  source_2_mirror(ie, time, igp, jgp, vector_level)[vector];
            }
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    exec_view_mappable<Source_T, Scalar * [NP][NP][NUM_LEV]>::value &&
        host_view_mappable<Dest_T, Real * [NUM_PHYSICAL_LEV][2][NP][NP]>::value,
    void>::type
sync_to_host(Source_T source_1, Source_T source_2, Dest_T dest) {
  ExecViewUnmanaged<Scalar * [NP][NP][NUM_LEV]>::HostMirror source_1_mirror(
      Kokkos::create_mirror_view(source_1)),
      source_2_mirror(Kokkos::create_mirror_view(source_2));
  Kokkos::deep_copy(source_1_mirror, source_1);
  Kokkos::deep_copy(source_2_mirror, source_2);
  for (int ie = 0; ie < source_1.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int igp = 0; igp < NP; ++igp) {
          for (int jgp = 0; jgp < NP; ++jgp) {
            dest(ie, level, 0, igp, jgp) =
                source_1_mirror(ie, igp, jgp, vector_level)[vector];
            dest(ie, level, 1, igp, jgp) =
                source_2_mirror(ie, igp, jgp, vector_level)[vector];
          }
        }
      }
    }
  }
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    host_view_mappable<Source_T, Real * [NUM_PHYSICAL_LEV][NP][NP]>::value &&
        exec_view_mappable<Dest_T, Scalar * [NP][NP][NUM_LEV]>::value,
    void>::type
sync_to_device(Source_T source, Dest_T dest) {
  typename Dest_T::HostMirror dest_mirror = Kokkos::create_mirror_view(dest);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int igp = 0; igp < NP; ++igp) {
          for (int jgp = 0; jgp < NP; ++jgp) {
            dest_mirror(ie, igp, jgp, vector_level)[vector] =
                source(ie, level, igp, jgp);
          }
        }
      }
    }
  }
  Kokkos::deep_copy(dest, dest_mirror);
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    host_view_mappable<Source_T, Real * [NUM_PHYSICAL_LEV][2][NP][NP]>::value &&
        exec_view_mappable<Dest_T, Scalar * [NP][NP][NUM_LEV]>::value,
    void>::type
sync_to_device(Source_T source, Dest_T dest_1, Dest_T dest_2) {
  typename Dest_T::HostMirror dest_1_mirror =
      Kokkos::create_mirror_view(dest_1);
  typename Dest_T::HostMirror dest_2_mirror =
      Kokkos::create_mirror_view(dest_2);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int igp = 0; igp < NP; ++igp) {
          for (int jgp = 0; jgp < NP; ++jgp) {
            dest_1_mirror(ie, igp, jgp, vector_level)[vector] =
                source(ie, level, 0, igp, jgp);
            dest_2_mirror(ie, igp, jgp, vector_level)[vector] =
                source(ie, level, 1, igp, jgp);
          }
        }
      }
    }
  }
  Kokkos::deep_copy(dest_1, dest_1_mirror);
  Kokkos::deep_copy(dest_2, dest_2_mirror);
}

template <typename Source_T, typename Dest_T>
typename std::enable_if<
    host_view_mappable<Source_T, Real * [NUM_PHYSICAL_LEV][2][NP][NP]>::value &&
        exec_view_mappable<Dest_T, Scalar * [2][NP][NP][NUM_LEV]>::value,
    void>::type
sync_to_device(Source_T source, Dest_T dest) {
  typename Dest_T::HostMirror dest_mirror = Kokkos::create_mirror_view(dest);
  for (int ie = 0; ie < source.extent_int(0); ++ie) {
    for (int vector_level = 0, level = 0; vector_level < NUM_LEV;
         ++vector_level) {
      for (int vector = 0; vector < VECTOR_SIZE; ++vector, ++level) {
        for (int dim = 0; dim < 2; ++dim) {
          for (int igp = 0; igp < NP; ++igp) {
            for (int jgp = 0; jgp < NP; ++jgp) {
              dest_mirror(ie, dim, igp, jgp, vector_level)[vector] =
                  source(ie, level, dim, igp, jgp);
            }
          }
        }
      }
    }
  }
  Kokkos::deep_copy(dest, dest_mirror);
}

template <typename FPType>
KOKKOS_INLINE_FUNCTION constexpr FPType min(const FPType &val_1,
                                            const FPType &val_2) {
  return val_1 < val_2 ? val_1 : val_2;
}

template <typename FPType, typename... FPPack>
KOKKOS_INLINE_FUNCTION constexpr FPType min(const FPType &val, FPPack... pack) {
  return val < min(pack...) ? val : min(pack...);
}

template <typename ViewType>
typename std::enable_if<
    !std::is_same<typename ViewType::value_type, Scalar>::value, Real>::type
frobenius_norm(const ViewType view) {
  typename ViewType::pointer_type data = view.data();

  size_t length = view.size();

  // Note: use Kahan algorithm to increase accuracy
  Real norm = 0;
  Real c = 0;
  Real temp, y;
  for (size_t i = 0; i < length; ++i) {
    y = data[i] * data[i] - c;
    temp = norm + y;
    c = (temp - norm) - y;
    norm = temp;
  }

  return std::sqrt(norm);
}

template <typename ViewType>
typename std::enable_if<
    std::is_same<typename ViewType::value_type, Scalar>::value, Real>::type
frobenius_norm(const ViewType view) {
  typename ViewType::pointer_type data = view.data();

  size_t length = view.size();

  // Note: use Kahan algorithm to increase accuracy
  Real norm = 0;
  Real c = 0;
  Real temp, y;
  for (size_t i = 0; i < length; ++i) {
    for (int v = 0; v < VECTOR_SIZE; ++v) {
      y = data[i][v] * data[i][v] - c;
      temp = norm + y;
      c = (temp - norm) - y;
      norm = temp;
    }
  }

  return std::sqrt(norm);
}

template <typename rngAlg, typename PDF>
void genRandArray(Real *const x, int length, rngAlg &engine, PDF &&pdf) {
  for (int i = 0; i < length; ++i) {
    x[i] = pdf(engine);
  }
}

template <typename rngAlg, typename PDF>
void genRandArray(Scalar *const x, int length, rngAlg &engine, PDF &&pdf) {
  for (int i = 0; i < length; ++i) {
    for (int j = 0; j < VECTOR_SIZE; ++j) {
      x[i][j] = pdf(engine);
    }
  }
}

template <typename ViewType, typename rngAlg, typename PDF>
void genRandArray(ViewType view, rngAlg &engine, PDF &&pdf) {
  auto mirror = Kokkos::create_mirror_view(view);
  genRandArray(mirror.data(), mirror.size(), engine, pdf);
  Kokkos::deep_copy(view, mirror);
}

template <typename ViewType>
KOKKOS_INLINE_FUNCTION typename std::enable_if<Kokkos::is_view<ViewType>::value,
                                               void>::type
setArray(ViewType view, const typename ViewType::value_type &val) {
  auto mirror = Kokkos::create_mirror_view(view);
  setArray(view.data(), mirror.size(), val);
}

template <typename ValueType>
KOKKOS_INLINE_FUNCTION typename std::enable_if<
    !Kokkos::is_view<ValueType>::value, void>::type
setArray(ValueType *data, const int &length, const ValueType &val) {
  for (int i = 0; i < length; ++i) {
    data[i] = val;
  }
}

template <typename FPType>
Real compare_answers(FPType target, FPType computed,
                     FPType relative_coeff = 1.0) {
  Real denom = 1.0;
  if (relative_coeff > 0.0 && target != 0.0) {
    denom = relative_coeff * std::fabs(target);
  }

  return std::fabs(target - computed) / denom;
}

template <typename ExecSpace, typename Tag = void>
Kokkos::TeamPolicy<ExecSpace, Tag> get_default_team_policy(const int nelems) {
  const int threads_per_team =
      DefaultThreadsDistribution<ExecSpace>::threads_per_team(nelems);
  const int vectors_per_thread =
      DefaultThreadsDistribution<ExecSpace>::vectors_per_thread();
  return Kokkos::TeamPolicy<ExecSpace, Tag>(nelems, threads_per_team,
                                            vectors_per_thread);
}

// Source: https://stackoverflow.com/a/7185723
// Used for iterating over a range of integers
// With this, you can write
// for(int i : int_range(start, end))
template <typename ordered_iterable> class Loop_Range {

public:
  class iterator {
    friend class Loop_Range;

  public:
    KOKKOS_INLINE_FUNCTION
    ordered_iterable operator*() const { return i_; }

    KOKKOS_INLINE_FUNCTION
    const iterator &operator++() {
      ++i_;
      return *this;
    }

    KOKKOS_INLINE_FUNCTION
    iterator operator++(int) {
      iterator copy(*this);
      ++i_;
      return copy;
    }

    KOKKOS_INLINE_FUNCTION
    bool operator==(const iterator &other) const { return i_ == other.i_; }
    KOKKOS_INLINE_FUNCTION
    bool operator!=(const iterator &other) const { return i_ != other.i_; }

  protected:
    KOKKOS_INLINE_FUNCTION
    constexpr iterator(ordered_iterable start) : i_(start) {}

  private:
    ordered_iterable i_;
  };

  KOKKOS_INLINE_FUNCTION
  constexpr iterator begin() const { return begin_; }

  KOKKOS_INLINE_FUNCTION
  constexpr iterator end() const { return end_; }

  KOKKOS_INLINE_FUNCTION
  constexpr Loop_Range(ordered_iterable begin, ordered_iterable end)
      : begin_(begin), end_(end) {}

private:
  iterator begin_;
  iterator end_;
};

template <typename value_type> struct underlying_type {
  using type = value_type;
};

template <> struct underlying_type<Scalar> {
  using type = Real;
};

template <
    typename host_view,
    typename std::enable_if<
        // first verify we're actually working with a view
        Kokkos::is_view<host_view>::value &&
            // next verify the view is accessible from the host space
            host_view_mappable<host_view,
                               typename host_view::scalar_array_type>::value &&
                host_view::Rank == 1,
        int>::type * = nullptr>
class Host_View_Iterator {
public:
  static constexpr bool is_scalar_view =
      std::is_same<typename std::remove_all_extents<
                       typename host_view::scalar_array_type>::type,
                   Scalar>::value;

  using array_type = typename host_view::scalar_array_type;
  using storage_type = typename std::remove_all_extents<array_type>::type;

  using value_type = typename underlying_type<storage_type>::type;

  using pointer = value_type *;
  using reference = value_type &;

  using index_type = int;
  using difference_type = int;

  using iterator_category = std::random_access_iterator_tag;

  // Static Methods

  // templating here is a hack to not instantiate these functions until they are
  // needed
  template <typename _host_view = host_view>
  static constexpr typename std::enable_if<
      std::is_same<typename std::remove_all_extents<
                       typename _host_view::scalar_array_type>::type,
                   Scalar>::value == true,
      int>::type
  max_index(host_view view) {
    return view.extent_int(0) * VECTOR_SIZE;
  }

  template <typename _host_view = host_view>
  static constexpr typename std::enable_if<
      std::is_same<typename std::remove_all_extents<
                       typename _host_view::scalar_array_type>::type,
                   Scalar>::value == false,
      int>::type
  max_index(host_view view) {
    return view.extent_int(0);
  }

  static Host_View_Iterator start(const host_view view) {
    return Host_View_Iterator<host_view>(view, 0);
  }

  static Host_View_Iterator end(const host_view view) {
    return Host_View_Iterator<host_view>(view, max_index(view));
  }

  // Object Methods

  Host_View_Iterator(const host_view view, index_type index)
      : m_view(view), m_index(index) {
    assert(m_index <= max_index(m_view));
  }

  Host_View_Iterator(const Host_View_Iterator &other)
      : m_view(other.m_view), m_index(other.m_index) {}

  Host_View_Iterator &operator=(const Host_View_Iterator &other) {
    m_view = other.m_view;
    m_index = other.m_index;
    return *this;
  }

  Host_View_Iterator &operator++() {
    assert(m_index + 1 <= max_index(m_view));
    ++m_index;
    return *this;
  }

  Host_View_Iterator operator++(index_type) {
    assert(m_index + 1 <= max_index(m_view));
    Host_View_Iterator copy(*this);
    ++m_index;
    return copy;
  }

  Host_View_Iterator operator+(difference_type offset) {
    assert(m_index + offset <= max_index(m_view));
    Host_View_Iterator copy(*this);
    copy.m_index += offset;
    return copy;
  }

  Host_View_Iterator &operator--() {
    assert(m_index > 0);
    --m_index;
    return *this;
  }

  Host_View_Iterator operator-(difference_type distance) const {
    assert(m_index >= distance);
    assert(m_index + distance <= max_index(m_view));
    return Host_View_Iterator(m_view, m_index - distance);
  }

  difference_type operator-(const Host_View_Iterator &other) const {
    return m_index - other.m_index;
  }

  difference_type distance(const Host_View_Iterator &other) const {
    return (*this) - other;
  }

  // templating here is a hack to not instantiate these functions until they are
  // needed
  template <typename _host_view = host_view>
  typename std::enable_if<
      std::is_same<typename std::remove_all_extents<
                       typename _host_view::scalar_array_type>::type,
                   Scalar>::value == true,
      reference>::type
  operator*() const {
    assert(m_index >= 0);
    assert(m_index < max_index(m_view));
    return m_view(m_index / VECTOR_SIZE)[m_index % VECTOR_SIZE];
  }

  template <typename _host_view = host_view>
  typename std::enable_if<
      std::is_same<typename std::remove_all_extents<
                       typename _host_view::scalar_array_type>::type,
                   Scalar>::value == false,
      reference>::type
  operator*() const {
    return m_view(m_index);
  }

  pointer *operator->() const { return &(*(*this)); }

  bool operator==(const Host_View_Iterator &other) const {
    assert(other.m_view == m_view);
    return other.m_index == m_index;
  }

  bool operator!=(const Host_View_Iterator &other) {
    assert(other.m_view == m_view);
    return m_index != other.m_index;
  }

  bool operator<(const Host_View_Iterator &other) const {
    assert(other.m_view == m_view);
    return m_index < other.m_index;
  }

  bool operator<=(const Host_View_Iterator &other) const {
    assert(other.m_view == m_view);
    return m_index <= other.m_index;
  }

  bool operator>(const Host_View_Iterator &other) const {
    assert(other.m_view == m_view);
    return m_index > other.m_index;
  }

  bool operator>=(const Host_View_Iterator &other) const {
    assert(other.m_view == m_view);
    return m_index >= other.m_index;
  }

  // private:
  host_view m_view;
  index_type m_index;
};

} // namespace Homme

#endif // HOMMEXX_UTILITY_HPP
