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
