#!/usr/bin/env python3

import xml.etree.ElementTree as ET
from collections import namedtuple, defaultdict
from pathlib import Path

# g2i converts group name to group number
g2i = lambda g: 0 if g == "samples" or g is None else int(g.split(".")[0])
Group = namedtuple("Group", "points dependencies tests")
Test = namedtuple("Test", "index points")


def read_groups_from_testset_with_tests(testset):
    tests, groups = testset.find("./tests"), testset.find("./groups") or []
    tests_info = [(g2i(t.attrib.get("group")), t) for t in tests]
    groups_info, groups = {g2i(g.attrib["name"]): g for g in groups}, {}
    for test_index, (group, test_info) in enumerate(tests_info):
        if group not in groups:
            deps = {d for d in groups_info[group] if d.tag == "dependencies"}
            deps = {g2i(d.attrib["group"]) for ds in deps for d in ds}
            each = groups_info[group].attrib["points-policy"] == "each-test"
            policy = "all" if not each or group == 0 else "sum"
            groups[group] = Group(points=policy, dependencies=deps, tests=[])
        points = test_info.attrib.get("points")
        points = int(float(points)) if points else None
        groups[group].tests.append(Test(test_index, points))
    for i, (points, _, tests) in groups.items():
        if points == "all":
            if all(p is not None for (_, p) in tests):
                points = sum(p for (_, p) in tests)
            groups[i] = groups[i]._replace(points=points)
    return groups


def read_groups_from_testset_with_groups(testset):
    groups_info, groups = {g2i(g.attrib["comment"]): g for g in testset}, {}
    for i, group in groups_info.items():
        points = int(group.attrib["group-bonus"])
        tests = [Test(i, t.attrib.get("points")) for i, t in enumerate(group)]
        points = points if any(t.points is None for t in tests) else "sum"
        tests = [Test(i, int(p) if p else None) for i, p in tests]
        deps = set(map(int, group.attrib["require-groups"].split()))
        groups[i] = Group(points, (deps | {0}) - {i}, tests)
    return groups


def read_groups_xml(path):
    if not path.is_file():
        return None
    xml_tree = ET.parse(path)
    tsts = xml_tree.getroot().findall(".//testset[@name='tests']")
    grps = xml_tree.getroot().findall(".//testset[@name='main']")
    groups = read_groups_from_testset_with_tests(tsts[0]) if tsts else None
    groups = read_groups_from_testset_with_groups(grps[0]) if grps else groups
    if groups is None:
        raise Exception("error: no testset with tests was found")
    return groups


def read_groups_cfg(path):
    # read group info in format:
    # <
    # group_1_points, group_1_tests
    # group_2_points, group_2_tests
    # ...
    # >
    #
    # or in format:
    # TESTS_BEGIN
    # -1
    # -0
    # 42
    # ...
    # TESTS_END
    if not path.is_file():
        return None
    with path.open("r") as cfg:
        lines = [l.strip().lower() for l in cfg.read().strip().split("\n")]
    if '>' not in lines:
        info_start, info_end, groups = lines.index("tests_begin"), lines.index("tests_end"), {}
        tests = 1
        for i, points in enumerate(lines[(info_start + 1) : info_end]):
            if not points.startswith('-'):
                groups[len(groups)] = Group(None if int(points) else 0, {}, [Test(None, None) for _ in range(tests)])
                tests = 0
            tests += 1
        return groups
    info_start, info_end, groups = lines.index("<"), lines.index(">"), {}
    for i, line in enumerate(lines[(info_start + 1) : info_end]):
        points, tests = map(int, line.strip().split(","))
        groups[i] = Group(points, {}, [Test(None, None) for _ in range(tests)])
    return groups


def print_groups_table(groups):
    has_deps = any(len(deps) for (_, deps, _) in groups.values())
    header = " GROUPS \t POINTS \t TESTS  " + "\t DEPENDENCIES" * has_deps
    print(header)
    for group, (points, deps, tests) in groups.items():
        row = f"group #{group}:\t{points} points\t{len(tests)} tests"
        if has_deps:
            deps_info = f"depends on {' '.join(map(str, deps))}"
            deps_info = deps_info if deps else "no dependencies"
            row = f"{row} \t{deps_info}"
        print(row)
    print(f"total points: {sum(points for points, _, _ in groups.values())}")
    print(f"total tests: {sum(len(tests) for _, _, tests in groups.values())}")


sources = map(Path, ("problem.xml.polygon", "problem.xml", "tester.cfg", "task.cfg"))
for s in sources:
    print(f"reading {s}...")
    groups = read_groups_xml(s) if s.suffix in [".xml", ".polygon"] else read_groups_cfg(s)
    if groups:
        break
    print(f"{s} not found")
else:
    print("error reading groups info from xml/cfg files")
    exit(1)

if not Path("task.cfg").is_file():
    print("error reading task.cfg: no such file")
    exit(1)

# regroup sum-groups into separate groups
groups, old_groups, o2n = dict(), groups, defaultdict(set)
for i, group in old_groups.items():
    if group.points == "sum":
        deps = {g for d in group.dependencies for g in o2n[d]}
        for test in group.tests:
            o2n[i].add(len(groups))
            groups[len(groups)] = Group(test.points, deps, [test])
    else:
        o2n[i].add(len(groups))
        groups[len(groups)] = group

# read points from "task.cfg" if there are any
# GROUP1_POINTS = points_for_group_1 points_for_group_2 points_for_group_3 ...
with open("task.cfg") as cfg:
    cfg = [l.strip().lower() for l in cfg.read().split("\n")]
    for line in [l for l in cfg if l.startswith('group1_points')]:
        points_for_groups = line.split("=")[1].strip().split(" ")
        for i, group in sorted(groups.items()):
            if group.points is None:
                groups[i] = group._replace(points=int(points_for_groups.pop(0)))

print_groups_table(groups)

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

if not lines[-2].startswith("NEW_GROUP"):
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
for group, (_, deps, _) in groups.items():
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
