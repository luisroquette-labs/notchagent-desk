from pathlib import Path
import io
import re
import subprocess
import tokenize


ROOT = Path(__file__).resolve().parents[1]
FILES = subprocess.check_output(["git", "ls-files", "-z"], cwd=ROOT).decode().split("\0")
failures = []


def slash_comments(text, style):
    found = []
    pairs = {"(": ")", "[": "]", "{": "}"}

    def string_at(i):
        raw = re.match(r'#+("""|")', text[i:]) if style == "swift" else None
        hashes = "#" * (len(raw.group(0)) - len(raw.group(1))) if raw else ""
        quote = raw.group(1) if raw else ('"""' if style == "swift" and text.startswith('"""', i) else text[i])
        i += len(hashes) + len(quote)
        while i < len(text):
            if style == "swift" and text.startswith("\\" + hashes + "(", i):
                i = code_at(i + 2 + len(hashes), ")")
            elif style == "js" and quote == "`" and text.startswith("${", i):
                i = code_at(i + 2, "}")
            elif text.startswith(quote + hashes, i):
                return i + len(quote) + len(hashes)
            elif text[i] == "\\" and not hashes:
                i += 2
            else:
                i += 1
        return i

    def regex_at(i):
        i += 1
        in_class = False
        while i < len(text):
            if text[i] == "\\":
                i += 2
            elif text[i] == "[":
                in_class = True
                i += 1
            elif text[i] == "]":
                in_class = False
                i += 1
            elif text[i] == "/" and not in_class:
                return i + 1
            else:
                i += 1
        return i

    def code_at(i, closing=None):
        previous = ""
        while i < len(text):
            char = text[i]
            if closing and char == closing:
                return i + 1
            if closing and char in pairs:
                i = code_at(i + 1, pairs[char])
                previous = ")"
                continue
            if style == "swift" and re.match(r'#+"', text[i:]):
                i = string_at(i)
                previous = '"'
                continue
            if style == "cpp" and char == "'" and i > 0 and i + 1 < len(text) and text[i - 1] in "0123456789abcdefABCDEF" and text[i + 1] in "0123456789abcdefABCDEF":
                i += 1
                continue
            if char in ('"', "'", "`"):
                i = string_at(i)
                previous = char
                continue
            if style != "css" and text.startswith("//", i):
                found.append(i)
                end = text.find("\n", i)
                i = len(text) if end < 0 else end + 1
                continue
            if text.startswith("/*", i):
                found.append(i)
                depth = 1
                i += 2
                while i < len(text) and depth:
                    if style == "swift" and text.startswith("/*", i):
                        depth += 1
                        i += 2
                    elif text.startswith("*/", i):
                        depth -= 1
                        i += 2
                    else:
                        i += 1
                continue
            if style == "js" and char == "/" and (
                previous in "=([{:,;!?&|"
                or text[:i].rstrip().endswith("=>")
                or re.search(r"\b(?:return|throw|case|yield|await)\s*$", text[:i])
            ):
                i = regex_at(i)
                previous = "/"
                continue
            if not char.isspace():
                previous = char
            i += 1
        return i

    code_at(0)
    return found


def hash_comments(text):
    found = []
    pos = 0
    for line in text.splitlines(keepends=True):
        quote = ""
        escaped = False
        escaped_space = False
        for i, char in enumerate(line):
            if escaped:
                escaped = False
                escaped_space = char.isspace()
                continue
            if not char.isspace() and char != "#":
                escaped_space = False
            if char == "\\" and quote != "'":
                escaped = True
                continue
            if quote:
                if char == quote:
                    quote = ""
            elif char in ("'", '"'):
                quote = char
            elif char == "#" and (i == 0 or line[i - 1].isspace()) and not escaped_space:
                if not (pos == 0 and line.startswith("#!")):
                    found.append(pos + i)
                break
        pos += len(line)
    return found


assert not slash_comments('auto url = "https://example.test/a//b";', "cpp")
assert slash_comments('auto x = 1; // remove', "cpp")
assert slash_comments("constexpr int n = 1'000; // remove", "cpp")
assert not slash_comments('let value = #"// literal"#', "swift")
assert slash_comments('a { color: red; /* remove */ }', "css")
assert not hash_comments('#!/usr/bin/env bash\necho "# literal"\n')
assert not hash_comments('echo "\\\" # literal"\n')
assert not hash_comments('echo foo\\ #literal\n')
assert hash_comments('# remove\n')


for name in FILES:
    if not name or name.startswith(".git/"):
        continue
    path = ROOT / name
    if not path.is_file():
        continue
    text = path.read_text() if path.suffix in {".swift", ".ino", ".h", ".hpp", ".cpp", ".cs", ".js", ".css", ".html", ".py", ".sh", ".yml", ".yaml", ".csv"} else ""
    if path.suffix == ".swift":
        offsets = slash_comments(text, "swift")
        if name == "sdk/apple/Package.swift" and re.match(r"// swift-tools-version: \d+\.\d+(?:\.\d+)?(?:\n|$)", text):
            offsets = [offset for offset in offsets if offset != 0]
    elif path.suffix in {".ino", ".h", ".hpp", ".cpp", ".cs"}:
        offsets = slash_comments(text, "cpp")
    elif path.suffix == ".js":
        offsets = slash_comments(text, "js")
    elif path.suffix == ".css":
        offsets = slash_comments(text, "css")
    elif path.suffix == ".html":
        offsets = [match.start() for match in re.finditer(r"<!--[\s\S]*?-->", text)]
        for block in re.finditer(r"<(style|script)\b[^>]*>(.*?)</\1>", text, re.I | re.S):
            style = "css" if block.group(1).lower() == "style" else "js"
            start = block.start(2)
            offsets.extend(start + offset for offset in slash_comments(block.group(2), style))
    elif path.suffix == ".py":
        offsets = [token.start[0] for token in tokenize.generate_tokens(io.StringIO(text).readline) if token.type == tokenize.COMMENT and not (token.start[0] == 1 and token.string.startswith("#!"))]
        failures.extend(f"{name}:{line}" for line in offsets)
        continue
    elif path.suffix in {".sh", ".yml", ".yaml", ".csv"}:
        offsets = hash_comments(text)
    else:
        continue
    failures.extend(f"{name}:{text.count(chr(10), 0, offset) + 1}" for offset in offsets)

if failures:
    raise SystemExit("Comments are forbidden (baseline: 0):\n" + "\n".join(failures))
print("Comment lint passed (baseline: 0).")
