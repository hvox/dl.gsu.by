import groups_reader
import groups_printer
import groups_tools
from pathlib import Path
from collections import namedtuple, defaultdict

Group = namedtuple("Group", "points dependencies tests")


class Groups:
    def __init__(self, groups):
        self.groups = groups_tools.fix_group_enumeraction(groups)

    def to_dict(self):
        return self.groups

    def normalized(self):
        groups, old_groups = dict(), self.groups
        o2n = {group: set() for group in range(len(old_groups))}
        for i, group in old_groups.items():
            if group.points == "sum":
                deps = {g for d in group.dependencies for g in o2n[d]}
                for test in group.tests:
                    o2n[i].add(len(groups))
                    groups[len(groups)] = Group(test.points, deps, [test])
            else:
                o2n[i].add(len(groups))
                groups[len(groups)] = group
        return Groups(groups)

    def __str__(self):
        return groups_printer.groups_table_to_tsv(self.groups)

    def __repr__(self):
        return f"Groups({self.groups})"

    @staticmethod
    def load(source):
        source, groups = Path(source), None
        if not groups and source.is_dir():
            groups = groups_reader.guess_groups_based_on_tests_directory(source)
        if not groups and source.suffix in [".xml", ".polygon"]:
            groups = groups_reader.read_groups_xml(source)
        if not groups and source.suffix in [".cfg"]:
            groups = groups_reader.read_groups_cfg(source)
        if not groups:
            return None
        return Groups(groups_reader.read_missing_group_info(groups))
