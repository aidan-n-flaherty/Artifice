extends Area3D

var units = -1

var shield = -1

var selected = false

var color

var outpostName

var removed = false

var added = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if units != get_parent().getUnits():
		$Units.text = str(get_parent().getUnits())
	if shield != get_parent().getShield():
		$Shield.text = str(get_parent().getShield())
		$SubViewport/OutpostInfo.setShield(get_parent().getShield(), get_parent().getMaxShield())
	if outpostName != get_parent().getName():
		$Name.text = get_parent().getName()
	if selected != get_parent().isSelected():
		if get_parent().isSelected():
			$FlagSprite.modulate = Color.WHITE
		else:
			color = null
	if color != get_parent().getColor():
		$FlagSprite.modulate = get_parent().getColor()
	
	units = get_parent().getUnits()
	shield = get_parent().getShield()
	selected = get_parent().isSelected()
	color = get_parent().getColor()
	outpostName = get_parent().getName()
	if get_parent().isFactory():
		$City/factories.show()
		$City/generators.hide()
	elif get_parent().isGenerator():
		$City/generators.show()
		$City/factories.hide()
	elif get_parent().isMine():
		pass
	else:
		$City/generators.hide()
		$City/factories.hide()
		$shield.hide()
		$City/buildings.hide()

func _enter_tree():
	if not get_node_or_null("SubViewport"):
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
		var outpostInfo = get_node("SubViewport/OutpostInfo")
		outpostInfo.set_owner(null)
		get_node("SubViewport").remove_child(outpostInfo)
		$SubViewportTemp.add_child(outpostInfo)
		get_node("SubViewport").queue_free()
