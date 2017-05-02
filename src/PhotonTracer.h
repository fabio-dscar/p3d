#pragma once

//#define PHOTON_DEBUG 0

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#if defined(_WIN32) || defined(_WIN64)
#define PHOTON_WINDOWS
#elif defined(__linux__)
#define PHOTON_LINUX
#endif

// Global includes
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#define PHOTON_MSVC
#include <float.h>
#include <intrin.h>
#pragma warning(disable : 4244)  // int to float warning
#pragma warning(disable : 4843)  // double to float warning
#pragma warning(disable : 4305)  // double const to float
#endif

#if defined(_MSC_VER) && _MSC_VER == 1900
#define PHOTON_MSVC2015
#endif

#if defined(_MSC_VER) && _MSC_VER == 1800
#define PHOTON_MSVC2013
#endif

#ifdef PHOTON_MSVC2013
#define PHOTON_CONSTEXPR const
#else
#define PHOTON_CONSTEXPR constexpr
#endif