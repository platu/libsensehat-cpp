# Finite State Machine introduction to simulate and run a robot

This folder contains a set of example programs which provide a step by step
introduction to Finiet State Machine (FSM) C++ coding.

## Non blocking keyboard event detection

As we want to simulate FSM, we need a way to code an event driven program. The
very starting point is to detect key pressed on a keyboard.

Here is a first example code : [00_non-blocking-kb.cpp](./00_non-blocking-kb.cpp)

This program uses the `keypressed()` function as an event detector.
If nothing happens in the main task, the task is just an empty loop.

The `keypressed()` function returns an integer value which is the count in bytes
of ASCII characters in the keyboard buffer. If the value is greater than 0, it
means that a key has been pressed. This is our **event**.

In the example code, we check if the value is equal to 1. If it is the case, the
`cin.get()` function is used to read the key from the keyboard buffer. Then the
keyboard character is displayed on the console.

Whenever the `q` key is pressed, the program exits from the main task and quits.

There is a second example code :
[00_non-blocking-kb+js.cpp](./00_non-blocking-kb+js.cpp)

Here, another kind of event is detected : the joystick button pressed event.

This stands for the emergency stop button of the robot. When the button is
pressed, the porgram exits from the main task and quits.

## First two states FSM

The next step is to code a minimalistic FSM. There we start simulating a
robot with two states : `STOP` and `RUN`.

The robot is supposed to be stopped at the beginning.
When the `r` key is pressed, the robot starts running. 
When the `s` key is pressed, the robot stops.
When the `q` key is pressed, the program exits from the main task and quits.

Here is the code : [01_two-states-robot.cpp](./01_two-states-robot.cpp)

Another example code is provided :
[01_two-states-robot+js-emergency-stop.cpp](./01_two-states-robot+js-emergeny-stop.cpp)

This code adds functions for state machine evolution and actions.

Within the main loop, events are detected and encoded.

If an event has occured, the state machine evolution function `evolve()` is
called. Finally, the action function is called if the state has changed.