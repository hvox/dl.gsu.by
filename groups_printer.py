def table2str(table):
    rows = [[str(cell) for cell in row] for row in table]
    widths = [max(len(row[c]) for row in rows) for c in range(len(rows[0]))]
    widths = [(w // 8) * 8 for w in widths]
    rows = [[c.ljust(w) for c, w in zip(row, widths)] for row in rows]
    return "\n".join("\t".join(row) for row in rows) + "\n"


def is_increasing_with_delta(it, delta):
    it = iter(it)
    try:
        curr = next(it)
        while True:
            nxt = next(it)
            if nxt - curr != delta:
                return False
            curr = nxt
    except StopIteration:
        return True
    except TypeError:
        return False


def list2str(it):
    elements = list(it)
    if is_increasing_with_delta(elements, 1):
        if len(elements) < 3:
            return ", ".join(map(str, elements))
        return f"{elements[0]}..{elements[-1]}"
    ranges = []
    for cur, prev in zip(elements, [None] + elements):
        if any(not isinstance(x, int) for x in (cur, prev)) or cur - prev != 1:
            ranges.append([])
        ranges[-1].append(cur)
    return ", ".join(list2str(rng) for rng in ranges)


def groups_table_to_tsv(groups):
    has_deps = any(len(deps) for (_, deps, _) in groups.values())
    offset = int(isinstance(groups[0].points, int) and groups[0].points > 0)
    groups_table, total_points, total_tests = [], 0, 0
    for group, (points, deps, tests) in sorted(groups.items()):
        policy = [
            "no points are scored for passing this group",
            "points are scored if all tests are passed",
            "all tests are independently scored",
        ][bool(points) + (points == "sum")]
        points = sum(t.points for t in tests) if points == "sum" else points
        row = [group + offset, points, len(tests)]
        row += [list2str(range(total_tests + 1, total_tests + len(tests) + 1))]
        row += has_deps * [list2str(map(lambda g: g + offset, deps))]
        row += [policy]
        groups_table.append(row)
        if points is not None:
            total_points += points
        total_tests += len(tests)
    header = ["group", "points", "tests", "range"]
    header += ["depends on"] * has_deps + ["score policy"]
    total = ["total:", total_points, total_tests] + [""] * (2 + has_deps)
    return table2str([header] + groups_table + [total])
