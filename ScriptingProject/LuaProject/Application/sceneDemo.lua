local health = 100

for _ = 1, 100 do
	local entity = scene.CreateEntity()
	local tickDamage = math.random(5, 20)
	scene.SetComponent(entity, "health", health)
	scene.SetComponent(entity, "poison", tickDamage)
end