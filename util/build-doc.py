import os
import shutil
import json
import re

# 定数の定義
DOC_SRC_DIR = '../doc/src'
NONO_DIR = '../nono'


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


def create_md_files(src_dir, nono_dir):
    """ src_dir およびそのサブディレクトリ下に default.md を作成します。 """
    with open(os.path.join(src_dir, 'SUMMARY.md'), 'w') as file1:
        file1.write('# SUMMARY\n')
        file1.write('[Home](default.md)\n')

        # src/default.md の作成
        with open(os.path.join(src_dir, 'default.md'), 'w') as file:
            file.write("# nonolib\n")
            # include 文を書く
            subdirs = [d for d in os.listdir(
                nono_dir) if os.path.isdir(os.path.join(nono_dir, d))]
            subdirs.sort()
            for subdir in subdirs:
                subdir_path = os.path.join(nono_dir, subdir)
                # H1 タイトルをディレクトリ名で書く
                file.write(f'## {subdir}\n')
                file1.write(f'- [{subdir}]()\n')
                # nono ディレクトリ下のファイル名を取り出してリストに書く
                for filename in sorted(os.listdir(subdir_path)):
                    file_path = os.path.join(subdir_path, filename)
                    if os.path.isfile(file_path):
                        with open(os.path.join(src_dir, filename.split('.')[0] + ".md"), 'w') as file2:
                            rel_file_path = os.path.join("./../", file_path)
                            file2.write(f'# {filename.split(".")[0]}\n')
                            file2.write('```cpp\n')
                            file2.write(
                                f'{{{{#include {rel_file_path}}}}}\n')
                            file2.write('```\n')
                        file.write(
                            f'### [{filename.split(".")[0]}]({"./" + filename.split(".")[0] + ".md"})\n')
                        file1.write(
                            f'    - [{filename.split(".")[0]}]({"./" + filename.split(".")[0] + ".md"})\n')
                        data = extract_and_convert_to_json(file_path)
                        if "brief" in data:
                            file.write(data['brief'])
                            file.write('\n')
                        if "TODO" in data:
                            file.write("\n**TODO**: ")
                            file.write(data['TODO'])
                            file.write('\n')


def remove_directory(dir_path):
    """ 指定されたディレクトリとその内容を削除します。 """
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)


if __name__ == "__main__":
    # 既存のディレクトリを削除
    remove_directory(DOC_SRC_DIR)

    # ディレクトリの作成
    create_directory_structure(DOC_SRC_DIR, NONO_DIR)

    # markdown ファイルの作成
    create_md_files(DOC_SRC_DIR, NONO_DIR)
