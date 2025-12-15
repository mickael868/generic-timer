# Generic Timer Service

A lightweight, thread-safe timer service for C++14 that allows scheduling callbacks with arbitrary arguments. The service uses a single worker thread to manage multiple timers efficiently.

## Key Feature: Automatic Argument Management

**No need to preserve arguments until timer expiration!** The TimerService automatically copies and manages all callback arguments internally. This eliminates common timer pitfalls where arguments go out of scope or are destroyed before the timer fires.

```cpp
TimerService timer_service;

// Safe: argument is copied and managed by the timer
std::string message = "Hello!";
auto timer_id = timer_service.schedule(
    std::chrono::milliseconds(1000),
    [](const std::string& msg) {
        std::cout << msg << std::endl;  // Always safe to access
    },
    message
);
// message can be destroyed here - timer has its own copy
message.clear();  // Safe!
```

## Features

- **Generic Callbacks**: Schedule timers with callbacks that accept any type of argument
- **Thread-Safe**: Safe to use from multiple threads
- **Single Worker Thread**: Efficient resource usage with one background thread
- **Timer Cancellation**: Cancel scheduled timers by ID
- **Automatic Argument Management**: Arguments are copied and stored within timers, relieving users from lifetime management concerns

## Requirements

- C++14 compatible compiler
- CMake 3.15 or later

## Building

1. Clone the repository:
   ```bash
   git clone https://github.com/rifatsahiner/generic-timer.git
   cd generic-timer
   ```

2. Create a build directory and configure:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:
   ```bash
   make
   ```

## Usage

Include the timer service header in your code:

```cpp
#include "timer_service/timer_service.h"
```

### Basic Example

```cpp
#include "timer_service/timer_service.h"
#include <iostream>

int main() {
    TimerService timer_service;

    // Schedule a timer that fires after 1 second
    auto timer_id = timer_service.schedule(
        std::chrono::milliseconds(1000),
        [](int value) {
            std::cout << "Timer fired with value: " << value << std::endl;
        },
        42
    );

    // Wait for timer to fire
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    return 0;
}
```

### Advanced Examples

#### Scheduling with Different Argument Types

```cpp
TimerService timer_service;

// String argument
timer_service.schedule(
    std::chrono::milliseconds(500),
    [](const std::string& msg) {
        std::cout << "Message: " << msg << std::endl;
    },
    "Hello, World!"
);

// Custom struct
struct Point {
    int x, y;
};

timer_service.schedule(
    std::chrono::milliseconds(1000),
    [](Point p) {
        std::cout << "Point: (" << p.x << ", " << p.y << ")" << std::endl;
    },
    Point{10, 20}
);
```

#### Member Function Callbacks

```cpp
class MessageHandler {
public:
    void onTimeout(const std::string& message) {
        std::cout << "Received: " << message << std::endl;
    }
};

TimerService timer_service;
MessageHandler handler;

// Using std::bind
timer_service.schedule(
    std::chrono::milliseconds(2000),
    std::bind(&MessageHandler::onTimeout, &handler, std::placeholders::_1),
    "Timeout via bind"
);

// Using lambda capture
timer_service.schedule(
    std::chrono::milliseconds(2500),
    [&handler](const std::string& msg) {
        handler.onTimeout(msg);
    },
    "Timeout via lambda"
);
```

#### Timer Cancellation

```cpp
TimerService timer_service;

// Schedule a timer
auto timer_id = timer_service.schedule(
    std::chrono::milliseconds(1000),
    [](int value) {
        std::cout << "This won't print!" << std::endl;
    },
    999
);

// Cancel it before it fires
std::this_thread::sleep_for(std::chrono::milliseconds(500));
bool cancelled = timer_service.cancel(timer_id);
std::cout << "Timer cancelled: " << (cancelled ? "Yes" : "No") << std::endl;
```

## Running the Demo

After building, run the included demo:

```bash
./generic-timer
```

This will demonstrate various timer scheduling scenarios including different argument types, cancellation, and member function callbacks.

## TODO / Future Enhancements

- **Periodic Timers**: Add functionality for recurring timers that fire at regular intervals
  - `schedulePeriodic()` method that reschedules itself after each execution
  - Option to specify maximum number of repetitions or run indefinitely
  - Return a timer ID that can cancel the entire periodic sequence

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.