import xml.etree.ElementTree as ET
from collections import namedtuple, defaultdict
from pathlib import Path

# g2i converts group name to group number
g2i = lambda g: 0 if g == "samples" or g is None else int(g.split(".")[0])

Group = namedtuple("Group", "points dependencies tests")
Test = namedtuple("Test", "points")


def get(lst, index, default_value):
    try:
        return lst[index]
    except IndexError:
        return default_value


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
        groups[group].tests.append(Test(points))
    for i, (points, _, tests) in groups.items():
        if points == "all":
            points = sum(p for (p,) in tests)
            groups[i] = groups[i]._replace(points=points)
    return groups


def read_groups_from_testset_with_groups(testset):
    groups_info, groups = {g2i(g.attrib["comment"]): g for g in testset}, {}
    for i, group in groups_info.items():
        points = int(group.attrib["group-bonus"])
        tests = [test.attrib.get("points") for test in group]
        points = points if any(t is None for t in tests) else "sum"
        tests = list(map(lambda test: Test(int(test) if test else None), tests))
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
    # group_1_points[, group_1_tests]
    # group_2_points[, group_2_tests]
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
    groups = {}
    if ">" not in lines:
        info_start = lines.index("tests_begin")
        info_end = lines.index("tests_end")
        tests_count = 1
        for i, points in enumerate(lines[(info_start + 1) : info_end]):
            if not points.startswith("-"):
                points = None if int(points) != 0 else 0
                tests = [Test(None) for _ in range(tests_count)]
                groups[len(groups)] = Group(points, {}, tests)
                tests_count = 0
            tests_count += 1
        return groups
    info_start, info_end = lines.index("<"), lines.index(">")
    for i, line in enumerate(lines[(info_start + 1) : info_end]):
        group_info = list(map(int, line.strip().split(",")))
        points, tests = group_info[0], get(group_info, 1, 1)
        groups[i] = Group(points, {}, [Test(None) for _ in range(tests)])
    return groups


def read_missing_group_info(groups):
    with open("task.cfg") as cfg:
        cfg = [l.strip().lower() for l in cfg.read().split("\n")]
    cfg = [l for l in cfg if l.startswith("group1_points")]
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
                tests.append(test)
            group = group._replace(tests=tests)
        elif group.points is None:
            group = group._replace(points=next(group1_points))
        groups[i] = group
    return groups
