from pathlib import Path

doc_path = Path("./doc")


def front_matter(path: Path):
    text = f"---\n" \
           f"title: {path.stem}\n" \
           f"documentation_of: //{path}\n" \
           f"---\n"
    return text


def do(path: Path):
    with open(doc_path/(path.stem + ".md"), 'w') as f:
        f.write(front_matter(path))


def get(path: Path):
    for p in path.iterdir():
        if p.is_file():
            if p.name[0] == '.':
                continue
            do(p)
        elif p.is_dir():
            get(p)


get(Path("./src"))
