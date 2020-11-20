#pragma once
#include <cinttypes>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include <type_traits>
namespace ossia
{
#if defined(_MSC_VER) && defined(_DEBUG)
template <class T>
struct pod_allocator
{
  using value_type = T;
    
  template <typename... Args>
  explicit pod_allocator(Args&&...) noexcept
  {
  }

  pod_allocator() noexcept = default;
  pod_allocator(const pod_allocator&) noexcept = default;
  pod_allocator(pod_allocator&&) noexcept = default;
  pod_allocator& operator=(const pod_allocator&) noexcept = default;
  pod_allocator& operator=(pod_allocator&&) noexcept = default;

  static inline T* allocate(std::size_t num) noexcept
  {
    return new T[num];
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
    delete[] p;
  }

  friend inline bool operator==(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return false;
  }
};
#else
template <class T>
struct pod_allocator
{
  using value_type = T;

  pod_allocator() noexcept = default;
  pod_allocator(const pod_allocator&) noexcept = default;
  pod_allocator(pod_allocator&&) noexcept = default;
  pod_allocator& operator=(const pod_allocator&) noexcept = default;
  pod_allocator& operator=(pod_allocator&&) noexcept = default;

  static inline T* allocate(std::size_t num) noexcept
  {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>, "can only be used with POD types");
    static_assert(
        alignof(T) <= alignof(std::max_align_t),
        "type must not have specific alignment requirements");

    return (T*)std::malloc(sizeof(T) * num);
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
    std::free(p);
  }

  friend inline bool operator==(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return false;
  }
};
#endif
template <typename T>
using pod_vector = std::vector<T, pod_allocator<T>>;

using int_vector = pod_vector<int>;
using float_vector = pod_vector<float>;
using double_vector = pod_vector<double>;
}
