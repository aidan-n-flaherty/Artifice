extends Area3D

var units = -1

var shield = -1

var maxShield = -1

var selected = false

var color = null

var outpostName = null

var destroyed = null

var removed = false

var added = false

var viewportAdded = false

# Called when the node enters the scene tree for the first time.
func _ready():
	rot($Outpost)
	
func rot(node):
	node.get_node("City/Factory").rotation_degrees.y = 45 + 90 * get_parent().getID()
	node.get_node("City/Generator").rotation_degrees.y = 45 + 90 * get_parent().getID()
	
	node.get_node("JellyfishMesh").setID(get_parent().getID())

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var camera = get_viewport().get_camera_3d()
	
	if camera:
		var scaleAmount = 1.0 + (camera.size - 240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y))/(240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y)) if camera.size > 240 * min(1, get_viewport().size.x * 1.0 / get_viewport().size.y) else 1.0
		
		$RotationInvariant.scale = Vector3(1.0, 1.0, 1.0) * scaleAmount
		
		var actualPos = get_parent().position + position
		
		var pos = Vector2(actualPos.x - camera.get_camera_transform().origin.x, actualPos.z - (camera.get_camera_transform().origin.z + 200))
		pos.y /= sqrt(2)
		
		if pos.x < -camera.size/2 - 25 || pos.x > camera.size/2 + 25 || pos.y < -get_viewport().size.y * 1.0 / get_viewport().size.x * camera.size/2 - 25 || pos.y > get_viewport().size.y * 1.0 / get_viewport().size.x * camera.size/2 + 25:
			visible = false
			
			if get_node_or_null("Outpost"):
				var node = get_node("Outpost")
				node.queue_free()
				remove_child(node)
			
			return
		else:
			visible = true
			
			if not get_node_or_null("Outpost"):
				var node = preload("res://OutpostSubMesh.tscn").instantiate()
				destroyed = null
				node.name = "Outpost"
				rot(node)
				add_child(node)
	
	if units != get_parent().getUnits():
		$Units.text = str(get_parent().getUnits())
	if shield != get_parent().getShield() or maxShield != get_parent().getMaxShield():
		$Shield.text = str(get_parent().getShield())
		if get_node_or_null("SubViewport"):
			get_node("SubViewport/OutpostInfo").setShield(get_parent().getShield(), get_parent().getMaxShield())
		else:
			get_node("SubViewportTemp/OutpostInfo").setShield(get_parent().getShield(), get_parent().getMaxShield())
	if outpostName != get_parent().getName():
		$Name.text = get_parent().getName()
	if (get_parent().canViewType() or get_parent().isInRadar()) and destroyed != get_parent().isBroken():
		$Outpost/JellyfishMesh.setDark(get_parent().isBroken())
	if selected != get_parent().isSelected():
		if get_parent().isSelected():
			$FlagSprite.modulate = get_parent().getColor().lightened(0.5)
		else:
			color = null
	
	if selected:
		$Outpost.position.y = 0.8 * $Outpost.position.y + 2 * 0.2
	else:
		$Outpost.position.y = 0.8 * $Outpost.position.y
	
	if color != get_parent().getColor():
		$FlagSprite.modulate = get_parent().getColor()
	
	$FlagSprite/FlagSprite2.visible = get_parent().getSelfOwned()
	
	units = get_parent().getUnits()
	shield = get_parent().getShield()
	maxShield = get_parent().getMaxShield()
	selected = get_parent().isSelected()
	color = get_parent().getColor()
	outpostName = get_parent().getName()
	
	if get_parent().canViewType() or get_parent().isInRadar():
		destroyed = get_parent().isBroken()
	
	$Outpost/JellyfishMesh.showLights(not get_parent().isInRadar() and not get_parent().canViewType())
	
	if get_parent().canViewType() or get_parent().isInRadar():
		$RotationInvariant.show()
		$Shadow.show()
		if not get_parent().isBroken():
			$Outpost/City.show()
			$Outpost/ring.show()
			$Outpost/floor.show()
		else:
			$Outpost/City.hide()
			$Outpost/ring.hide()
			$Outpost/floor.hide()
	else:
		$Shadow.hide()
		$RotationInvariant.hide()
		$Outpost/City.hide()
		$Outpost/ring.hide()
		$Outpost/floor.hide()
	
	if not get_parent().isInRadar():
		$FloorSprite.hide()
		$Shield.hide()
		$Units.hide()
	else:
		if not viewportAdded:
			addViewport()
		
		$FloorSprite.show()
		$Shield.show()
		$Units.show()
	
	$Outpost/City/Factory.visible = get_parent().isFactory()
	
	$Outpost/City/Generator.visible = get_parent().isGenerator()
	
	$Outpost/City/Mine.visible = get_parent().isMine()
	
func addViewport():
	if not get_node_or_null("SubViewport"):
		viewportAdded = true
		var outpostInfo = get_node("SubViewportTemp/OutpostInfo")
		$SubViewportTemp.remove_child(outpostInfo)
		
		var viewport = SubViewport.new()
		viewport.size = Vector2i(200, 200)
		viewport.name = "SubViewport"
		viewport.transparent_bg = true
		viewport.msaa_2d = Viewport.MSAA_4X
		viewport.disable_3d = true
		viewport.add_child(outpostInfo)
		add_child(viewport)
		
		$FloorSprite.texture = viewport.get_texture()
		viewport.render_target_update_mode = SubViewport.UPDATE_ONCE

func _exit_tree():
	if get_node_or_null("SubViewport"):
		viewportAdded = false
		var outpostInfo = get_node("SubViewport/OutpostInfo")
		outpostInfo.set_owner(null)
		get_node("SubViewport").remove_child(outpostInfo)
		$SubViewportTemp.add_child(outpostInfo)
		get_node("SubViewport").queue_free()
