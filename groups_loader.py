import xml.etree.ElementTree as ET
import os

problem = "problem.xml"

def checkfile(file):
    if not os.path.isfile(file):
        print(f'{file} not found')
        return False
    return True

if not checkfile("task.cfg"):
    exit(1)
if not checkfile(problem):
    problem += ".polygon"
if not checkfile(problem):
    print("error reading xml")
    exit(1)

print(f'reading {problem}')

tree = ET.parse(problem)
tests = tree.getroot().findall('.//testset[@name=\'tests\']/tests')[0]

dic = {}

def add(group, points):
    if not dic.get(group):
        dic[group] = [float(0), 0]
    dic[group][0] += float(points)
    dic[group][1] += 1

for i in tests:
    add(i.attrib['group'], i.attrib['points'])

total = float(0)
for group, (points, tests) in dic.items():
    print(f'group #{group}:\t{points} points\t{tests} tests')
    total += points
print(f'total points: {total}')

if os.path.isfile('task.cfg.old'):
    print("found old task.cfg, copying skipped")
else:
    print("making a copy of task.cfg")
    os.system("copy task.cfg task.cfg.old")

lines = []
lines_end = []
skip_line = False
after_end = False
for line in open("task.cfg"):
    if not skip_line:
        lines.append(line)
    if line.lower().startswith("tests_begin"):
        skip_line = True
    if line.lower().startswith("tests_end"):
        after_end = True
    if after_end:
        lines_end.append(line)

if not lines[-2].startswith("NEW_GROUP"):
    lines.insert(-1, "NEW_GROUP = 1\n")

i = 0
while dic.get(str(i)):
    points, tests = dic[str(i)]
    for j in range(tests - 1):
        lines.append("-1\n")
    lines.append(str(int(round(points))) + "\n")
    i += 1

lines += lines_end

print("rewriting task.cfg")

with open("task.cfg", 'w') as f:
    for line in lines:
        f.write(line)
