extends MarginContainer

signal guild_status_update

var guild_name

"To do:
	- add dynamic list that displays roughly ~10 guilds by default
	- add functionality to join guild by name
	- add ability for user to create their own guild, if the global
	maximum for guilds has not yet been met
		- when a user creates a guild, updates a counter tracking number
		of guilds
"

"
List of server side variables that need to be added:
- List of all Guilds currently created, and their current playercount
- Variable to store user's guild between sessions
- Variable to store user's status within their guild
- Tracker variable for number of guilds that currently exist total

Need to figure out:
	How results for a player search of a guild will work
"

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(guild_name):
	
	self.guild_name = guild_name

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

#Implement signial emission from child to parent that triggers switch to
#Guild scene again in MainMenu script to make entering guild work

func _on_test_join_guild_pressed():
	GameData.in_guild = true
	emit_signal("guild_status_update")
