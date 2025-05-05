local vector = require("vector")
local transform = require("transform")

local t = transform.new(
    vector.new(1, 2, 3),
    vector.new(0, 0, 0),
    vector.new(1, 1, 1)
)

print("Transform test:")
print("Position:", t.position.x, t.position.y, t.position.z)
print("Rotation:", t.rotation.x, t.rotation.y, t.rotation.z)
print("Scale:   ", t.scale.x, t.scale.y, t.scale.z)
