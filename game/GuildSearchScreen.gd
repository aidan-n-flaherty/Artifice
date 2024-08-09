extends MarginContainer

signal guild_status_update

var guild_name

"
To do:
	- add dynamic list that displays roughly ~10 guilds by default
	- add functionality to join guild by name
	- add ability for user to create their own guild, if the global
	maximum for guilds has not yet been met
		- when a user creates a guild, updates a counter tracking number
		of guilds
"

"
List of server side variables that need to be added:
- List of all Guilds currently created, their current playercounts, and the maximum number of players they should have
- Variable to store user's guild between sessions
- Variable to store user's status within their guild
- Tracker variable for number of guilds that currently exist total

Need to figure out:
	How results for a player search of a guild will work

Guild:
	GuildName (string)
	PlayerCount (int)
	Leaders (HashSet of IDs)

Within players:
	in_guild (True/False)
	guildID (Some positive int)

Store Guilds in HashTable of GuildIds and Guild Objects

Drafting json object for guild storage:

{
'guilds':[
	{'GuildId':10 , 'GuildName':'Test', 'PlayerCount':10, 'Leaders':[ 1, 2, 3 ] }
]
}

Add a button to refresh the list of guilds?
"

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(guild_name):
	
	self.guild_name = guild_name
	
	$MarginContainer/VBoxContainer/ListMargins/CreateGuildMenu.visible = false
	
	#Here is where the code for initializing the list of Guilds to join must be declared

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

#Implement signial emission from child to parent that triggers switch to
#Guild scene again in MainMenu script to make entering guild work

func _on_test_join_guild_pressed():
	GameData.in_guild = true
	emit_signal("guild_status_update")
	
func _create_guild():
	print("This is where the HTTPS call would be!")
	$MarginContainer/VBoxContainer/ListMargins/CreateGuildMenu.visible = true
	
func _on_exit_creat_guild_press():
	$MarginContainer/VBoxContainer/ListMargins/CreateGuildMenu.visible = false
	
