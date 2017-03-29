#pragma once

#include <PhotonTracer.h>

#ifdef PHOTON_WINDOWS
#include <Windows.h>
#endif

namespace Photon {

    namespace Utils {

        class Timer {
        public:
            Timer() {
#ifdef PHOTON_WINDOWS
                // get ticks per second
                QueryPerformanceFrequency(&_freq);

                // start timer
                QueryPerformanceCounter(&_start);
#endif
            }

            ~Timer() {
#ifdef PHOTON_WINDOWS
                // stop timer
                QueryPerformanceCounter(&_end);
#endif
            }

            void stop() {
#ifdef PHOTON_WINDOWS
                // stop timer
                QueryPerformanceCounter(&_end);
#endif
            }

            double elapsed() {
#ifdef PHOTON_WINDOWS
                return (_end.QuadPart - _start.QuadPart) * 1000.0 / _freq.QuadPart;
#endif
            }

        private:
#ifdef PHOTON_WINDOWS
            LARGE_INTEGER _freq;
            LARGE_INTEGER _start;
            LARGE_INTEGER _end;
#endif
        };

    }

}