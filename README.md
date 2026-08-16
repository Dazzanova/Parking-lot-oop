# Parking Lot OOP

A small vehicle parking lot system written in C++ while learning OOP, smart pointers, design patterns, and better class design.

### What it does

- Park Cars and Bikes
- Find and vacate parking spots
- Calculate parking fees based on parking time
- Show the current parking layout
- Manage vehicle ownership using `unique_ptr`

### Stuff I'm using

- C++
- OOP (inheritance, polymorphism, composition)
- `unique_ptr` / RAII
- Strategy Pattern for pricing
- `chrono` for parking duration

### Current structure

```text
Parking
   |
   +-- ParkingSpot
   |      |
   |      +-- Vehicle
   |           +-- Car
   |           +-- Bike
   |
   +-- PricingStrategy
          +-- HourlyPricing
