dino = {
    species = "Tyrannosaurus rex",
    name = "T-Rex-Kalle",
    wings = false,

    getnoise = function(size)
        local noise = "R"
        for _ = 1, size do
            noise = noise .. "o"
        end
        noise = noise .. "aar!"
        return noise
    end
}
