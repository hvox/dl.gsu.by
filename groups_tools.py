from groups_reader import Group

def fix_group_enumeraction(groups):
    o2i, new_groups = {}, {}
    for i, (j, group) in enumerate(sorted(groups.items())):
        o2i[j] = i
        points, deps, tests = group
        deps = {o2i[d] for d in deps}
        new_groups[i] = Group(points, deps, tests)
    return new_groups
