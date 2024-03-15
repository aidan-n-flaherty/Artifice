extends Node


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func numToStr(num: int, unit: String):
	return str(num) + " " + unit + "s" if num != 1 else str(num) + " " + unit

func timeToStr(time: int):
	if time < 0: return timeToStr(abs(time)) + " ago"
	if time > 365 * 24 * 60 * 60: return numToStr(int(time / (365 * 24 * 60 * 60)), "year")
	if time > 4 * 7 * 24 * 60 * 60: return numToStr(int(time / (4 * 7 * 24 * 60 * 60)), "month")
	if time > 7 * 24 * 60 * 60: return numToStr(int(time / (7 * 24 * 60 * 60)), "week")
	if time > 24 * 60 * 60: return numToStr(int(time / (24 * 60 * 60)), "day")
	if time > 60 * 60: return numToStr(int(time / (60 * 60)), "hour")
	if time > 60: return numToStr(int(time / 60), "minute")
	return numToStr(time, "second")

func timeToDateStr(t: int):
	var time = Time.get_datetime_dict_from_unix_time(t + 60 * Time.get_time_zone_from_system().bias)
	var current = Time.get_datetime_dict_from_system()
	
	var subtime = "%d:%02d %s" % [(time.hour + 11) % 12 + 1, time.minute, "AM" if time.hour < 12 else "PM"]
	
	var day = "%d%s" % [time.day, "st" if time.day % 10 == 1 else "nd" if time.day % 10  == 2 else "rd" if time.day % 10 == 3 else "th"]
	if current.year == time.year and current.month == time.month and current.day == time.day:
		return "Today at " + subtime
	
	if current.year == time.year and current.day < time.day + 7:
		return "%s at %s" % [["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"][time.weekday], subtime]
	
	return "%s %s, %d, at %s" % [["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"][time.month - 1], day, time.year, subtime]
