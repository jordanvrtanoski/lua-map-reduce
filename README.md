# lua-map-reduce
Lua Map-Reduce library written in C

The library exports the following functions

* `map(table, function(key, value))` -> Executes the function on each node in the table, including the nested tables, and replaces the value of with the result of the function. The function returns the number of the visited nodes.
* `reduce(table, function(accumulator, key, value), initialValue)` -> Calles the reduce function on each node and stores the returning result in the accumulator. The accumulator starts from the initial value. The function returns the value of the accumultor at the end of the execution.
* `foreach(table, function(key, vale))` -> Executes the function on each node in the table, including nested tables. The funciton returns the number of the visited nodes.


## Example

Finding the value of the first 10 sqares

```
require "libluamapr"
a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
map(a,function(k,v) return v^2 end)
r = reduce(a,function(c,k,v) return c+v end, 0)
print("Sum of first 10 squares is", r)
```

