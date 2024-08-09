extends MarginContainer

signal guild_status_update

var guild_name

var number_of_members

var max_members

@export_file("*.tscn") var MainMenu

var guild_leader = true
#test variable to make sure that the button for guild leader menu only appears when someone is a
#guild leader

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
	
	$MarginContainer/GuildInfoBox/ChatandLeaderMenu/LeaderMenu.visible = false
	
	self.guild_name = guild_name
	
	var margin_value = 50
	
	number_of_members = "3"
	max_members = "50"
	
	#Initialize the title box to display the retrieved guild name
	$MarginContainer/GuildInfoBox/TitleBox/UserGuild.text = guild_name
	#Setting the margins for the title box
	$MarginContainer/GuildInfoBox/TitleBox/UserGuild.add_theme_constant_override("margin_top", margin_value)
	$MarginContainer/GuildInfoBox/TitleBox/UserGuild.add_theme_constant_override("margin_left", margin_value)
	$MarginContainer/GuildInfoBox/TitleBox/UserGuild.add_theme_constant_override("margin_bottom", margin_value)
	$MarginContainer/GuildInfoBox/TitleBox/UserGuild.add_theme_constant_override("margin_right", margin_value)
	
	#Initialize the title box to display the current number of players in the guild
	$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/PlayercountMargins/Playercount.text = "Current Member Count: " + number_of_members + " / " + max_members
	#Setting the margins for the guild count box
	$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/PlayercountMargins/Playercount.add_theme_constant_override("margin_top", margin_value)
	$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/PlayercountMargins/Playercount.add_theme_constant_override("margin_left", margin_value)
	$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/PlayercountMargins/Playercount.add_theme_constant_override("margin_bottom", margin_value)
	$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/PlayercountMargins/Playercount.add_theme_constant_override("margin_right", margin_value)
	
	if(guild_leader == true):
		$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/OpenLeaderMenu.show()
	else:
		$MarginContainer/GuildInfoBox/PlayercountLeave/LeaveAndCount/OpenLeaderMenu.hide()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


#Implement signial emission from child to parent that triggers switch to
#Guild scene again in MainMenu script to make entering guild work


func _on_test_leave_guild_pressed():
	GameData.in_guild = false
	emit_signal("guild_status_update")


func _on_open_leader_menu_pressed():
	$MarginContainer/GuildInfoBox/ChatandLeaderMenu/LeaderMenu.visible = !$MarginContainer/GuildInfoBox/ChatandLeaderMenu/LeaderMenu.visible
