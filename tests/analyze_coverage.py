from dataclasses import dataclass
import os
import sys
from typing import List


@dataclass
class CoverageStats:
    lines_covered: int = 0
    lines_total: int = 0


def get_coverage_stats(coverage_file: str) -> CoverageStats:
    coverage_stats: CoverageStats = CoverageStats()

    with open(coverage_file, "r", encoding="ascii") as coverage_file:
        for line in coverage_file.read().splitlines():
            if ":" not in line:
                continue

            line_coverage: str = line[: line.index(":")].strip().replace("*", "")

            # skip any lines that shouldn't be included in the count
            if line_coverage == "-":
                continue

            # only count lines with valid coverage stats (excludes symbol name lines)
            if line_coverage.isdigit():
                coverage_stats.lines_covered += 1
                coverage_stats.lines_total += 1
            if line_coverage.count("#") == len(line_coverage):
                coverage_stats.lines_total += 1

    return coverage_stats


def analyze_coverage_dir(target_dir: str) -> CoverageStats:
    all_coverage_stats: List[CoverageStats] = [
        get_coverage_stats(os.path.join(target_dir, coverage_file))
        for coverage_file in os.listdir(target_dir)
        if coverage_file.endswith(".gcov")
    ]

    return CoverageStats(
        lines_covered=sum(
            coverage_stats.lines_covered for coverage_stats in all_coverage_stats
        ),
        lines_total=sum(
            coverage_stats.lines_total for coverage_stats in all_coverage_stats
        ),
    )


if __name__ == "__main__":
    # get the target directory and coverage percentage from the provided arguments
    if len(sys.argv) < 3:
        raise ValueError("Invalid arguments provided")
    target_dir: str = sys.argv[1]
    target_coverage: float = float(sys.argv[2]) / 100

    # analyze what's in the coverage directory
    coverage_stats: CoverageStats = analyze_coverage_dir(target_dir)

    # throw an error if the coverage target wasn't met
    if target_coverage > (coverage_stats.lines_covered / coverage_stats.lines_total):
        raise ValueError(
            "Actual coverage of "
            f"{round(coverage_stats.lines_covered / coverage_stats.lines_total * 100, 2)}% "
            "failed to meet target of "
            f"{round(target_coverage * 100, 2)}%"
        )

    print(
        "[ ✓ PASSED ] Coverage was "
        f"{round(coverage_stats.lines_covered / coverage_stats.lines_total * 100, 2)}%"
    )
