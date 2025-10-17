# libsensehst-cpp CHANGELOG

## October 2025 -- GPIO

The GPIO library API was changed alongside the migration of the Raspberry Pi OS
from Bookworm to Trixie.

This project code was initially developed using the
[libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git) v1.x API.
With the v2.2.x version now packaged in Debian Trixie, the GPIO functions have
been rewritten.

### New GPIO API design

According to the [libgpiod V2: New Major Release with a Ton of New
Features](https://lpc.events/event/16/contributions/1247/), Libgpiod v2.x has
been redesigned to improve usability and provide a flexible API and enhanced
platform support. The new version features a streamlined API that is less prone
to errors, supports GObject and D-Bus integration, and provides a modern model
for GPIO programming. Its internal structures have been reorganised to include
immutable snapshots and expanded language bindings.

The public interface now enables line requests to be grouped and line parameters
to be configured more precisely, including bias, drive and active low settings.
Requests can also be made across multiple lines at once.

### Changes to the code in `sensehat.cpp`

#### Data structures

On line 89 of the `sensehat.cpp` file is an array that stores the status of the
GPIO line request objects.

```cpp
static struct gpiod_chip *gpio_chip;
#define GPIOLIST 7
const uint8_t gpio_pinlist[GPIOLIST] = {5, 6, 16, 17, 22, 26, 27};
static struct gpiod_line_request *gpio_line[GPIOLIST];

/* Forward declaration of helper (defined later). */
static struct gpiod_line_request *_request_line(unsigned int pin,
                                                gpio_dir_t direction);
```

#### Functions

A first helper function, `_open_first_gpiochip()`, has been added to dynamically
detect and open the first available `/dev/gpiochipN`, rather than using a
hard-coded path.This is similar to what was done for the framebuffer device used
to drive the LED matrix.

A second helper function, `_request_line(pin, direction)`, has been added that
encapsulates the creation of `gpiod_line_settings` and `gpiod_line_config`, and
calls `gpiod_chip_request_lines()`.

Replacing read/write calls with `gpiod_line_request_get_value(request, offset)`
and `gpiod_line_request_set_value(request, offset, value)`.
The new API requires you to specify the row offsets. We then use these to read
or write values according to the defined pin number.

All the requests done with `gpiod_line_request` are systematically released via
the `gpiod_line_request_release()` call in the `senseShutdown()` function.

#### Implementation note

The added helpers are basic and are designed for simple reading and writing
operations that students will find easy to understand. They can be enhanced if
you require more comprehensive features, such as pull-up/down, debouncing and
edge detection.

## October 2025 -- PWM

### Simplified PWM Management with Persistent File Descriptors

The PWM implementation has been upgraded to use persistent file descriptors
rather than opening and closing sysfs files on each operation. This approach
provides significant operational and administrative benefits compared to the
manual setup documented in `docs/pwm.md`.

#### Old Approach Overhead

The previous documentation required:

- Creation and management of a custom systemd service (`pwm-export.service`)
- A shell script (`pwm-export.sh`) to export PWM channels at boot time

#### New Approach Benefits

The library now handles PWM channel management transparently:

- **Automatic initialization**: PWM channels are initialized during `senseInit()`
- **Persistent file descriptors**: Files are opened once and kept open (stored in
  `pwm_fd_period[2]`, `pwm_fd_duty[2]`, and `pwm_fd_enable[2]`)
- **Efficient operations**: Subsequent PWM calls avoid filesystem overhead by
  reusing open descriptors
- **Clean shutdown**: All file descriptors are properly closed in `senseShutdown()`

#### Implementation Details

Three pairs of persistent file descriptors are maintained per PWM channel:

```cpp
/* Persistent file descriptors for pwm sysfs files per channel.
 * Index 0 => pwm0, index 1 => pwm1. -1 means not opened. */
static int pwm_fd_period[2] = {-1, -1};
static int pwm_fd_duty[2] = {-1, -1};
static int pwm_fd_enable[2] = {-1, -1};
```

This design minimizes system calls and I/O overhead, making PWM control more
responsive.
