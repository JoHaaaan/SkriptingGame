local transform = {}

local vector = require("vector")

transform.__index = transform

function transform.istransform(t)
    local mt = getmetatable(t)
    return mt and mt.__index == transform
end

function transform.__newindex(t, k, v)
    print("transform – not possible to assign new fields")
end


function transform.__tostring(t)
    return string.format(
        "[Transform]\n  Position: %s\n  Rotation: %s\n  Scale:    %s",
        tostring(t.position), tostring(t.rotation), tostring(t.scale)
    )
end


function transform.__eq(a, b)
    return a.position == b.position
       and a.rotation == b.rotation
       and a.scale == b.scale
end

-- Konstruktor
local function new(p, r, s)
    assert(p == nil or vector.isvector(p), "transform new - expected args: vector or nil")

    local t = {
        position = p or vector(),
        rotation = r or vector(),
        scale    = s or vector()
    }

    return setmetatable(t, transform)
end
setmetatable(transform, {
    __call = function(_, p, r, s)
        return transform.new(p, r, s)
    end
})

transform.new = new
return transform
