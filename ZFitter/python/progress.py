import sys

def getWidth(width=None):
	if not width:
		import subprocess
		rows, columns = subprocess.check_output(['stty', 'size']).split()
		width = int(columns)
	return width

def start(title,width=None):
	global progress_x,progress_width
	term_width = getWidth(width)
	progress_width = term_width - len(title) - 4
	if sys.stdout.isatty():
		sys.stdout.write(title + ": [" + "-"*progress_width + "]" + chr(8)*(progress_width +1))
		sys.stdout.flush()
		progress_x = 0

def update(x):
	global progress_x,progress_width
	if sys.stdout.isatty():
		 newprog = int(x * progress_width // 100)
		 if(newprog - progress_x):
		 	sys.stdout.write("#" * (newprog - progress_x))
		 	sys.stdout.write("-{n:->3}%".format(n=int(x)))
		 	sys.stdout.write(chr(8)*5)
		 	sys.stdout.flush()
		 progress_x = newprog

def end():
	global progress_x,progress_width
	if sys.stdout.isatty():
		 sys.stdout.write("#" * (progress_width - progress_x) + "]\n")
		 sys.stdout.flush()

