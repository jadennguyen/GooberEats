# GooberEats
A turn-by-turn navigation system that uses Open Street Map geospatial data and produce relatively optimized delivery instructions


# Description
### The input to the delivery logistics system: 
* Open Street Map data
* The latitude and longitude of the central meal prep depot
* The latitude and longitude of one or more delivery stops (always on street corners), along with what food item must be delivered at each such street corner

### The output of the delivery logistics system:
* A set of driving directions from the central depot, where the food is picked up, to each of the delivery stops, then back to the central depot



# Design
‣ Developed using C++, exhibiting data structures such as the creation of an open hash map to store geospatial data

‣ Implemented Simulated Annealing to optimize an input set of delivery coordinates and A* search algorithm to produce a point-by-point routing system

# Screenshots
### Map Data
<img src="https://imgur.com/H5qx43X.png" width="75%" height="75%">

### Delivery Locations
<img src="https://imgur.com/V1EyNLo.png" width="75%" height="75%">

### Navigation Directions
<img src="https://imgur.com/ZwzxMLo.png" width="75%" height="75%">
