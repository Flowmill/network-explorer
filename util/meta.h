/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <type_traits>

#include <cstdint>

namespace meta {

/**
 * A type tag that can be cheaply instantiated and passed by value.
 *
 * It allows passing type information by value when it is unknown if the type
 * itself is cheaply constructed or passed by value.
 *
 * This is a helper for meta-function implementations like `meta::foreach`.
 *
 * For a real-world example of usage, check `meta::foreach`.
 */
template <typename T> struct tag {
  using type = T;
};

/**
 * A helper to retrieve the type of a type tag.
 *
 * This greatly helps combining lambdas with meta-functions.
 *
 * For a real-world example of usage, check `meta::foreach`.
 */
template <typename T> typename T::type tag_type(T);

/**
 * A minimalistic type-list used to represent a list of types at compile time.
 */
template <typename... T> struct list {
  /**
   * The size of the list.
   */
  static constexpr std::size_t size = sizeof...(T);
};

} // namespace meta

#include <util/meta.inl>

namespace meta {

/**
 * Iterates over the elements of a type list, calling the given function for each one.
 *
 * The given function must have the following signature:
 *
 *  template <typename T>
 *  void fn(meta::tag<T>, Args &...args);
 *
 * All arguments given to foreach will be passed to the given function on
 * every call so beware of double-moves.
 *
 * Example:
 *
 *  using list = meta::list<std::string, int, double>;
 *
 *  meta::foreach<list>([](auto tag) {
 *    using type = decltype(meta::tag_type(tag));
 *    std::cout << "enter a value for a " << typeid(type).name() << ": ";
 *    type value;
 *    std::cin >> value;
 *    std::cout << "\nvalue given: " << value << std::endl;
 *  });
 */
template <typename List, typename Fn, typename... Args> void foreach (Fn &&fn, Args && ... args);

} // namespace meta
