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
        deps = map(int, group.attrib["require-groups"].split())
        groups[i] = Group(points, set(map(lambda g: g - 1, deps)), tests)
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


def table2str(table):
    rows = [[str(cell) for cell in row] for row in table]
    widths = [max(len(row[c]) for row in rows) for c in range(len(rows[0]))]
    widths = [(w // 8) * 8 for w in widths]
    rows = [[c.ljust(w) for c, w in zip(row, widths)] for row in rows]
    return "\n".join("\t".join(row) for row in rows)


def print_groups_table(groups):
    has_deps = any(len(deps) for (_, deps, _) in groups.values())
    offset = int(not has_deps and groups[0].points and groups[0].points > 0)
    header = ["group", "points"] + ["depends on"] * has_deps + ["score policy"]
    groups_table, total_points, total_tests = [header], 0, 0
    for group, (points, deps, tests) in groups.items():
        policy = [
                "no points are scored for passing this group",
                "points are scored if all tests are passed",
                "all tests are independently scored",
            ][bool(points) + (points == "sum")]
        points = sum(t.points for t in tests) if points == "sum" else points
        row = [group, points] + [", ".join(str(d) for d in deps)] * has_deps
        groups_table.append(row + [policy])
        total_points += points
        total_tests += len(tests)
    print(table2str(groups_table))
    print(f"total points: {total_points}")
    print(f"total tests: {total_tests}")


def read_missing_group_info(groups):
    with open("task.cfg") as cfg:
        cfg = [l.strip().lower() for l in cfg.read().split("\n")]
    cfg = [l for l in cfg if l.startswith('group1_points')]
    if len(cfg) != 1:
        return groups
    group1_points = map(int, cfg[0].split("=")[1].strip().split(" "))
    groups, old_groups = dict(), groups
    for i, group in list(old_groups.items()):
        if group.points == "sum":
            tests = []
            for test in group.tests:
                if test.points is None:
                    test = test._replace(points=next(group1_points))
                test.append(test)
            group = group._replace(tests=tests)
        groups[i] = group
    return groups


def split_sum_groups(groups):
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
    return groups


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

groups = read_missing_group_info(groups)
groups = split_sum_groups(groups)
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
