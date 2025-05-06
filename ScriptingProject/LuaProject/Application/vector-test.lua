local vector = require("vector")

local a = vector.new(1, 2, 3)
local b = { x = 1, y = 2, z = 3 }

print("a is vector:", vector.isvector(a))
print("b is vector:", vector.isvector(b))
