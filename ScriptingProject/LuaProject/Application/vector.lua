local vector = {}

function vector.__newindex(t, k, v)
    print("Vector - not possible to assign new fields")
end

function vector.new(x, y, z)
    local t = {
        x = x or 0,
        y = y or 0,
        z = z or 0
    }
    setmetatable(t, vector)

    return t
end

function vector.isvector(t)
    local mt = getmetatable(t)
    return mt and mt.__index == vector
end

function vector.__unm(t)
    return vector.new(-t.x, -t.y, -t.z)

end

function vector.__add(a, b)
    return vector.new(a.x + b.x, a.y + b.y, a.z + b.z)
end

function vector.__sub(a, b)
    return vector.new(a.x - b.x, a.y - b.y, a.z - b.z)
end

function vector.__mul(a, b)
    if type(a) == "number" then
        return vector.new(a * b.x, a * b.y, a * b.z)
    elseif type(b) == "number" then
    return vector.new(a.x * b, a.y * b, a.z * b)
    elseif vector.isvector(a) and vector.isvector(b) then
        return vector.new(a.x * b.x, a.y * b.y, a.z * b.z)
    end

end

function vector.__div(a, b)
    if type(b) == "number" then
        return vector.new(a.x / b, a.y / b, a.z / b)
    elseif vector.isvector(a) and vector.isvector(b) then
        return vector.new(a.x / b.x, a.y / b.y, a.z / b.z)
    end
end

function vector.__tostring(t)
    return string.format("(%g, %g, %g)", t.x, t.y, t.z)
end

function vector.__eq(a, b)
    if vector.isvector(a) and vector.isvector(b) then
        return a.x == b.x and a.y == b.y and a.z == b.z
    end
    return false

end


vector.__index = vector

return vector
