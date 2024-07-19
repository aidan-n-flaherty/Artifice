extends MarginContainer

signal guild_status_update

var guild_name

@export_file("*.tscn") var MainMenu

"
To do:
	-attach a chat log a la the in game chat log that stores user
	interactions on the server side
	-add guild hierarchy
	-add the ability for the owner of a guild to disband it
"

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(guild_name):
	
	self.guild_name = guild_name
	
	var margin_value = 50
	
	$MarginContainer/VBoxContainer/TitleBox/UserGuild.text = guild_name
	
	$MarginContainer/VBoxContainer/TitleBox/UserGuild.add_theme_constant_override("margin_top", margin_value)
	$MarginContainer/VBoxContainer/TitleBox/UserGuild.add_theme_constant_override("margin_left", margin_value)
	$MarginContainer/VBoxContainer/TitleBox/UserGuild.add_theme_constant_override("margin_bottom", margin_value)
	$MarginContainer/VBoxContainer/TitleBox/UserGuild.add_theme_constant_override("margin_right", margin_value)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


#Implement signial emission from child to parent that triggers switch to
#Guild scene again in MainMenu script to make entering guild work


func _on_test_leave_guild_pressed():
	GameData.in_guild = false
	emit_signal("guild_status_update")
