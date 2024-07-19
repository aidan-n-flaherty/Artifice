extends Area3D

var mirror

var rot

var units

var selected

var color

# Called when the node enters the scene tree for the first time.
func _ready():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var camera = get_viewport().get_camera_3d()
	
	if camera:
		var scaleAmount = 1.0 + (camera.size - 240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y))/(240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y)) if camera.size > 240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y) else 1.0
		
		$Counter.scale = Vector3(1.4, 1.4, 1.4) * scaleAmount
		$Color.scale = Vector3(1.63, 0.7, 1.63) * scaleAmount
		$RotationInvariant.scale = Vector3(1.0, 1.0, 1.0) * scaleAmount
		$CollisionShape.scale = Vector3(1.0, 1.0, 1.0) * scaleAmount
		$Counter.position.y = scaleAmount
		$Color.position.y = scaleAmount
		
		var actualPos = get_parent().position + position
		var pos = Vector2(actualPos.x - camera.get_camera_transform().origin.x, actualPos.z + actualPos.y - (camera.get_camera_transform().origin.z + 200))
		pos.y /= sqrt(2)
		
		if pos.x < -camera.size/2 - 25 || pos.x > camera.size/2 + 25 || pos.y < -get_viewport().size.y * 1.0 / get_viewport().size.x * camera.size/2 - 25 || pos.y > get_viewport().size.y * 1.0 / get_viewport().size.x * camera.size/2 + 25:
			visible = false
			
			if get_node_or_null("Submarine"):
				var node = get_node("Submarine")
				node.queue_free()
				remove_child(node)
			
			return
		else:
			visible = true
				
			if not get_node_or_null("Submarine"):
				var node = preload("res://Submarine.tscn").instantiate()
				node.name = "Submarine"
				
				add_child(node)
				
				rot = null
				units = null
				selected = null
				color = null
			
			$Submarine.scale = Vector3(1.8, 1.4, 1.4) * (0.5 * scaleAmount + 0.5)
		
	if -get_rotation().y != rot: $RotationInvariant.rotation = Vector3(0, -get_rotation().y, 0)
	if units != get_parent().getUnits():
		$RotationInvariant/UnitsLabel.text = str(get_parent().getUnits())
	if selected != get_parent().isSelected():
		if get_parent().isSelected():
			$Color.get_surface_override_material(0).albedo_color = Color.WHITE
		else:
			color = null
	if color != get_parent().getColor():
		if not get_parent().isSelected():
			$Color.get_surface_override_material(0).albedo_color = get_parent().getColor()
		$Submarine.get_surface_override_material(0).albedo_color = get_parent().getColor()
		$Submarine.get_surface_override_material(1).albedo_color = get_parent().getColor().darkened(0.5)
			
		#$Submarine.get_surface_override_material(4).emission = get_parent().getColor()
	
	$RotationInvariant/Gift.visible = get_parent().isGift()
	
	rot = -get_rotation().y
	units = get_parent().getUnits()
	selected = get_parent().isSelected()
	color = get_parent().getColor()
	
	if selected:
		position.y = 0.8 * position.y + 2 * 0.2
	else:
		position.y = 0.8 * position.y
