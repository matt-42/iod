#pragma once

#include <chrono>


namespace vpp
{

  class timer
  {
  public:
    void start() { start_ = std::chrono::high_resolution_clock::now(); }
    void end() { end_ = std::chrono::high_resolution_clock::now(); }


    unsigned long us() const {
      return std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_).count();
    }

    unsigned long ms() const {
      return std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
    }

    unsigned long ns() const {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - start_).count();
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_, end_;
  };

}
