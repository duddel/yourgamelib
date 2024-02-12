import re
import argparse


re_finding = re.compile(
    r"^(.*):(\d+):(\d+):\s(warning|error):\s(.+)\s\[(.*)\]$")


def main(args):
    findings_lines = []
    with open(args.file) as f:
        for line in f:
            if re.match(re_finding, line):
                findings_lines.append(line)

    findings_lines = set(findings_lines)
    findings_lines = sorted(findings_lines)

    findings = [
        {
            "path": f[1],
            "line": f[2],
            "column": f[3],
            "type": f[4],
            "message": f[5],
            "checks": f[6]
        } for f in [re.search(re_finding, l) for l in findings_lines]]

    last_path = None
    for f in findings:
        if f["path"] != last_path:
            print(f["path"])
            last_path = f["path"]
        print(
            f"  {f['line']}:{f['column']} ({f['type']}) {f['message']} [{f['checks']}]")


if __name__ == "__main__":
    # command line args
    aPars = argparse.ArgumentParser(
        description="Parse and process clang-tidy output")
    aPars.add_argument("file", type=str, help='clang-tidy log file (input)')
    args = aPars.parse_args()

    main(args)
