#!/usr/bin/env python3

assert tuple(map(int, __import__("platform").python_version_tuple())) >= (3, 5)
from groups import Groups
from pathlib import Path

if not Path("task.cfg").is_file():
    print("can't read task.cfg: no such file")
    print("creating empty task.cfg")
    Path("task.cfg").write_text("tests_begin\ntests_end\n")

sources = ["problem.xml.polygon", "problem.xml", "tester.cfg", "task.cfg"]
for source in map(Path, sources):
    groups = Groups.load(source)
    if groups:
        break
    print(f"{source} not found")
else:
    print("error reading groups info from xml/cfg files")
    exit(1)

print(groups)
Path("table_with_group_statistics.tsv").write_text(str(groups))
groups = groups.normalized().to_dict()

if Path("task.cfg.old").is_file():
    print("found old task.cfg, copying skipped")
else:
    print("making a copy of task.cfg")
    Path("task.cfg.old").write_text(Path("task.cfg").read_text())

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

if len(lines) < 2 or not lines[-2].startswith("NEW_GROUP"):
    lines.insert(-1, "NEW_GROUP = 1\n")

dic = {i: (pts, len(tests)) for i, (pts, _, tests) in groups.items()}
i = 0
while dic.get(i):
    points, tests = dic[i]
    for j in range(tests - 1):
        lines.append("-1\n")
    lines.append(str(int(round(points))) + "\n")
    i += 1
lines += lines_end

# update lines from DEPS_BEGIN to DEPS_END in task.cfg
lines_lower = [l.strip().lower() for l in lines]
try:
    deps_beg = lines_lower.index("deps_begin")
    deps_end = lines_lower.index("deps_end")
except ValueError:
    deps_beg, deps_end = len(lines), len(lines) + 1
    lines += ["deps_begin\n", "deps_end\n"]
cfg_deps = []
for group, (_, deps, _) in sorted(groups.items()):
    if len(deps) > 0:
        cfg_deps.append(f"{group}: {' '.join(str(d) for d in deps)}\n")
if cfg_deps:
    lines[(deps_beg + 1) : deps_end] = cfg_deps
else:
    lines[deps_beg : deps_end + 1] = cfg_deps

print("rewriting task.cfg")
with open("task.cfg", "w") as f:
    for line in lines:
        f.write(line)
