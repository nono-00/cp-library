import os
import shutil

# 定数の定義
DOC_SRC_DIR = '../doc/src/'
NONO_DIR = '../nono/'


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


def create_md_file(src_dir, nono_dir, summary, depth):
    # src/default.md の作成
    with open(os.path.join(src_dir, 'default.md'), 'w') as default:
        if src_dir == DOC_SRC_DIR:
            default.write("# nonolib\n")
        else:
            default.write(f"## {os.path.join('.', src_dir[10:])}\n")

        # include 文を書く
        for subdir in sorted(os.listdir(nono_dir)):
            subdir_path = os.path.join(nono_dir, subdir)
            if os.path.isfile(subdir_path):
                data = extract_and_convert_to_json(subdir_path)
                filename = '.'.join(subdir.split('.')[:-1])
                with open(os.path.join(src_dir, filename + ".md"), 'w') as file:
                    rel_file_path = os.path.join(
                        "../" * (depth + 1), subdir_path)
                    file.write(f'# {filename}\n')
                    file.write('```cpp\n')
                    file.write(
                        f'{{{{#include {"./" + rel_file_path}}}}}\n')
                    file.write('```\n')
                default.write(
                    f'### [{filename}]({"./" + filename + ".md"})\n')
                print(
                    f'{"    " * depth}- [{filename}](.{os.path.join(src_dir[10:], filename + ".md")})')
                summary.write(
                    f'{"    " * depth}- [{filename}](.{os.path.join(src_dir[10:], filename + ".md")})\n')
                if "brief" in data:
                    default.write(data['brief'])
                    default.write('\n')
                if "TODO" in data:
                    default.write("\n**TODO**: ")
                    default.write(data['TODO'])
                    default.write('\n')
            else:
                os.makedirs(os.path.join(src_dir, subdir))
                print(
                    f'{"    " * depth}- [{subdir}](.{os.path.join(src_dir[10:], subdir, "default.md")})')
                summary.write(
                    f'{"    " * depth}- [{subdir}](.{os.path.join(src_dir[10:], subdir, "default.md")})\n')
                create_md_file(os.path.join(src_dir, subdir),
                               os.path.join(nono_dir, subdir), summary, depth + 1)
                default.write(
                    f'{{{{#include {os.path.join(subdir, "default.md")}}}}}\n')
        default.write("___ \n")


def create_md_files(src_dir, nono_dir):
    """ src_dir およびそのサブディレクトリ下に default.md を作成します。 """
    with open(os.path.join(src_dir, 'SUMMARY.md'), 'w') as summary:
        summary.write('# SUMMARY\n')
        summary.write('[Home](./default.md)\n')

        create_md_file(src_dir, nono_dir, summary, 0)


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
