require "libluamapr"

error=0

a = {
    b = "c",
    d = {
        e = "f",
        g = "h"    
    },
    e = {
        a = {
            e = "f",
            m = "f"
        },
        b = "another c"
    }
}

function prtest(key, value)
	if (key == "b") then
		print("B has ",value)
	end
end


function tomap(key, value)
	if (key == "g") then
		print("replace value of g")
		return "replaced"
	else
		return value
	end
end


print("=== Test #1 map: Replace value of a.d.g ===")

print("--- before ---")
preattyprint(a)

print("--- Calling map ---")
b = map(a, tomap)
print(b)

print("--- Calling foreach ---")
preattyprint(a)
print("-----------------------")
if a.d.g == "replaced" then print("PASSED") else print("FAILED") error=error+1 end
print("=======================")


print("=== Test #1 reduce: Calculate the 2^n (n = number of nodes) with inline funtion ===")
r=reduce(a, function(c,k,b) return c+c end, 1)
print("2^n = ",r)
print("-----------------------")
if r == 64 then print("PASSED") else print("FAILED") error=error+1 end
print("=======================")

print("=== Test #2 reduce: Calculate the number of nodes * 2 with external function ===")
function red(ac, k, v)
	return ac+2
end
r=reduce(a,red,0)
print("2*n = ",r)
print("-----------------------")
if r == 12 then print("PASSED") else print("FAILED") error=error+1 end
print("=======================")



print("=== Test #1 map/reduce: Find sum of first 10 squares ===")
a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
map(a,function(k,v) return v^2 end)
r = reduce(a,function(c,k,v) return c+v end, 0)
print("Sum of first 10 squares is", r)
print("--------------------------")
if r == 385 then print("PASSED") else print("FAILED") error=error+1 end
print("==========================")

os.exit(error)
