# Smart Cache Management and Replacement Simulator

This is our DSA practical project, done as a team of three in C++. It is a command line program that pretends to be a small cache. You give it a cache size and a list of requests, and it runs the requests through four replacement policies (LRU, FIFO, LFU and Optimal) and shows how each one did.

## The problem

A cache can only hold a few items. When it is full and a new item is requested, one of the old items has to go. The replacement policy is the rule that decides which one.

If the requested item is already in the cache it is a hit. If not, it is a miss. A good policy gets more hits.

## The four policies

- LRU (least recently used): throws out the item that has not been used for the longest time.
- FIFO (first in, first out): throws out the item that came into the cache first. Using an item again does not change its place.
- LFU (least frequently used): throws out the item that has been used the fewest times. If two items have the same count, the older one goes.
- Optimal: throws out the item that will be needed farthest in the future, or never again. It has to know all the requests in advance, so no real system can use it. We added it only to show the best result possible, so we can see how close the other three get.

## Data structures used

- LRU: a hash map and a doubly linked list. The most recently used item is at the front and the least recently used is at the back. The hash map finds the node directly, so moving it to the front on a hit takes O(1).
- FIFO: a hash map and a doubly linked list used as a queue. New items go in at the front, the oldest is removed from the back, and a hit changes nothing.
- LFU: two hash maps. One goes from a key to its node. The other goes from a use count to a doubly linked list of all the items with that count. We also keep track of the smallest count so we know which list to remove from.
- Optimal: a hash map for the cache, and a hash map that gives each item a queue of the positions where it will be requested later.

LRU, FIFO and LFU handle each request in O(1). Optimal looks at every item in the cache when there is a miss, so it is O(capacity) per miss.

## Folders

```
cache-simulator/
  FIFO-LFU/
    FIFO.h
    LFU.h
    test_fifo.cpp
    test_lfu.cpp
  LRU-Optimal/
    LRU.h
    optimal.h
    test_lru.cpp
    test_optimal.cpp
  simulator/
    Simulator.h
  main.cpp
  tests.cpp
  README.md
```

Each policy is its own class with the same methods, so they can be used the same way. After a run you can read the results with getHits(), getMisses(), getRemovals(), getHitRatio() and getMissRatio().

Simulator.h runs all four policies on the same input and gives back the numbers for each. main.cpp is the menu program, and tests.cpp has the automatic tests.

## Building and running

You need g++ (any recent version). Run these from the main folder:

```
g++ main.cpp -o cache_sim
./cache_sim
```

On Windows use `cache_sim.exe` instead of `./cache_sim`.

Please do not call the output file "simulator". There is already a folder with that name and the compiler will give an error.

The menu looks like this:

```
1. Enter capacity and requests
2. Load sample input (capacity 3)
3. Show LRU step by step
4. Show FIFO step by step
5. Show LFU step by step
6. Show Optimal step by step
7. Compare all policies
8. Compare for capacities 1 to 5
0. Exit
```

For option 1, type the cache capacity, then the number of requests, then the requests themselves. Requests have to be whole numbers, 0 or bigger.

## Example

Option 2 loads capacity 3 with the requests 1 2 3 1 4 1 5 1 2 3. Option 7 then gives:

```
Policy   Hits    Misses  Hit ratio   Miss ratio  Replacements
LRU      3       7       30.00%      70.00%      4
FIFO     2       8       20.00%      80.00%      5
LFU      3       7       30.00%      70.00%      4
Optimal  4       6       40.00%      60.00%      3
```

Here is LRU step by step with capacity 3 and the requests 1 2 3 1 4:

```
1 MISS removed: - | cache: 1
2 MISS removed: - | cache: 1 2
3 MISS removed: - | cache: 1 2 3
1 HIT  removed: - | cache: 2 3 1
4 MISS removed: 2 | cache: 3 1 4
```

The request for 4 removed 2 because 1 had just been used. FIFO would have removed 1 at that point, since 1 came in first.

In the cache line, the first item is the next one that would be removed. In LFU the number in brackets is how many times the item was used.

## Tests

```
g++ tests.cpp -o tests_run
./tests_run
```

It prints PASS or FAIL for every test and a count at the end. It checks some examples we worked out by hand, edge cases (capacity 0, capacity 1, no requests), and 500 random inputs that are compared against simple slow versions of each policy.

Each folder also has small test files for single policies, for example:

```
cd FIFO-LFU
g++ test_fifo.cpp -o test_fifo
./test_fifo
```

These read the capacity, the number of requests and the requests from the keyboard, show the steps, and then print a short result for capacities 1 to 4. Every file with a main() has to be compiled on its own.

## Things to know

- Items are whole numbers only.
- Optimal needs the whole request list before it starts, so it is only for comparing.
- When LFU has a tie, the item that has been in the cache longer is removed first.

## Team

- Geetisha Jain - 
- Khushi Yadav - 
- Krisha Ahuja -
