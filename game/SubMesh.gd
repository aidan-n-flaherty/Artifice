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
		var actualPos = get_parent().position + position
		var pos = camera.unproject_position(actualPos)
		if pos.x < -get_viewport().size.x * 0.2 || pos.x > get_viewport().size.x * 1.2 || pos.y < -get_viewport().size.y * 0.2 || pos.y > get_viewport().size.y * 1.2:
			visible = false
			return
		else:
			visible = true
		
	if -get_rotation().y != rot: $RotationInvariant.rotation = Vector3(0, -get_rotation().y, 0)
	if units != get_parent().getUnits():
		$RotationInvariant/Units.setValue(str(get_parent().getUnits()))
		$RotationInvariant/UnitsLabel.text = str(get_parent().getUnits())
	if selected != get_parent().isSelected():
		$RotationInvariant/Units.setSelection(get_parent().isSelected())
		if get_parent().isSelected():
			$Color.get_surface_override_material(0).albedo_color = Color.WHITE
		else:
			color = null
	if color != get_parent().getColor():
		$RotationInvariant/Units.setColor(get_parent().getColor())
		$Color.get_surface_override_material(0).albedo_color = get_parent().getColor()
	
	$RotationInvariant/Gift.visible = get_parent().isGift()
	
	rot = -get_rotation().y
	units = get_parent().getUnits()
	selected = get_parent().isSelected()
	color = get_parent().getColor()
	
	if selected:
		position.y = 0.9 * position.y + 2 * 0.1
	else:
		position.y = 0.9 * position.y
