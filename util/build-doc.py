import os
import shutil

# 定数の定義
DOC_SRC_DIR = '../doc/src'
NONO_DIR = '../nono'

DEST = '../doc/src/'
SRC = '../nono/'


def extract_and_convert_to_json(input_file):
    with open(input_file, 'r') as file:
        lines = file.readlines()

    data = {}
    current_key = None

    for line in lines:
        if line.startswith('///'):
            content = line[3:].strip()
            if ':' in content:
                key, value = map(str.strip, content.split(':', 1))
                data[key] = value
            else:
                # Handle cases where there is no key-value separator
                if current_key:
                    data[current_key] = content
                else:
                    current_key = content

    return data


def create_directory_structure(src_dir, nono_dir):
    """ 指定されたベースディレクトリに必要なディレクトリ構造を作成します。 """
    if not os.path.exists(src_dir):
        os.makedirs(src_dir)


def create_header(header, src, dest):
    return f'{header}({os.path.relpath(src, start=dest)})\n'


def create_file(src_dir, dest_dir, name, base):
    assert os.path.exists(os.path.join(src_dir, name))
    src_file = os.path.join(src_dir, name)
    with open(os.path.join(dest_dir, base + ".md"), 'w') as file:
        file.write(f'# {base}\n')
        file.write('```cpp\n')
        file.write(
            f'{{{{#include {os.path.relpath(src_file, start=dest_dir)}}}}}\n')
        file.write('```\n')
    return extract_and_convert_to_json(src_file)


def create_dir(src_dir, dest_dir, summary, summary_header, depth, defaults, default_header):
    assert os.path.exists(src_dir)
    assert os.path.exists(dest_dir)
    summary.write(summary_header)
    default_path = os.path.join(dest_dir, 'default.md')
    with open(default_path, 'w') as default:
        defaults.append((default, dest_dir))

        for (file, path) in defaults:
            file.write(create_header(default_header, default_path, path))

        for name in sorted(os.listdir(src_dir)):
            if os.path.isdir(os.path.join(src_dir, name)):
                next_src_dir = os.path.join(src_dir, name)
                next_dest_dir = os.path.join(dest_dir, name)
                next_summary_header = f'{"    " * depth}- [{name}]({os.path.relpath(os.path.join(dest_dir, name, "default.md"), start=DEST)})\n'
                next_depth = depth + 1
                next_default_header = f'## [{os.path.relpath(next_dest_dir, start=DEST)}]'
                os.makedirs(next_dest_dir)
                create_dir(next_src_dir, next_dest_dir, summary,
                           next_summary_header, next_depth, defaults, next_default_header)
            else:
                base = '.'.join(name.split('.')[:-1])
                dest_file = os.path.join(dest_dir, base + ".md")
                data = create_file(src_dir, dest_dir, name, base)
                for (file, path) in defaults:
                    file.write(
                        f'### [{base}]({os.path.relpath(dest_file, start=path)})\n')
                    if "brief" in data:
                        file.write(data['brief'])
                        file.write('\n')
                    if "TODO" in data:
                        file.write("\n**TODO**: ")
                        file.write(data['TODO'])
                        file.write('\n')
                summary.write(
                    f'{"    " * depth}- [{base}]({os.path.relpath(os.path.join(dest_dir, base + ".md"), start=DEST)})\n')
        for (file, path) in defaults:
            file.write("\n___\n")
        defaults.pop(-1)


def create_md_files(dest_dir, src_dir):
    """ src_dir およびそのサブディレクトリ下に default.md を作成します。 """
    with open(os.path.join(dest_dir, 'SUMMARY.md'), 'w') as summary:
        summary.write('# SUMMARY\n')
        create_dir(src_dir, dest_dir, summary,
                   '[Home](default.md)\n', 0, [], '# [nonolib]')


def remove_directory(dir_path):
    """ 指定されたディレクトリとその内容を削除します。 """
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)


if __name__ == "__main__":
    # 既存のディレクトリを削除
    remove_directory(DEST)

    # ディレクトリの作成
    create_directory_structure(DEST, SRC)

    # markdown ファイルの作成
    create_md_files(DEST, SRC)
