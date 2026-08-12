# Parking Lot OOP

A vehicle parking lot system, implemented in C++ while learning OOP and trying to get more comfortable with designing classes properly and writing clean, modular code.

### What it does

* Park Cars and Bikes
* Find and vacate parking spots
* Calculate parking fees based on parking time
* Show the current parking layout

### Stuff I'm using

* C++
* OOP (inheritance, polymorphism, composition)
* `unique_ptr` / RAII
* Strategy Pattern for pricing
* `chrono` for parking duration

### Current structure

```text
Parking
   |
   +-- Vehicle
   |    +-- Car
   |    +-- Bike
   |
   +-- PricingStrategy
        +-- HourlyPricing
```

I'm building this incrementally, so the design will probably change quite a bit as I learn more about OOP and system design.

### Next targets

* Parking tickets
* `ParkingSpot` class
* Better spot management
* More pricing strategies
* Cleaner project structure
