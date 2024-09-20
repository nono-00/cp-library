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
    subdirs = [d for d in os.listdir(
        nono_dir) if os.path.isdir(os.path.join(nono_dir, d))]
    for subdir in subdirs:
        os.makedirs(os.path.join(src_dir, subdir), exist_ok=True)


def create_md_files(src_dir, nono_dir):
    """ src_dir およびそのサブディレクトリ下に default.md を作成します。 """
    with open(os.path.join(src_dir, 'SUMMARY.md'), 'w') as file:
        file.write('# SUMMARY\n')
        file.write('[Home](default.md)\n')
        subdirs = [d for d in os.listdir(
            nono_dir) if os.path.isdir(os.path.join(nono_dir, d))]
        subdirs.sort()
        for subdir in subdirs:
            file.write(f'[{subdir}]({subdir}/default.md)\n')

    # src/default.md の作成
    with open(os.path.join(src_dir, 'default.md'), 'w') as file:
        file.write("# home\n")
        # include 文を書く
        subdirs = [d for d in os.listdir(
            nono_dir) if os.path.isdir(os.path.join(nono_dir, d))]
        subdirs.sort()
        # include 文を書く
        for subdir in subdirs:
            file.write(f'{{{{#include {subdir}/default.md}}}}\n')

    # 各サブディレクトリ下の default.md の作成
    for subdir in sorted(os.listdir(nono_dir)):
        subdir_path = os.path.join(nono_dir, subdir)
        if os.path.isdir(subdir_path):
            with open(os.path.join(src_dir, subdir, 'default.md'), 'w') as file:
                # H1 タイトルをディレクトリ名で書く
                file.write(f'## {subdir}\n')
                # nono ディレクトリ下のファイル名を取り出してリストに書く
                for filename in sorted(os.listdir(os.path.join(nono_dir, subdir))):
                    file_path = os.path.join(nono_dir, subdir, filename)
                    if os.path.isfile(file_path):
                        file.write(f'### [{filename.split(".")[0]}]()\n')
                        data = extract_and_convert_to_json(file_path)
                        if "brief" in data:
                            file.write(data['brief'])
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
